#ifndef VFS_H
#define VFS_H
#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<string>
#include<ctime>
#include<fstream>
#include "inode.hpp"
#include "queue.hpp"
#include "vector.hpp"
using namespace std;

class VFS
{
	private:
		Inode *root;				//root of the VFS
		Inode *curr_inode;			//current iNode
		Inode *prev_inode;			//previous iNode
		Queue<Inode*> bin;			//bin containing the deleted Inodes
		Queue<string> bin_paths;	//paths of the items in bin
		Queue<Inode*> bin_parents;  //parents of the items in bin
	
	public:	 	
		//Required methods
		VFS();	
		void help();						
		string pwd(Inode* node = nullptr) const;
		void ls(string extension);						
		void mkdir(string folder_name);
		void touch(string file_name, unsigned int size);
		void cd(string path);
		void rm(string name);
		void size(string path);
		void showbin();
		void emptybin();
		void exit();

		//My helper methods
		string currentTime();
		bool correct_name(string name);
		bool repeated_name(string name);
		Inode* getNode(string path);
		Inode* getParent(string path);
		int getSize(Inode* inode);
		
		//My Optional Mehods
		void find(string name);
		void find_helper(Inode *ptr, string name);
		void mv(string file, string folder);
		void recover();

};
//===========================================================
#endif
