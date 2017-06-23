#ifndef DEF_PRESSET_H
#define DEF_PRESSET_H

#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "../io/io.h"
#include "../core/Modules.h"

using namespace std;

/*
*	Each Module_Node contains an IO_Accessor_List
*/
typedef std::vector<IO_Accessor*> IO_Accessor_List;

typedef struct{
	
	short s;
	short sv;
	short sp;
	short t;
	short tv;
	short tp;
}Connection;
/*
*	Contain list of all connection for a Module_Node
*/
typedef std::vector<Connection> Connection_List;

/*
*	Class used to store Module and its accessors and connections
*/
class Module_Node{
	
	public :
		
		/*
		*	Main constructor called with pointer to the module
		*/
		Module_Node(Module* mod);
		~Module_Node();
		
		/*
		*	Return pointer to the module
		*/
		Module* 		get_module() const;
		
		/*
		*	Add, remove or get the accessor at index i
		*/
		void 			accessor_add(IO_Accessor *accessor);
		void 			accessor_remove(int i);
		IO_Accessor* 	accessor_get(int i) const;
		IO_Accessor_List* accessor_get_list();
		/*
		*	Update the io_accessor list with the given value table
		*/
		void			accessor_update(int *potar_tab);
		
		/*
		*	Manage connections
		*	Add : Add a connection, format = XX,XX,XX,XX,XX --> source voice, source port, target, target voice, target port
		*	Remove and get : i : index of  the connection
		*/
		void			connection_add(Connection c);
		void			connection_remove(int i);
		Connection 		connection_get(int i) const;
		Connection_List*	connection_get_list();
		
		Module *_mod;
		IO_Accessor_List _io_accessor_list;
		Connection_List _connection_list;
};

/*
*	Main map for store modules nodes
*/
typedef std::vector<Module_Node*> Module_Node_List;

/*
*	---------------------------------------------------------------------------
*	Presset Managing stuff
*	---------------------------------------------------------------------------
*/
static const string node_root("Preset");
static const string node_module("Module");
static const string node_param("Params");
static const string node_accessor("Accessor");
static const string node_potentiometer("Potentiometer");
static const string node_connection("Connection");


int save_presset(string name, string version, Module_Node_List list);

#endif