#ifndef DEF_MODULES_H
#define DEF_MODULES_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

/*
*   	Function for register class member callback function
*/
private int mod_Process_Callback(jack_nframes_t nframes, void *u);

/*
*	Basic class for all modules
*/
class Module{
	
	public:
		//see mod_register_ports for additional params
		Module(const char *server, const char *name, int port_count, const char **port_names, const char **port_types, unsigned long **port_flags);
		
		virtual int process(jack_nframes_t nframes, void *arg){}; // Client's callback functio
	
	protected:
	
		jack_client_t *client;	//JACK Client
		char* name;		//Client unique name
};

#endif
