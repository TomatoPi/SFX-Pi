#include "Modules.h"

int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

/*
*	Basic module constructor
*	basic setup of jack client and server
*/
Module::Module(const char *server, const char *name){

	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	//Creation du client JACK, "name" dans le serveur "server"
	client = jack_client_open (name, options, &status, server);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	//si un client porte deja ce nom, mise a jour du nom
	if (status & JackNameNotUnique) {
		name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", name);
	}
	
	//enregistrement de la fonction callback
	jack_set_process_callback(client, mod_Process_Callback, this);
	
	this->client = client;
	this->name = (char*)name;
}
