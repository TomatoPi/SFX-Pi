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

/**
*   Connection struct.
*   Store the signature of a JACK connection between two modules
*/
typedef struct{
	
	short s;    /**< Source's index */
	short sp;   /**< Source's port index */
	short t;    /**< Target's index */
	short tp;   /**< Target's port index */
}Connection;

/**
*	Alias for vector of IO_Accessor pointer
*/
typedef std::vector<IO_Accessor*> IO_Accessor_List;

/**
*	Alias for vector of Connection
*/
typedef std::vector<Connection> Connection_List;

/**
*   Module node class.
*	Contain a module it's accessors and it's connections
*/
class Module_Node{
	
	public :
		
		/**
		*	Module node constructor
        *   @param mod pointer to the node's module
		*/
		Module_Node(Module* mod);
		~Module_Node();
		
		
		
		/**
		*	Get node's module
        *   @return pointer to node's module
		*/
		Module* 		get_module() const;
		
		/**
		*	Add, remove or get the accessor at index i
		*/
		void 			accessor_add(IO_Accessor *accessor);
		void 			accessor_remove(int i);
		IO_Accessor* 	accessor_get(int i) const;
		IO_Accessor_List* accessor_get_list();
		/**
		*	Update the io_accessor list with the given value table
		*/
		void			accessor_update(int *potar_tab);
		
        /**
        *   Add, remove or get connection at i
        */
		void			connection_add(Connection c);
		void			connection_remove(int i);
		Connection 		connection_get(int i) const;
		Connection_List*	connection_get_list();
		
    protected :
    
		Module *_mod;
		IO_Accessor_List _io_accessor_list;
		Connection_List _connection_list;
};

/**
*	Alias for vector of Module nodes
*/
typedef std::vector<Module_Node*> Module_List;
/**
*   Main graph class.
*/
class Module_Node_List{
	
	public :
    
        Module_Node_List();
    
        /**
        *   Main function used to create new module.
        *   Instantiate a new module of given type and add it to the graph
        *   
        *   @param mod type of module to add_accessor
        *   @return 0 on success
        */
        int add_module(MODULE_TYPE mod);

        /**
        *   Remove module at idx.
        *   
        *   @param idx index of module to remove
        *   @return 0 on success
        */
        int del_module(int idx);

        /**
        *   Function for connect two module nodes.
        *	
        *	source_idx = -1 for System_Capture ports
        *	target_idx = -1 for System_Playback ports
        */
        int add_connection(short source_idx, short is, short target_idx, short id);
        int del_connection(short source_idx, short is, short target_idx, short id);

        /**
        *	Create a link between a potentiometer and param[param_idx] of module[target]
        *	See io/io.h --> IO_Accessor
        */
        int add_accessor(int target, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);

        Module_List list_;
        int count_;
};

/*
*	Return the name of source's port number is, or System Capture port [is] if source is NULL
*/
const char* get_source_name(Module *source, int is, Module *target);

/*
*	Return the name of target's port number is, or System Playback port [is] if target is NULL
*/
const char* get_target_name(Module *target, int id, Module *source);

/*
*	---------------------------------------------------------------------------
*	Presset Managing stuff
*	---------------------------------------------------------------------------
*/
static const string NODE_ROOT("Preset");
static const string NODE_MODULE("Module");
static const string NODE_PARAM("Params");
static const string NODE_ACCESSOR("Accessor");
static const string NODE_POTENTIOMETER("Potentiometer");
static const string NODE_CONNECTION("Connection");


int save_presset(string name, string version, Module_Node_List list);

int load_presset(string name, string version, Module_Node_List *list);

#endif