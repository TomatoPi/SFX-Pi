#include "Tonestack.h"
Tonestack::Tonestack(const char *server): Module(server, MOD_TONE, TONE_PARAMS_COUNT,
    1, 1, 0, 0, "In", "Out"),
    filter_(Filter_3EQ(200, 1000, jack_get_sample_rate(client_)))
{
    
    this->change_param(TONE_DEFAULT_PARAM);
	
	if (jack_activate (this->client_)) {
		
        throw string("Failed activate client");
	}
}

int Tonestack::process(jack_nframes_t nframes, void *arg){

    sample_t *in, *out;	
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    float gl, gm, gh;
    gl = param_[TONE_GL];
    gm = param_[TONE_GM];
    gh = param_[TONE_GH];

    for(jack_nframes_t i = 0; i < nframes; i++){
        
        out[i] = filter_.compute(in[i], gl, gm, gh);
    }
	
	return 0;	
}

int Tonestack::bypass(jack_nframes_t nframes, void *arg){
	
	sample_t *in, *out;	
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    memcpy(out, in, sizeof(sample_t) * nframes);
    
	return 0;
}

void Tonestack::change_param(int idx, float value){
    
    param_[idx] = value;
    
    if(idx == TONE_FLOW){
		
		filter_.set_lf(value);
	}else if(idx == TONE_FHIGH){
		
		filter_.set_hf(value);
	}
}

void Tonestack::change_param(const float *values){
    
    memcpy(param_, values, param_c_);
    filter_.set_freq(param_[TONE_FLOW], param_[TONE_FHIGH]);
}

string Tonestack::return_param_name(int idx){
    
    return TONE_PARAM_NAMES[idx];
}