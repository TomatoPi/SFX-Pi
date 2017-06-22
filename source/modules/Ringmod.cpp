#include "Ringmod.h"
Ringmod_voice::Ringmod_voice(jack_client_t *client,int idx): Module_voice(client, RINGMOD_PARAMS, 2, 1, 0, 0){
	
	this->set_param_list(RINGMOD_PARAMS, (float*)ringmod_default_params);
	
	char n[20];
	char n2[20];
	sprintf(n, "In_%d", idx);
	sprintf(n2, "Mod_%d", idx);
	register_port(client, this->port_audio_in_, PORT_AI, 2, n, n2);
	sprintf(n, "Out_%d", idx);
	register_port(client, this->port_audio_out_, PORT_AO, 1, n);
	
	this->is_ready_ = 1;
}

void Ringmod_voice::set_param(int param, float var){
	
	this->Module_voice::set_param(param, var);
}

void Ringmod_voice::set_param_list(int size, float *pl){
	
	this->Module_voice::set_param_list(size, pl);
}

Ringmod::Ringmod(const char *server, int vc): Module(server, MRINGM, vc){

	for(int i = 0; i < vc; i++){
		
		this->voice_.push_back(new Ringmod_voice(this->client_, i));
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

int Ringmod::process(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
				
			sample_t *in, *out, *mod;
			
			in  = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);
			
			mod = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 1), nframes);
			
			out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
			
			float depth = (*itr)->get_param(RING_DEPTH);
			
			for(jack_nframes_t i = 0; i < nframes; i++){
				
				out[i] = (in[i] * (1 - depth)) + (in[i] * mod[i] * depth);
			}
		}
	}
	return 0;
}

int Ringmod::bypass(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			mod_generic_bypass_callback(nframes, (*itr)->get_port(PORT_AI, 0), (*itr)->get_port(PORT_AO, 0));
		}
	}
	return 0;
}

void Ringmod::add_voice(){
	
	int idx = this->voice_.size();
	this->voice_.push_back(new Ringmod_voice(this->client_, idx));
}

const char* Ringmod::get_param_name(int p) const{
	
	if(p < this->get_voice(0)->get_param_count())
		return ringmod_param_names[p];
	return "NULL";
}