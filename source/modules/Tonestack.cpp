#include "Tonestack.h"
Tonestack_voice::Tonestack_voice(jack_client_t *client, int idx): Module_voice(client, TONE_PARAMS, 1, 1, 0, 0){

	this->filter = new Filter_3EQ(200, 1000, jack_get_sample_rate(client));
	this->set_param_list(TONE_PARAMS, (float*)tone_default_params);
	
	char n[10];
	sprintf(n, "In_%d", idx);
	register_port(client, this->port_audio_in_, PORT_AI, 1, n);
	sprintf(n, "Out_%d", idx);
	register_port(client, this->port_audio_out_, PORT_AO, 1, n);
	
	this->is_ready_ = 1;
}

Tonestack_voice::~Tonestack_voice(){
	
	delete this->filter;
}

void Tonestack_voice::set_param(int param, float var){

	if(param == TONE_FLOW){
		
		this->filter->set_lf(var);
		this->param_[TONE_FLOW] = var;
	}else if(param == TONE_FHIGH){
		
		this->filter->set_hf(var);
		this->param_[TONE_FHIGH] = var;
	}else{
		
		return this->Module_voice::set_param(param, var);
	}
}

void Tonestack_voice::set_param_list(int size, float *pl){

	if(size == this->param_count_){
		
		this->Module_voice::set_param_list(size, pl);
		this->filter->set_freq(this->param_[TONE_FLOW], this->param_[TONE_FHIGH]);
	}
}

Filter_3EQ* Tonestack_voice::get_filter() const{
	
	return this->filter;
}

Tonestack::Tonestack(const char *server, int vc): Module(server, MTONE, vc){
	
	for(int i = 0; i < vc; i++){
		
		this->voice_.push_back(new Tonestack_voice(this->client_, i));
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

int Tonestack::process(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
	
		if(!!(*itr)->is_ready()){
		
			sample_t *in, *out;	
			in  = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);
			out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
			
			float gl, gm, gh;
			gl = (*itr)->get_param(TONE_GL);
			gm = (*itr)->get_param(TONE_GM);
			gh = (*itr)->get_param(TONE_GH);

			for(jack_nframes_t i = 0; i < nframes; i++){
				
				out[i] = ((Tonestack_voice*)*itr)->get_filter()->compute(in[i], gl, gm, gh);
			}
		}
	}
	return 0;	
}

int Tonestack::bypass(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			mod_generic_bypass_callback(nframes, (*itr)->get_port(PORT_AI, 0), (*itr)->get_port(PORT_AO, 0));
		}
	}
	return 0;
}

void Tonestack::add_voice(){
	
	int idx = this->voice_.size();
	this->voice_.push_back(new Tonestack_voice(this->client_, idx));
}

const char* Tonestack::get_param_name(int p) const{
	
	if(p < this->get_voice(0)->get_param_count())
		return tone_param_names[p];
	return "NULL";
}