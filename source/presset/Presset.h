#ifndef DEF_PRESSET_H
#define DEF_PRESSET_H

#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "../modules/Drive.h"
#include "../modules/LFO.h"
#include "../modules/Ringmod.h"
#include "../modules/Delay.h"
#include "../modules/Tonestack.h"
#include "../modules/Reverb.h"
#include "../modules/Chorus.h"

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
typedef std::vector<Module_Node*> Module_List;
class Module_Node_List{
	
	public :
	//	Main function used to create new module
	int add_module(MODULE_TYPE mod, int v);

	//	Remove module at idx
	int del_module(int idx);

	/*
	*	Connect module[source_idx]'s port[is] to module[target_idx]'s port[id]
	*	source_idx = -1 for System_Capture ports
	*	target_idx = -1 for System_Playback ports
	*/
	int add_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id);
	int del_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id);

	/*
	*	Create a link between a potentiometer and param[param_idx] of module[target]
	*	See io/io.h --> IO_Accessor
	*/
	int add_accessor(int target, int target_voice, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);

	Module_List list_;
	int count_;
};

/*
*	Return the name of source's port number is, or System Capture port [is] if source is NULL
*/
const char* get_source_name(Module *source, int v, int is, Module *target);

/*
*	Return the name of target's port number is, or System Playback port [is] if target is NULL
*/
const char* get_target_name(Module *target, int v, int id, Module *source);

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

int load_presset(string name, string version, Module_Node_List *list);

#endif