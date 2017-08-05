#ifndef DEF_MODULES_H
#define DEF_MODULES_H

/*
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
*/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <vector>

#include <math.h>
#include <jack/jack.h>

using namespace std;

/**
*   Enum of avaiables ports types
*/
enum MODULE_PORT_TYPE{
    
    AUDIO_I,
    AUDIO_O,
    MIDI_I,
    MIDI_O
};

/**
*   Function for register class member callback functions
*   @see Module::process_callback(jack_nframes_t, void*)
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u);
int mod_Bypass_Callback(jack_nframes_t nframes, void *u);

/**
*	Enum of all avaiable modules
*/
typedef enum{
	
	MOD_DRIVE	=0, 
	MOD_DELAY   =1, 
	MOD_LFO	    =2, 
	MOD_RINGM	=3, 
	MOD_TONE	=4, 
	MOD_REV	    =5,
	MOD_CHORUS	=6
}MODULE_TYPE;

/**
*	Get formated name of given module type
*/
string modtype_to_string(MODULE_TYPE type);

/**
*   Alias for vector of param array used by modules.
*   Banks are used to save different sets of param's values for a module
*/
typedef std::vector<float*> ModBank;

/*
*	Basic class for all modules
*/
class Module{
	
	public:
    
        /**
        *   Module basic constructor.
        *   It create jack client for module, alloc and register it's ports
        *   It also alloc params array and make module waiting for activation
        *   Module specific subconstructor have to set it ready for compute
        *
        *   @param server JACK's server name
        *   @param type module's type
        *   @param pc number of parameters
        *   @param ai number of audio inputs
        *   @param ao number of audio outputs
        *   @param mi number of midi inputs
        *   @param mo unmber of midi outputs
        *   @param ... names of ports in order
        */
		Module(const char *server, MODULE_TYPE type, int pc, int ai, int ao, int mi, int mo, ...);
		~Module();
		
        /**
        *   Process callback called by jack when module is active
        *
        *   @param nframes number of frames to compute
        *   @param arg pointer to this module, unused
        *   @return 0
        */
		virtual int process(jack_nframes_t nframes, void *arg){};
        /**
        *   Bypass callback called when module is bypassed.
        *   Basically copy input to output_iterator or send 0
        *
        *   @see process(jack_nframes_t nframes, void *arg)
        */
		virtual int bypass(jack_nframes_t nframes, void *arg){};
	
        /**
        *   Set module status
        *   @param state true for bypass module
        */
		void 	set_bypass(bool state);
        /**
        *   Get module status
        *   @return true if module is bypassed
        */
		bool 	get_bypass() const;
		
        /**
        *   Get module JACK's client
        *   @return module's client
        */
		jack_client_t* 	get_client() const;
		
        /**
        *   Get module's type
        *   @return module's type
        */
		MODULE_TYPE get_type();
        
        /**
        *   Get module's name
        *   @return module's name
        */
        char* get_name();
        
        /**
        *   Change a single module's param.
        *   Check if given idx corrspond to a param, do nothing if not
        *   @param idx idx of param to change_param
        *   @param value new value for param
        */
        void set_param(int idx, float value);
        /**
        *   Change all params in a single row.
        *   Check if given param count is equal to module's param count
        *   do nothing if not
        *   @param count number of params in given array
        *   @param values params array
        */
        void set_param(int count,const float *values);
        
        int get_param_count() const;
        
        float get_param(int idx) const;
        
        /**
        *   Get formated name of given param.
        *   Used in user interface for navigate between params
        *   return empty string if idx is not valid
        *   @param idx param's index
        *   @return param's name or empty string if index is not valid
        */
        string get_param_name(int idx);
        
        /**
        *   Get paramne name followed by it value.
        *   Returned string is maximum 11 caracters long for screen
        *   @param idx param's index
        *   @return formated string or NONE if given param not found
        */
        string get_formated_param(int idx);
        
