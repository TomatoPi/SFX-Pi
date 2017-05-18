#include "Modules.h"

int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

/*
*	Basic module constructor
*	basic setup of jack client and server
*	Port registration
*/
Module::Module(const char *server, const char *name, int ai, int ao, int mi, int mo, ...){

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
	va_list arg;
	int port_count = ai + ao + mi + mo;
	this->port = (jack_port_t**)malloc(port_count * sizeof(jack_port_t*));
	if(this->port == NULL){
		fprintf(stderr, "Failed creating ports array\n");
		exit(1);
	}
	va_start(arg, mo);
	for(int i = 0; i < port_count; i++){
		int c = 0;
		if(i < (ai+ao+mi+mo))c = 3;
		if(i < (ai+ao+mi))c = 2;
		if(i < (ai+ao))c = 1;
		if(i < ai)c = 0;
		
		jack_port_t *port;
		const char *n = va_next(arg, const char *);
		
		switch(c){
			case 0:
				port = jack_port_register( this->client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
				break;
			case 1:
				port = jack_port_register( this->client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
				break;
			case 2:
				port = jack_port_register( this->client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
				break;
			case 3:
				port = jack_port_register( this->client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
				break;
		}
		
		if(port == NULL){
			fprintf(stderr	, "No more port aviable\n");
			exit(1);
		}
		this->port[i] = port;
	}
	va_end(arg);
	
	//register callback function
	jack_set_process_callback(client, mod_Process_Callback, this);
	
	this->client = client;
	this->name = (char*)name;
}
