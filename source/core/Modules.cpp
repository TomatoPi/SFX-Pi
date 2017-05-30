#include "Modules.h"

int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

int mod_Bypass_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->bypass(nframes, u);
}

int mod_GenericStereoBypass_Callback(jack_nframes_t nframes, jack_port_t **ports, int foi){
	
	for(int i = 0; i < 2; i++){
		jack_default_audio_sample_t *in = (jack_default_audio_sample_t*)jack_port_get_buffer(ports[i], nframes);
		jack_default_audio_sample_t *out = (jack_default_audio_sample_t*) jack_port_get_buffer(ports[foi+i], nframes);
		memcpy(out, in, sizeof(jack_default_audio_sample_t) * nframes);
	}
	return 0;
}

const char* mod_tton(MODULE_TYPE type){

	switch(type){
		
		case MDRIVE:
			return "Drive";
		case MDELAY:
			return "Delay";
		case MLFO:
			return "LFO";
		case MRINGM:
			return "RingMod";
		default:
			return "Module";
	}
}

/*
*	Basic module constructor
*	basic setup of jack client and server
*	Port registration
*/
Module::Module(const char *server, MODULE_TYPE type, int pc, int ai, int ao, int mi, int mo, ...): type(type), port_count(ai+ao+mi+mo), params_count(pc), is_bypassed(0), is_params_outdated(0){

	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	const char* name = mod_tton(type);
	
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
	
	this->client = client;
	this->name = (char*)name;
	
	//register module ports
	int port_count = ai + ao + mi + mo;
	if(port_count != 0){
		
		va_list arg;
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
			const char *n = va_arg(arg, char*);

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
	}
	
	//register callback function
	jack_set_process_callback(client, mod_Process_Callback, this);
	
	/*
	*	Setup params list
	*/
	this->params = (float*)malloc(this->params_count * sizeof(float));
	if(this->params == NULL){
		fprintf(stderr, "Failed Create Params list\n");
		exit(1);
	}
}

Module::~Module(){

	free(this->port);
	free(this->params);
	jack_client_close(this->client);
}

void Module::set_bypass(int state){
	
	this->is_bypassed = state;
	if(state){
		jack_set_process_callback(client, mod_Bypass_Callback, this);
	}else{
		jack_set_process_callback(client, mod_Process_Callback, this);
	}
}

int Module::get_bypass(){
	return this->is_bypassed;
}

int Module::set_param(int param, float var){
	
	if(param >= this->params_count){
		return 1;
	}
	this->params[param] = var;
	this->is_params_outdated = 1;
	return 0;
}

float Module::get_param(int param){
	
	if(param < this->params_count){
		return this->params[param];
	}
	return 0.0;
}

float* Module::get_param_adress(int param){
	
	if(param < this->params_count){
		return &(this->params[param]);
	}
	return NULL;
}

int Module::set_param_list(int size, float *params){
	
	if(size != this->params_count)
		return 1;
	free(this->params);
	this->params = params;
	this->is_params_outdated = 1;
	return 0;
}

jack_port_t* Module::get_port(int idx){
	
	if(idx < this->port_count){
		return this->port[idx];
	}
	return NULL;
}

int Module::get_port_count(){

	return this->port_count;
}

jack_client_t* Module::get_client(){

	return this->client;
}
