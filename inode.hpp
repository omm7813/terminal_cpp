#ifndef INODE_H
#define INODE_H
#include<cstdlib>
#include<string>
#include<ctime>
#include "vector.hpp"

using namespace std;
enum {File=0,Folder=1};

class Inode
{
	private:
		string name;				//name of the Inode
		bool type;					//type of the Inode 0 for File 1 for Folder
		unsigned int size;			//size of current Inode
		string cr_time; 			//time of creation
		Vector<Inode*> children;	//Children of Inode
		Inode* parent; 				//link to the parent 

	public:
		Inode() {}

		Inode(string i_name, Inode* i_parent, bool i_type, int i_size, string i_cr_time) //: name(name),type(type),size(size),cr_time(cr_time),parent(parent)
		{ 	
			name = i_name;
			type = i_type;
			size = i_size;
			cr_time = i_cr_time;
			parent = i_parent;
		}
		
		friend class VFS;
};

#endif