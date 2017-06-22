#include "Reverb.h"
Reverb_voice::Reverb_voice(jack_client_t *client, int idx):Module_voice(client, REVERB_PARAMS, 1, 2, 0, 0){
	
	
	int sr = jack_get_sample_rate(client);
	for(int i = 0; i < COMBCOUNT; i++){
		
		this->comb_[i] = new Filter_comb(this->damp_, this->room_, combsize[i] * sr / 44100, sr, 0);
	}
	for(int i = 0; i < ALLPCOUNT; i++){
		
		this->allp_[i] = new Filter_allpass(this->allpf_, allpsize[i]);
	}
	
	this->set_param_list(REVERB_PARAMS, (float*)reverb_default_params);
	
	char n[20];
	
	sprintf(n, "In_%d", idx);
	this->port_audio_in_[0]  = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	
	sprintf(n, "Out_%d", idx);
	this->port_audio_out_[0] = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	sprintf(n, "Rev_%d", idx);
	this->port_audio_out_[1] = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	this->is_ready_ = 1;
}

Reverb_voice::~Reverb_voice(){
	
	for(int i = 0; i < COMBCOUNT; i++){
	
		delete this->comb_[i];
	}

	for(int i = 0; i < ALLPCOUNT; i++){
		
		delete this->allp_[i];
	}
}

void Reverb_voice::set_param(int param, float var){
	
	if(param < REVERB_PARAMS){
		
		this->param_[param] = var;
		
		switch(param){
			case REV_ROOM :
				this->room_ = (var * scaleroom) + offsetroom;
				break;
			case REV_DAMP :
				this->damp_ = var * scaledamp;
				break;
			case REV_WET :
				this->wet_ = var * scalewet;
				break;
			case REV_DRY :
				this->dry_ = var * scaledry;
				break;
			case REV_ALL :
				this->allpf_ = var;
				break;
			default :
				break;
		}
		
		this->update();
	}
}

void Reverb_voice::set_param_list(int size, float *pl){
	
	if(size == this->param_count_){
		
		this->Module_voice::set_param_list(size, pl);
		
		this->room_ = (this->param_[REV_ROOM] * scaleroom) + offsetroom;
		this->damp_ = this->param_[REV_DAMP] * scaledamp;
		this->wet_  = this->param_[REV_WET] * scalewet;
		this->dry_  = this->param_[REV_DRY] * scaledry;
		this->allpf_ = this->param_[REV_ALL];
		
		this->update();
	}
}

Filter_comb* Reverb_voice::get_comb(int idx) const{
	
	if(idx < COMBCOUNT) return this->comb_[idx];
	return this->comb_[0];
}

Filter_allpass* Reverb_voice::get_allp(int idx) const{
	
	if(idx < ALLPCOUNT) return this->allp_[idx];
	return this->allp_[0];
}

void Reverb_voice::update(){
	
	for(int i = 0; i < COMBCOUNT; i++){
		
		this->comb_[i]->set_feedback(this->room_);
		this->comb_[i]->set_damp(this->damp_);
	}
	
	for(int i = 0; i < ALLPCOUNT; i++)
		this->allp_[i]->set_feedback(this->allpf_);
}
		
Reverb::Reverb(const char *server, int vc) : Module(server, MREV, vc){
	
	for(int i = 0; i < vc; i++){
		
		this->voice_.push_back(new Reverb_voice(this->client_, i));
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}
	
int Reverb::process(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
				
			sample_t *in;	
			in = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);
			
			sample_t *out;
			out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
				
			sample_t *rev;
			rev = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 1), nframes);
			
			float gain = this->gain_;
			
			float dry = (*itr)->get_param(REV_DRY);
			float wet = (*itr)->get_param(REV_WET);
			
			for(jack_nframes_t i = 0; i < nframes; i++){

				sample_t input = in[i]* gain;
				rev[i] = 0;
				
				for(int k = 0; k < COMBCOUNT; k++){
					
					rev[i] += ((Reverb_voice*)*itr)->get_comb(k)->compute(input);
				}
				
				for(int k = 0; k < ALLPCOUNT; k++){
					
					rev[i] = ((Reverb_voice*)*itr)->get_allp(k)->compute(rev[i]);
				}
				
				out[i] = rev[i]*wet + in[i]*dry;
			}
		}
	}
	return 0;
}

int Reverb::bypass(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			mod_generic_bypass_callback(nframes, (*itr)->get_port(PORT_AI, 0), (*itr)->get_port(PORT_AO, 0));
		}
	}
	return 0;
}

void Reverb::add_voice(){
	
	int idx = this->voice_.size();
	this->voice_.push_back(new Reverb_voice(this->client_, idx));
}

const char* Reverb::get_param_name(int p) const{
	
	if(p < this->get_voice(0)->get_param_count())
		return reverb_param_names[p];
	return "NULL";
}