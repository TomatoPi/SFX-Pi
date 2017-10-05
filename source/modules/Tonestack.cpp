#include "Tonestack.h"
Tonestack::Tonestack(): Module(MOD_TONE, TONE_PARAMS_COUNT,
    1, 1, 0, 0, "In", "Out"),
    filter_(Filter_3EQ(200, 1000, jack_get_sample_rate(client_)))
{
    
    this->set_param(MOD_COUNT + TONE_PARAMS_COUNT, TONE_DEFAULT_PARAM);
	
	if (jack_activate (this->client_)) {
		
        throw string("Failed activate client");
	}
}

inline int Tonestack::do_process(jack_nframes_t nframes){

    sample_t *in, *out;	
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    if ( !is_bypassed_ ){
        
        float vol = param_[MOD_VOLUME];
        
        float gl, gm, gh;
        gl = param_[TONE_GL];
        gm = param_[TONE_GM];
        gh = param_[TONE_GH];

        for(jack_nframes_t i = 0; i < nframes; i++){
            
            out[i] = vol * filter_.compute(in[i], gl, gm, gh);
        }
    }else{
        
        memcpy( out, in, sizeof(sample_t) * nframes );
    }
	
	return 0;	
}

/*
int Tonestack::bypass(jack_nframes_t nframes, void *arg){
	
	sample_t *in, *out;	
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    memcpy(out, in, sizeof(sample_t) * nframes);
    
	return 0;
}
*/

void Tonestack::change_param(int idx, float value){
    
    //param_[idx] = value;
    
    if(idx == TONE_FLOW){
		
		filter_.set_lf(value);
	}else if(idx == TONE_FHIGH){
		
		filter_.set_hf(value);
	}
}

void Tonestack::change_param(const float *values){
    
    //memcpy(param_, values, param_c_);
    filter_.set_freq(param_[TONE_FLOW], param_[TONE_FHIGH]);
}

void Tonestack::new_bank(){
    
    this->add_bank( MOD_COUNT + TONE_PARAMS_COUNT, TONE_DEFAULT_PARAM);
}

string Tonestack::return_param_name(int idx){
    
    return TONE_PARAM_NAMES[idx];
}

string Tonestack::return_formated_param(int idx){
    
    string n = TONE_PARAM_NAMES[idx];
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case TONE_FLOW :
        
            n += "  ";
            n += f_ftos( param_[idx] );
            break;
        
        case TONE_FHIGH :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case TONE_GL :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case TONE_GM :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case TONE_GH :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        default :
            break;
    }
    
    return n;
} 
