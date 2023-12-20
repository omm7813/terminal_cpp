#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<string>
#include<ctime>
#include<fstream>

#include "vfs.hpp"
#include "inode.hpp"
#include "vector.hpp"
#include "queue.hpp"


#define MAXBIN 10
using namespace std;

string VFS::currentTime() {
    // Obtain current time
    std::time_t t = std::time(nullptr);
    // Convert to local time format
    std::tm *tm_ptr = std::localtime(&t);

    // tm_mon gives month from 0 to 11, add 1 for a human-readable format
    int month = tm_ptr->tm_mon + 1;
    int day = tm_ptr->tm_mday;
    // tm_year gives years since 1900
    int year = tm_ptr->tm_year + 1900;

    //Combine all of them in 1 string to return it
    string currentTime = to_string(day) + "-" + to_string(month) + "-" + to_string(year); 

    return currentTime;
}

bool VFS::correct_name(string name) {
    if (name.length() == 0) { return false; }
    bool has_extension = false;
    for (size_t i = 0; i < name.length(); ++i) {
        if (name[i] == '.') {
            // Ensure that the period is not at the start or end and there's only one
            if (has_extension || i == 0 || i == name.length() - 1) {
                return false;
            }
            has_extension = true;
        } else if (!std::isalnum(static_cast<unsigned char>(name[i]))) {
            // If it's not a period, it must be alphanumeric
            return false;
        }
    }
    return true;
}

bool VFS::repeated_name(string name) {
    bool repeated = false;
    //Compare the name to all of the names exists under the same folder
    for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it ){
        if((*it)->name == name) {
            repeated = true;
            break; // If a match is found, no need to continue checking
        }
    }
    return repeated;
}


VFS::VFS() {
    //initialize the root of the VF
    root = new Inode("root", nullptr, Folder, 0, currentTime());
    //create a vector to store the children of the root
    root->children = Vector<Inode*>();
    //initialize current and prev inodes
    curr_inode = root;
    prev_inode = nullptr;
    //Create the bins to store the information of a deleted inode
    Queue<string> bin_paths(MAXBIN);
    Queue<Inode*> bin(MAXBIN);
    Queue<Inode*> bin_parents(MAXBIN);
}

void VFS::help() {
    //print the available commands and their purposes
    cout << "Available Commands:\n";
    cout << "pwd                - Prints the path of the current directory.\n";
    cout << "ls                 - Displays the contents of the current directory.\n";
    cout << "mkdir <foldername> - Creates a new directory under the current one.\n";
    cout << "touch <filename> <size> - Creates a new file with a specified size.\n";
    cout << "cd <path>          - Changes the current directory to the specified path.\n";
    cout << "find <name>        - Searches for files or directories with the specified name.\n";
    cout << "mv <filename> <foldername> - Moves a file to the specified directory.\n";
    cout << "rm <name>          - Removes a file or directory and places it in the bin.\n";
    cout << "size <name>        - Displays the size of the specified file or directory.\n";
    cout << "emptybin           - Empties the bin of deleted items.\n";
    cout << "showbin            - Shows the oldest item in the bin.\n";
    cout << "recover            - Restores the oldest item from the bin.\n";
    cout << "exit               - Exits the program and saves the state.\n";
}

//Function to returnt the path of a file/folder as string
string VFS::pwd(Inode* node) const {
    //if nothing is passed, make the default the current inode
    if (node == nullptr) { node = curr_inode; }
    //create a temp inode for iteration
    Inode* temp = node;
    //initialize the path 
    string path = "";
    //if the node is the root itself, save the name as /
    if(temp == root) {path = "/";}
    while (temp != root) {
        //if not the root, store the name of its parent untill we reach the root
        path = "/" + temp->name + path;
        temp = temp->parent;
    }
    //return the path
    return path;
}

