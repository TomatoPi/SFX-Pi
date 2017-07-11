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
#include <string>
#include <stdarg.h>

#include <vector>

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
        
	protected:
    
        virtual void change_param(int idx, float value) {}; /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values) {};        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx) {}; /**< @see get_param_name(int idx) */
    
		jack_client_t 	*client_;	/**< JACK's Client */
		char* 			name_;		/**< Client unique name */
		MODULE_TYPE 	type_;		/**< Module's type */
		
		bool 			is_bypassed_;	/**< True if Module is bypassed */
        
        jack_port_t **audio_in_, **audio_out_;  /**< JACK's audio ports */
        int ai_, ao_;                           /**< number of audio ports*/
        jack_port_t **midi_in_, **midi_out_;    /**< JACK's midi ports */
        int mi_, mo_;                           /**< number of midi ports*/
        
        float *param_;  /**< Array of params */
        int param_c_;   /**< Number of params */
};

#endif