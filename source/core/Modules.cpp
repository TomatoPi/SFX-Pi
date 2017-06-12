#include "Modules.h"
/*
*	---------------------------------------------------------------------------
*	Misc stuff
*	---------------------------------------------------------------------------
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

int mod_Bypass_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->bypass(nframes, u);
}

int mod_generic_bypass_callback(jack_nframes_t nframes, jack_port_t *port_in, jack_port_t *port_out){
	
	jack_default_audio_sample_t *in  = (jack_default_audio_sample_t*) jack_port_get_buffer(port_in, nframes);
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t*) jack_port_get_buffer(port_out, nframes);
	memcpy(out, in, sizeof(jack_default_audio_sample_t) * nframes);
	return 0;
}

void register_port(jack_client_t *client, jack_port_t **port_tab, int type, int count, ...){
	
	if(count > 0 && type < PORT_TYPE_COUNT && type >= 0){
		
		va_list arg;
		
		if(port_tab == NULL){
			fprintf(stderr, "Failed creating ports array\n");
			exit(1);
		}
		va_start(arg, count);
		for(int i = 0; i < count; i++){

			jack_port_t *port;
			const char *n = va_arg(arg, char*);

			switch(type){
				case PORT_AI:
					port = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
					break;
				case PORT_AO:
					port = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
					break;
				case PORT_MI:
					port = jack_port_register( client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
					break;
				case PORT_MO:
					port = jack_port_register( client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
					break;
			}

			if(port == NULL){
				fprintf(stderr	, "No more port aviable\n");
				exit(1);
			}
			
			port_tab[i] = port;
		}
		va_end(arg);
	}
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
		case MTONE:
			return "Tonestack";
		case MREV:
			return "Reverb";
		default:
			return "Module";
	}
}

/*
*	---------------------------------------------------------------------------
*	Module Voice stuff
*	---------------------------------------------------------------------------
*/
Module_voice::Module_voice(jack_client_t *client, int pc, int ai, int ao, int mi, int mo):
	param_count_(pc)
{
	this->port_count_[PORT_AI] = ai;
	this->port_count_[PORT_AO] = ao;
	this->port_count_[PORT_MI] = mi;
	this->port_count_[PORT_MO] = mo;
	
	this->port_audio_in_ 	= (jack_port_t**) calloc(this->port_count_[PORT_AI], sizeof(jack_port_t*));
	this->port_audio_out_ 	= (jack_port_t**) calloc(this->port_count_[PORT_AO], sizeof(jack_port_t*));
	this->port_midi_in_ 	= (jack_port_t**) calloc(this->port_count_[PORT_MI], sizeof(jack_port_t*));
	this->port_midi_out_ 	= (jack_port_t**) calloc(this->port_count_[PORT_MO], sizeof(jack_port_t*));
	
	this->param_ = (float*) calloc(pc, sizeof(float));
	if(this->param_ == NULL){
		fprintf(stderr, "Cannot allocate params array \n");
		exit(1);
	}
}

Module_voice::~Module_voice(){
	
	free(this->port_audio_in_);
	free(this->port_audio_out_);
	free(this->port_midi_in_);
	free(this->port_midi_out_);
	free(this->param_);
}

void Module_voice::set_param(int param, float var){
	
	if(param < this->param_count_ && param > -1){
		
		this->param_[param] = var;
	}
}

float Module_voice::get_param(int param) const{

	if(param < this->param_count_ && param > -1) return this->param_[param];
	return PARAM_NOT_FOUND;
}

int Module_voice::get_param_count() const{
	
	return this->param_count_;
}

void Module_voice::set_param_list(int size, float *pl){
	
	if(size == this->param_count_){
		free(this->param_);
		this->param_ = pl;
	}
}

jack_port_t* Module_voice::get_port(int type, int port) const{
	
	switch(type){
		case PORT_AI:
			return this->port_audio_in_[port];
		case PORT_AO:
			return this->port_audio_out_[port];
		case PORT_MI:
			return this->port_midi_in_[port];
		case PORT_MO:
			return this->port_midi_out_[port];
	}
}

jack_port_t** get_port_array(int type){
	
	switch(type){
		case PORT_AI:
			return this->port_audio_in_;
		case PORT_AO:
			return this->port_audio_out_;
		case PORT_MI:
			return this->port_midi_in_;
		case PORT_MO:
			return this->port_midi_out_;
	}
}

int	Module_voice::get_port_count(int type) const{
	
	if(type < PORT_TYPE_COUNT && type > -1) return this->port_count_[type];
	return NULL;
}

/*
*	---------------------------------------------------------------------------
*	Module class stuff
*	---------------------------------------------------------------------------
*/
Module::Module(const char *server, MODULE_TYPE type, int pc, int vc): type_(type), param_count_(pc), voice_count_(vc), is_bypassed_(0){

	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	const char* name = mod_tton(type);
	
	// Creating jack client with name "name", in server "server"
	client = jack_client_open (name, options, &status, server);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	
	// Upgrade name if given name is not unique
	if (status & JackNameNotUnique) {
		name = jack_get_client_name(client);
		fprintf (stderr, "Unique name `%s' assigned\n", name);
	}
	
	this->client_ = client;
	this->name_ = (char*)name;
	
	// Register callback function
	jack_set_process_callback(client, mod_Process_Callback, this);
}

Module::~Module(){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++) this->voice_.erase(itr);
	this->voice_.clear();
	jack_client_close(this->client_);
}

void Module::set_bypass(int state){
	
	this->is_bypassed_ = state;
	if(state){
		jack_set_process_callback(client, mod_Bypass_Callback, this);
	}else{
		jack_set_process_callback(client, mod_Process_Callback, this);
	}
}

int Module::get_bypass() const{
	
	return this->is_bypassed_;
}

/*
int Module::set_param(int voice, int param, float var){
	
	if(param >= this->params_count_ || voice >= this->voice_count_){
		return 1;
	}
	this->params_[voice][param] = var;
	return 0;
}

float Module::get_param(int voice, int param) const{
	
	if(param < this->params_count_ && voice < this->voice_count_){
		return this->params_[param];
	}
	return 0.0f;
}

int Module::get_param_count() const{
	
	return this->params_count_;
}
*/
Module_voice* Module::get_voice(int idx) const{
	
	if(idx < this->voice_.size()) return this->voice_[idx];
	return NULL;
}

int Module::get_voice_count() const{
	
	return this->voice_count_;
}

void Module::del_voice(int idx){
	
	if(idx < this->voice_.size()) this->voice_.erase(this->voice_.begin() +idx);
}
/*
int Module::set_param_list(int size, float *params){
	
	if(size != this->params_count_)
		return 1;
	free(this->params_);
	this->params_ = params;
	return 0;
}

jack_port_t* Module::get_port(int type, int idx) const{
	
	if(type < PORT_TYPE_COUNT && type >= 0)
		if(idx < this->port_count_[type] && idx >= 0){
			return this->port_[type][idx];
		}
	return NULL;
}

int Module::get_port_count(int type) const{

	if(type < PORT_TYPE_COUNT && type >= 0)
		return this->port_count_[type];
	return 0;
}

void Module::add_port(int type, char* name){
	
}

void Module::remove_port(int type, int idx){
	
}
*/
jack_client_t* Module::get_client() const{

	return this->client;
}