//Function to print the children of a current folder
// Function definition: ls() in VFS (Virtual File System) class to print the children of the current folder
void VFS::ls(string extention) {
    // Check if the provided extension is empty, indicating a normal listing
    if(extention.empty()) {
        // Iterate over the children of the current inode (directory or file)
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            Inode* current = *it; // Get the current inode from the iterator
            if (current->type == 0) {
                // If it's a file (type 0), print its details: type, name, creation time, and size
                cout << "File" << setw(15) << current->name << setw(15) << current->cr_time << setw(10) << current->size << "bytes" << endl;
            }
            else {
                // If it's a folder, print its details: type, name, creation time, and size
                cout << "dir" << setw(15) << current->name << setw(15) << current->cr_time << setw(10) << current->size << "bytes" << endl;
            }
        }
    } 
    // Check if the extension is "sort", indicating a sorted listing
    else if (extention == "sort") {
        bool swapped;
         do {
            swapped = false;
            // Iterate over the children for sorting
            for (auto it = curr_inode->children.begin(); it != curr_inode->children.end(); ) {
                auto next_it = it; // Create an iterator for the next element
                ++next_it; // Increment to the next element
                // If the next element exists and its size is larger, swap them
                if (next_it != curr_inode->children.end() && (*it)->size < (*next_it)->size) {
                    Inode* temp = *it; // Temporary storage for the swap
                    *it = *next_it;
                    *next_it = temp;
                    swapped = true; // Indicate that a swap has occurred
                }
                if (next_it == curr_inode->children.end()) {
                    break; // Prevent iterating past the end
                }
                ++it; // Move to the next element
            }
        } while (swapped); // Repeat until no swaps are needed
        
        // After sorting, print the children similar to the first block
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            Inode* current = *it;
            if (current->type == 0) {
                cout << "File" << setw(10) << current->name << setw(15) << current->cr_time << setw(10) << current->size << "bytes" << endl;
            }
            else {
                cout << "dir" << setw(10) << current->name << setw(15) << current->cr_time << setw(10) << current->size << "bytes" << endl;
            }
        }

    } else {
        // If an invalid extension is provided, notify the user
        throw runtime_error("Invalid extension. Either use 'ls' or 'ls sort'.");
    }

}

//function to create new directory under the current directory
void VFS::mkdir(string foldername) {
    // Check if the folder name is valid by calling correct_name function
    if(!correct_name(foldername)) {
        // If the name is invalid, print an error message
        throw runtime_error("Wrong naming. Folder names can't be empty and should be alphanumeric only (i.e. comprises the letters A to Z, a to z, and the digits 0 to 9) without whitespaces or special characters, except the period “.” that can be used for file extensions.");
    } 
    // Check if the folder name already exists in the current directory by calling repeated_name function
    else if(repeated_name(foldername)) {
        // If the name already exists, print an error message
        throw runtime_error("Foldername already exists. Please try again with a different name.");
    } 
    else {
        // If the name is valid and not repeated, create a new Inode for the folder
        Inode* folder = new Inode(foldername, curr_inode, Folder, 10, currentTime());
        // Add the new folder Inode to the children of the current Inode
        curr_inode->children.push_back(folder);
        // Initialize the children vector of the new folder Inode
        folder->children = Vector<Inode*>();
    }
}

//Function to create new Files under the current directory
void VFS::touch(string filename, unsigned int size) {
    // Check if the file name is valid by calling correct_name function
    if(!correct_name(filename)) {
        // If the name is invalid, print an error message
        throw runtime_error("Wrong naming. File names can't be empty and should be alphanumeric only (i.e. comprises the letters A to Z, a to z, and the digits 0 to 9) without whitespaces or special characters, except the period “.” that can be used for file extensions.");
    } 
    // Check if the file name already exists in the current directory by calling repeated_name function
    else if (repeated_name(filename)) {
        // If the name already exists, print an error message
        throw runtime_error("File name already exists. Please try again with a different name.");
    } 
    else {
        // If the name is valid and not repeated, create a new Inode for the file
        Inode* file = new Inode(filename, curr_inode, File, size, currentTime());
        // Add the new file Inode to the children of the current Inode
        curr_inode->children.push_back(file);
        // Initialize the children vector of the new file Inode (although files typically don't have children Inodes)
        file->children = Vector<Inode*>();
    }
}

