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
		Module(const char *server, const char *name,int pc, int ai, int ao, int mi, int mo, ...);
		
		virtual int process(jack_nframes_t nframes, void *arg){}; // Client's callback function
	
		int set_param(int param, float var);
		float get_param(int param);
	
	protected:
	
		jack_client_t *client;	//JACK Client
		char* name;		//Client unique name
	
		jack_port_t **port;
	
		int params_count;
		float *params;
};

#endif
