#include "Tonestack.h"

Tonestack::Tonestack(const char *server): Module(server, MTONE, 5, 2, 2, 0, 0, "in_L", "in_R", "out_L", "out_R"){

	this->params[0] = 200;
	this->params[1] = 1000;
	this->params[2] = 0.75;
	this->params[3] = 3.0;
	this->params[4] = 7.0;
	
	this->filter_L = new Filter_3EQ(this->params[0], this->params[1], (int)jack_get_sample_rate(this->client));
	this->filter_R = new Filter_3EQ(this->params[0], this->params[1], (int)jack_get_sample_rate(this->client));
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

Tonestack::~Tonestack(){
	
	delete this->filter_L;
	delete this->filter_R;
}

int Tonestack::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *out_L;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	out_L = (sample_t*)jack_port_get_buffer(this->port[2], nframes);
	
	sample_t *in_R, *out_R;	
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	float gl, gm, gh;
	gl = this->params[2];
	gm = this->params[3];
	gh = this->params[4];

	for(jack_nframes_t i = 0; i < nframes; i++){
		
		out_L[i] = this->filter_L->compute(in_L[i], gl, gm, gh);
		out_R[i] = this->filter_R->compute(in_R[i], gl, gm, gh);
	}
	return 0;	
}

int Tonestack::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}

int Tonestack::set_param(int param, float var){
	
	if(param < 2){
		
		if(param == 0){
			this->filter_L->set_lf(var);
			this->filter_R->set_lf(var);
			this->params[0] = var;
		}
		if(param == 1){
			this->filter_L->set_hf(var);
			this->filter_R->set_hf(var);
			this->params[1] = var;
		}
		return 0;
	}else{
		
		return this->Module::set_param(param, var);
	}
}


int Tonestack::set_param_list(int size, float *params){
	
	if(!this->Module::set_param_list(size, params)){
		this->filter_L->set_freq(this->params[0], this->params[1]);
		this->filter_R->set_freq(this->params[0], this->params[1]);
		return 0;
	}
	return 1;
}