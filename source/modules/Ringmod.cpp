#include "Ringmod.h"
Ringmod::Ringmod(const char *server): Module(server, MOD_RINGM, RINGMOD_PARAMS_COUNT,
    2, 1, 0, 0, "In", "Mod", "Out")
{

    this->set_param(MOD_COUNT + RINGMOD_PARAMS_COUNT, RINGMOD_DEFAULT_PARAMS);
    
    if (jack_activate (this->client_)) {
        
        throw string("Failed activate client");
    }
}

inline int Ringmod::do_process(jack_nframes_t nframes){

 
    sample_t *in, *out, *mod;
    
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    
    mod = (sample_t*)jack_port_get_buffer(audio_in_[1], nframes);
    
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    if ( !is_bypassed_ ){
        
        float vol = param_[MOD_VOLUME];
        
        float depth = param_[RING_DEPTH];
        
        for(jack_nframes_t i = 0; i < nframes; i++){
            
            out[i] = vol * ( (in[i] * (1 - depth)) + (in[i] * mod[i] * depth) );
        }
    }else{
        
        memcpy( out, in, sizeof(sample_t) * nframes );
    }
  
    return 0;
}

/*
int Ringmod::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in, *out;
    
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    memcpy(out, in, sizeof(sample_t) * nframes);
    
    return 0;
}
*/

void Ringmod::change_param(int idx, float value){
    
    //param_[idx] = value;
}

void Ringmod::change_param(const float *values){
    
    //memcpy(param_, values, sizeof(float) * param_c_);
}

void Ringmod::new_bank(){
    
    this->add_bank( MOD_COUNT + RINGMOD_PARAMS_COUNT, RINGMOD_DEFAULT_PARAMS);
}

string Ringmod::return_param_name(int idx){
    
    return RINGMOD_PARAM_NAMES[idx];
}

string Ringmod::return_formated_param(int idx){
    
    string n = RINGMOD_PARAM_NAMES[idx];
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
            
        case RING_DEPTH :
        
            n += "   ";
            n += f_ftos( param_[idx] );
            break;
        
        default :
            break;
    }
    
    return n;
}