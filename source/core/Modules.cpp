#include "Modules.h"

int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

/*
*	Basic module constructor
*	basic setup of jack client and server
*	Port registration
*/
Module::Module(const char *server, const char *name, int port_count, const char **port_names, const char **port_types, unsigned long **port_flags){

	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	//Creating jack client with name "name", in server "server"
	client = jack_client_open (name, options, &status, server);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	
	//upgrade name if given name is not unique
	if (status & JackNameNotUnique) {
		name = jack_get_client_name(client);
		fprintf (stderr, "Unique name `%s' assigned\n", name);
	}
	
	//register module ports
	this->a_port = (jack_port_t**)malloc(port_count * sizeof(jack_port_t*));
	if(this->a_port == NULL){
		fprintf(stderr, "Failed creating ports array\n");
		exit(1);
	}
	for(int i = 0; i < port_count; i++){
		jack_port_t *port = jack_port_register( this->client, port_names[i], port_types[i], port_flags[i], 0);
		if(port == NULL){
			fprintf(stderr	, "No more port aviable\n");
			exit(1);
		}
		this->a_port[i] = port;
	}
	
	//register callback function
	jack_set_process_callback(client, mod_Process_Callback, this);
	
	this->client = client;
	this->name = (char*)name;
}
