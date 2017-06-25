#include "Chorus.h"

Chorus_voice::Chorus_voice(jack_client_t *client, int idx): Module_voice(client, CHORUS_PARAMS, 2, 1, 0, 0){
	
	this->samplerate_ = jack_get_sample_rate(client);
	
	int del[CHORUS_SIZE];
	for(int i = 0; i < CHORUS_SIZE; i++) del[i] = 9*i;
	fprintf (stderr, "4\n");
	this->buffer_ = new Buffer_M( (CHORUS_SIZE+2)*9, this->samplerate_, CHORUS_SIZE, del);
	
	fprintf (stderr, "5\n");
	this->set_param_list(CHORUS_PARAMS, (float*)default_chorus_values);
	
	fprintf (stderr, "6\n");
	char n[20], n2[20];
	sprintf(n, "In_%d", idx);
	sprintf(n2, "Mod_%d", idx);
	register_port(client, this->port_audio_in_, PORT_AI, 2, n, n2);
	
	fprintf (stderr, "7\n");
	sprintf(n, "Out_%d", idx);
	register_port(client, this->port_audio_out_, PORT_AO, 1, n);
	
	fprintf (stderr, "8\n");
	this->is_ready_ = 1;
}

Chorus_voice::~Chorus_voice(){
	
	delete this->buffer_;
}

void Chorus_voice::set_param(int param, float var){
	
	this->Module_voice::set_param(param, var);
}

void Chorus_voice::set_param_list(int size, float *pl){
	
	this->Module_voice::set_param_list(size, pl);
}

Buffer_M* Chorus_voice::get_buffer() const{
	
	return this->buffer_;
}

Chorus::Chorus(const char* server, int vc):Module(server, MCHORUS, vc){
	
	for(int i = 0; i < vc; i++){
		
		this->voice_.push_back(new Chorus_voice(this->client_, i));
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

int Chorus::process(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			
			Chorus_voice *voice = (Chorus_voice*)*itr;
			
			sample_t *in, *mod, *out;
			in  = (sample_t*)jack_port_get_buffer(voice->get_port(PORT_AI, 0), nframes);
			mod = (sample_t*)jack_port_get_buffer(voice->get_port(PORT_AI, 1), nframes);
			out = (sample_t*)jack_port_get_buffer(voice->get_port(PORT_AO, 0), nframes);
			
			float dph = voice->get_param(CHORUS_DEPTH);
			float dw  = voice->get_param(CHORUS_DRYWET);
			
			for(int i = 0; i < nframes; i++){
				
				sample_t w = 0;
				for(int k = 0; k < CHORUS_SIZE; k++)
					w += voice->get_buffer()->read(k, 1 + (dph * mod[i]));
				
				voice->get_buffer()->write(in[i]);
				
				out[i] = spi_dry_wet(in[i], w / (float)CHORUS_SIZE, dw);
			}
		}
	}
}

int Chorus::bypass(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			mod_generic_bypass_callback(nframes, (*itr)->get_port(PORT_AI, 0), (*itr)->get_port(PORT_AO, 0));
		}
	}
	return 0;
}

void Chorus::add_voice(){
	
	int idx = this->voice_.size();
	this->voice_.push_back(new Chorus_voice(this->client_, idx));
}

const char* Chorus::get_param_name(int p) const{
	
	if(p < CHORUS_PARAMS)
		return chorus_params_name[p];
	return "NULL";
}