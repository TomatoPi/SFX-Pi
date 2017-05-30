#ifndef DEF_MODULES_H
#define DEF_MODULES_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <jack/jack.h>

/*
*   	Function for register class member callback function
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u);
int mod_Bypass_Callback(jack_nframes_t nframes, void *u);

/*
*	Basic callback that copy stereo input to output
*	*ports order : in_l, in_r, out_l, out_r
*	foi is the first output's port
*/
int mod_GenericStereoBypass_Callback(jack_nframes_t nframes, jack_port_t **ports, int foi);

/*
*	Enum constainig list of all avaiable modules
*/
typedef enum{
	MDRIVE, MDELAY, MLFO, MRINGM
}MODULE_TYPE;

const char* mod_tton(MODULE_TYPE type);

/*
*	Basic class for all modules
*/
class Module{
	
	public:
		/*
		*	Constructor for all modules, setup jack client and io ports
		* 	server : server name
		*	name : module unique name
		*	pc : number of module's parameter
		* 	ai : number of audio input
		*	ao : number of midi input
		*	mi : number of audio output
		*	mo : number of midi output
		*	... : list of port names in order ai -> ao -> mi -> mo
		*/
		Module(const char *server, MODULE_TYPE type,int pc, int ai, int ao, int mi, int mo, ...);
		~Module();
		
		virtual int process(jack_nframes_t nframes, void *arg){}; // Client's callback function
		virtual int bypass(jack_nframes_t nframes, void *arg){}; // Client's callback function when client is bypassed
	
		void set_bypass(int state);
		int get_bypass();
	
		int set_param(int param, float var);
		float get_param(int param);
	
		float* get_param_adress(int param);
		int set_param_list(int size, float *params);
	
		jack_port_t* get_port(int idx);
		int get_port_count();
		jack_client_t* get_client();
	
	protected:
	
		jack_client_t *client;	//JACK Client
		char* name;		//Client unique name
		MODULE_TYPE type;
	
		jack_port_t **port;
		int port_count;
		
		int is_bypassed;
		int params_count;
		float *params;
	
		int is_params_outdated;
};

#endif