Inode* VFS::getNode(string path) {
    //check if it is the root 
    if (path[0] == '/' && path.length() == 1) { return root; }
    // If the path starts with '/', we start from the root, otherwise from the current directory.
    Inode* node = (path[0] == '/') ? root : curr_inode;

    // If the path starts with '/', skip the first character.
    size_t pos = (path[0] == '/') ? 1 : 0;

    // Current directory or file name being parsed.
    string name;

    // Parse the path by iterating over each character.
    for (; pos <= path.length(); ++pos) {
        // Check if we reached the end of a directory/file name or the end of the path.
        if (pos == path.length() || path[pos] == '/') {
            // Avoid processing empty names (which could happen with consecutive '/' characters).
            if (!name.empty()) {
                // Look for the directory/file in the current node's children.
                bool found = false;
                for (auto it = node->children.begin(); it != node->children.end(); ++it) {
                    if ((*it)->name == name) {
                        node = *it; // Move to the found node.
                        found = true;
                        break;
                    }
                }
                // If the directory/file was not found, return nullptr.
                if (!found) return nullptr;
                
                // Reset the name for the next directory/file.
                name.clear();
            }
        } else {
            // Accumulate characters to the current directory/file name.
            name += path[pos];
        }
    }

    // Return the node that corresponds to the last directory/file name found.
    return node;
}

Inode* VFS::getParent(string path) {
    // If the path starts with '/', we start from the root, otherwise from the current directory.
    Inode* node = (path[0] == '/') ? root : curr_inode;
    // Parent node initialized to the root or current directory as appropriate
    Inode* parent = (path[0] == '/') ? root : curr_inode;

    // If the path starts with '/', skip the first character.
    size_t pos = (path[0] == '/') ? 1 : 0;

    // Current directory or file name being parsed.
    string name;

    // Parse the path by iterating over each character.
    for (; pos <= path.length(); ++pos) {
        // Check if we reached the end of a directory/file name or the end of the path.
        if (pos == path.length() || path[pos] == '/') {
            // Avoid processing empty names (which could happen with consecutive '/' characters).
            if (!name.empty()) {
                // Look for the directory/file in the current node's children.
                bool found = false;
                for (auto it = node->children.begin(); it != node->children.end(); ++it) {
                    if ((*it)->name == name) {
                        parent = node; // Update parent to the current node
                        node = *it;    // Move to the found node.
                        found = true;
                        break;
                    }
                }
                // If the directory/file was not found, return nullptr.
                if (!found) return nullptr;
                
                // Reset the name for the next directory/file.
                name.clear();
            }
        } else {
            // Accumulate characters to the current directory/file name.
            name += path[pos];
        }
    }

    // Return the parent of the node that corresponds to the last directory/file name found.
    return parent;
}


void VFS::cd(string path) {
    //check the extension after cd prompt
    //if "cd ..", then move the current inode to the parent inode
    if (path == "..") {
        //check if it is the root, return error
        if (curr_inode == root) { cout << "This is the main folder." << endl; return; } 
        else {   
        prev_inode = curr_inode;
        curr_inode = curr_inode->parent;
        }
    } else if (path == "-") { //if "cd -", move to the last working directory
        //check if there is no last working directory 
        if(prev_inode == nullptr) { throw runtime_error("No last working directory"); } 
        else {
        Inode* temp = curr_inode;
        curr_inode = prev_inode;
        prev_inode = temp;
        }
    } else if (path.empty()) { //if "cd", move to the root
        prev_inode = curr_inode;
        curr_inode = root;
    } else if (path[0] == '/') { //if " cd /path" move to the specified path
        //call the function getNode to get a pointer to the iNode
        Inode* Inode = getNode(path);
        //check if it is found or not
        if (Inode == nullptr) {throw runtime_error("Path doesn't exist"); }
        //check if it is file or folder
        else if (Inode->type == 0) { throw runtime_error("Cannot move to a file."); } 
        //if folder, move the current node to the the Inode specified by the path
        else {
            prev_inode = curr_inode;
            curr_inode = Inode;
        }
    } else { //if none of the above, then it is a name of a file or folder
        //iterate through the children of the current folder to find the Inode 
        bool found = false;
        Inode* newInode = nullptr;
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            if ((*it)->name == path) { 
                newInode = *it;
                found = true;
                break; //The name cannot be repeated in the same folder
            }
        }
        // if not child of the current node, throw an error and exit 
        if (!found) {
            throw runtime_error("The name provided is not a folder inside the current folder");
        }
        //if file, throw and error and exit 
        else if (newInode->type == 0) {
            throw runtime_error("The name provided is not a folder inside the current folder");
        }
        else {
            prev_inode = curr_inode;
            curr_inode = newInode;    
        }   
    }
    //print the new path
    string new_path = pwd();
    cout << new_path << endl;
}

