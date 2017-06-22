#ifndef DEF_MAIN_H
#define DEF_MAIN_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <bits/stdc++.h>
#include <vector>
using namespace std;

#include <jack/jack.h>

#include "./modules/Drive.h"
#include "./modules/LFO.h"
#include "./modules/Ringmod.h"
#include "./modules/Delay.h"
#include "./modules/Tonestack.h"
#include "./modules/Reverb.h"

#include "./io/io.h"

/*
*	Each Module_Node contains an IO_Accessor_List
*/
typedef std::vector<IO_Accessor*> IO_Accessor_List;

/*
*	Contain list of all connection for a Module_Node
*	source_port_index * 1,000,000 + target Module_node_index * 1,000 + target_port_index
*/
typedef std::vector<int> Connection_List;

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
		/*
		*	Update the io_accessor list with the given value table
		*/
		void			accessor_update(int *potar_tab);
		
		/*
		*	Manage connections
		*	Add : Add a connection, format = XXX,XXX,XXX --> Source_port_index,Target_index,Target_port_index
		*	Remove and get : i : index of  the connection
		*/
		void			connection_add(int i);
		void			connection_remove(int i);
		int 			connection_get(int i) const;
		
	private :
		
		Module *_mod;
		IO_Accessor_List _io_accessor_list;
		Connection_List _connection_list;
};

/*
*	Main map for store modules nodes
*/
typedef std::unordered_map<int, Module_Node*> Module_Node_List;

/*
*	Main function used to create new module
*/
int main_add_module(MODULE_TYPE mod, int v);

/*
*	Remove module at idx
*/
int main_del_module(int idx);

/*
*	Return the name of source's port number is, or System Capture port [is] if source is NULL
*/
const char* get_source_name(Module *source, int v, int is, Module *target);

/*
*	Return the name of target's port number is, or System Playback port [is] if target is NULL
*/
const char* get_target_name(Module *target, int v, int id, Module *source);

/*
*	Connect module[source_idx]'s port[is] to module[target_idx]'s port[id]
*	source_idx = -1 for System_Capture ports
*	target_idx = -1 for System_Playback ports
*/
int main_add_connection(int source_idx, int source_v, int is, int target_idx, int target_v, int id);
int main_del_connection(int source_idx, int source_v, int is, int target_idx, int target_v, int id);

/*
*	Create a link between a potentiometer and param[param_idx] of module[target]
*	See io/io.h --> IO_Accessor
*/
int main_add_accessor(int target, int target_voice, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);

#endif