        /**
        *   Get module's port.
        *   @param type port's type
        *   @param idx port's index
        *   @return port of given type at given index or NULL if not found
        *   @see MODULE_PORT_TYPE
        */
        jack_port_t* get_port(MODULE_PORT_TYPE type, int idx);
        
        /**
        *   Get number of given port type
        *   @param type port's type
        *   @return number of prots of given type
        *   @see MODULE_PORT_TYPE
        */
        int get_port_count(MODULE_PORT_TYPE type);
        
        /**
        *   Add a bank to the module.
        *   @see ModBank
        *   @param size number of params in given bank
        *   @param bank bank to add
        */
        void add_bank(int size, const float *bank);
        
        /**
        *   Add a bank with default params values to the module.
        */
        void add_bank();
        
        /**
        *   Remove current bank from module.
        */
        void remove_bank();
        /**
        *   Remove given bank from module.
        *   @param idx index of bank to remove
        */
        void remove_bank(int idx);
        /**
        *   Change current bank and swap param values.
        *   Increment current bank or return to first one if current is the last
        *   @return true if bank have rewind to first bank
        */
        bool next_bank();
        /**
        *   Change current bank and swap param values.
        *   Decrement current bank or go to the last one if current is the first
        *   @return true if bank have rewind to last bank
        */
        bool prev_bank();
        /**
        *   Go to the given bank.
        *   If given index doesn't correspond to a bank, go to bank 0
        *   or default values if there is no bank
        *   @param idx target bank index
        */
        bool set_bank(int idx);
        /**
        *   Get curent bank index
        *   @return current bank index
        */
        int get_bank();
        
	protected:
    
        virtual void change_param(int idx, float value) {}; /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values) {};  /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx) {};       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx) {};   /**< @see get_formated_param(int idx) */
    
        virtual void new_bank() {};    /**< @see add_bank() */
    
		jack_client_t 	*client_;	/**< JACK's Client */
		char* 			name_;		/**< Client unique name */
		MODULE_TYPE 	type_;		/**< Module's type */
		
		bool 			is_bypassed_;	/**< True if Module is bypassed */
        
        jack_port_t **audio_in_, **audio_out_;  /**< JACK's audio ports */
        int ai_, ao_;                           /**< number of audio ports*/
        jack_port_t **midi_in_, **midi_out_;    /**< JACK's midi ports */
        int mi_, mo_;                           /**< number of midi ports*/
        
        float *param_;  /**< Array of current params */
        int param_c_;   /**< Number of params */
        
        ModBank bank_;  /**< Vector of avaiable params sets */
        int bank_idx_;  /**< Curent bank index */
};

#define SPI_HYSTERESIS 0.01f
#define SPI_POTAR_COUNT 6
#define SPI_MAX 1023.0f

typedef enum{
	
	CURVE_LIN=0,
	CURVE_SIG=1,
	CURVE_HAN=2,
	CURVEIHAN=3
}IO_CURVE;

float curve_lin(float v);	//	Identity tranfer function
float curve_sig(float v);	//	Tanh transfert function scaled between 0 and 1
float curve_han(float v);	//	Hanning window function
float curveihan(float v);	//	1 - Hanning window

class IO_Accessor{
 
	public :
	
		IO_Accessor(Module *module, int target_idx, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);
		
		int update(int *potar_tab);
		
		int is_dead() const;
		
		int 	get_target_idx() const;
		int 	get_target_param() const;
		int 	get_potar() const;
		float 	get_min() const;
		float 	get_max() const;
		int 	get_curve() const;
		int 	get_db() const;
		int 	get_inv() const;
	
	private :
		Module *module_;
	
		float (*curve)(float value);
		IO_CURVE curve_type;
		int target_idx;
		int target_param;
		
		int potentiometer;
		int is_db, is_inv;
		float min, max;
	
		int value;
		int state;
};

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

#endif