//recursive method to check if a given child is present under specific Inode or not
void VFS::find_helper(Inode* inode, string name) {
    if(inode->name == name) {
        //if the name is found, print its path
        cout << pwd(inode) << endl;
    }
    for (Vector<Inode*>::Iterator it = inode->children.begin(); it != inode->children.end(); ++it){
        //Do the same for each child
        find_helper(*it, name);
    }
}

void VFS::find(string name) {
    //call the find helper function to print all the files/folders that has this name
    find_helper(root, name);
}

void VFS::mv(string file, string folder) {
    //initialize the variables needed
    bool file_found = false, folder_found = false;
    Inode* file_inode;
    Inode* folder_inode;
    Inode* file_parent;
    int index = 0, file_index = 0;  

    //check if it is absolute path for the file or not 
    if (file[0] == '/') {
        file_inode = getNode(file);
        //check if the file exists
        if (file_inode == nullptr) { throw runtime_error("File path doesn't exist"); } 
        file_found = true;
        //if exists, find the index of the file with respect to the vector of the parent such that the function 
        //erase can be used
        file_parent = file_inode->parent;
        for (Vector<Inode*>::Iterator it = file_parent->children.begin(); it != file_parent->children.end(); ++it) {
            if((*it)->name == file_inode->name && (*it)->type == File) {
                file_index = index;
            } 
            index ++;
        }        
    } else {
        // if not abolute path:
        file_parent = curr_inode;
        //Iterate through the children of the current inode to search for the file
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            if((*it)->name == file && (*it)->type == File) {
                file_inode = *it; //if the file exists, store ptr to its inode
                file_found = true;
                file_index = index;
            } 
            index ++;
        }
    }

    //check if it is absolute path for the folder or not 
    if (folder[0] == '/') {
        folder_inode = getNode(folder);
        //check if the file exists
        if (folder_inode == nullptr) { throw runtime_error("Folder path doesn't exist"); } 
        else { 
            folder_found = true;
        }
    } else {
    // if not abolute path:
    //Iterate through the children of the parent inode to search for the folder
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            if ((*it)->name == folder && (*it)->type == Folder) {
                folder_inode = *it;  //if the folder exists, store ptr to its inode
                folder_found = true;
            } 
    }

    }
    //Verify that the file/folder exists
    if (!file_found || file_inode->type != File) { throw runtime_error("The file name entered doesn't exist"); }
    if (!folder_found || folder_inode->type != Folder) { throw runtime_error("The folder name entered doesn't exist"); }
    if (!file_found || !folder_found || file_inode->type != File || folder_inode->type != Folder) { return; }

    //Add the file to the children of the new folder
    folder_inode->children.push_back(file_inode);
    //update the parent of the moved file/folder
    file_inode->parent = folder_inode;
    //remove the moved folder/file from its old dir
    file_parent->children.erase(file_index);
}

void VFS::rm(string name) {
    //verify that the folder/file is inside the current node. If not, print to the user and then close.
    //If found, store a ptr to it
    int index = 0, index_file = 0;
    Inode* inode;
    Inode* parent;
    bool found = false;
    //check if it is absolute path or not
    if (name[0] != '/') {
        //if not a path, then it is under the current directory
        parent = curr_inode;
        for (Vector<Inode*>::Iterator it = parent->children.begin(); it != parent->children.end(); ++it) {
            if ((*it)->name == name) { 
                //if the name is found, save the node and exit
                inode = *it;
                index_file = index;
                found = true;
                break;
            }
            ++index;
        }
    } else {
        //if it is a path, find the node referred by this path
        inode = getNode(name);
        //check if exists or not
        if (inode == nullptr) { throw runtime_error("The path doesn't exist"); } 
        else { 
            found = true;
        }
        //if exists, find the index of the file with respect to the vector of the parent such that the function 
        //erase can be used
        parent = inode->parent;
        for (Vector<Inode*>::Iterator it = parent->children.begin(); it != parent->children.end(); ++it) {
            if((*it)->name == inode->name && (*it)->type == File) {
                index_file = index;
            } 
            index ++;
        }     
    }

    //check if it is found or not
    if (!found) { throw runtime_error("The folder/file name doesn't exist"); }
    
    if (bin.isFull()) {
        throw runtime_error("Bin is full, cannot remove more items.");
    } 

    //Put the inode into the bin and save its path, old parent 
    bin.enqueue(inode);
    bin_paths.enqueue(pwd(inode));
    bin_parents.enqueue(inode->parent);
    //update its parent and erase it from its old directory
    inode->parent = nullptr;
    parent->children.erase(index_file);
}


int VFS::getSize(Inode* inode) {
    // Base case: if the inode is null, return 0
    if (inode == nullptr) {
        return 0;
    }

    // If the inode is a folder, calculate the size of all children
    int totalSize = inode->size; // Initialize with the folder's own size (10)
    for (Vector<Inode*>::Iterator it = inode->children.begin(); it != inode->children.end(); ++it) {
        totalSize += getSize(*it); // Recursively add the size of each child
    }

    // Return the total size
    return totalSize;
}

void VFS::size(string name) {
    Inode* inode;
    bool found = false;
    //check if the input is absolute name or not
    if (name[0] != '/') {
    //if not a path, then it is under the current directory
        for (Vector<Inode*>::Iterator it = curr_inode->children.begin(); it != curr_inode->children.end(); ++it) {
            if ((*it)->name == name) { 
                //if the name is found, save the node and exit
                inode = *it;
                found = true;
                break;
            }
        }
    } else {
            //if it is a path, find the node referred by this path
            inode = getNode(name);
            //check if exists or not
            if (inode == nullptr) { throw runtime_error("The path doesn't exist"); } 
            else { 
                found = true;
            }    
        }

    if (!found) { throw runtime_error("The folder/file name doesn't exist"); }
    //if it is a file, just print the size of it
    if (inode->type == File) { cout << inode->size << endl; }
    //if it is a folder, print the total size of it
    else {
        //Perform post-order iteration for the folder to calculate the size of all of its children
        cout << getSize(inode) << " bytes" << endl;
    }
    
}

//function to show the first deleted element in the bin
void VFS::showbin() {
    //Notify the user if the bin is empty
    if (bin_paths.isEmpty()) { cout << "The bin is empty" << endl;} else {
    //If not empty, print the details of the first removed file/folder
    cout << "Next Element to remove: " << bin_paths.front_element() << "  (" << bin.front_element()->size << " bytes, " << bin.front_element()->cr_time << ")" << endl; 
    }
}

//function to delete all the elements inside the bin, without recovering any
void VFS::emptybin() {
    //while the bin is not empty, keep removing the front element
    while(!bin.isEmpty()) {bin.dequeue();}
    while(!bin_paths.isEmpty()) {bin_paths.dequeue();}
}

void VFS::recover() {
    //Find the inode need to be recovered
    Inode* to_recover = bin.front_element();
    //check if the old parent still exists
    Inode* parent = getNode(pwd(bin_parents.front_element()));
    if (parent == nullptr) { throw runtime_error("The parent of the file/folder no longer exists"); }
    //push the element back to its parent 
    parent->children.push_back(to_recover);
    //update the parent of the node
    to_recover->parent = parent;
    //remove the element from the bins
    bin.dequeue();
    bin_paths.dequeue();
    bin_parents.dequeue();
}

void VFS::exit() {
    // Print a goodbye message 
    cout << "Exiting the Virtual File System. Goodbye!" << endl;
    // Exit the program
    std::exit(0); 
}
