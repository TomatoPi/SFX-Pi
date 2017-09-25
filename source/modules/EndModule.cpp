#include "EndModule.h"

EndModule::EndModule( const char *server, MODULE_TYPE t): Module( server, t, END_PARAM_COUNT,
    2, 2, 0, 0, "In_L", "In_R", "Out_L", "Out_R"),
    is_sat_(false),
    max(0)
{
    
    this->set_param(MOD_COUNT + END_PARAM_COUNT, END_DEFAULT_PARAMS);
    
    if (jack_activate(this->client_)) {
        
		throw string("Failed activate client");
	}
}

inline int EndModule::do_process( jack_nframes_t nframes ){
    
    sample_t *inl, *outl;
    inl  = (sample_t*)jack_port_get_buffer( audio_in_[0] , nframes);
    outl = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);
    
    sample_t *inr, *outr;
    inr  = (sample_t*)jack_port_get_buffer( audio_in_[1] , nframes);
    outr = (sample_t*)jack_port_get_buffer( audio_out_[1], nframes);
    
    if ( !is_bypassed_ ){
        
        float volr = param_[END_RIGHT];
        float voll = param_[END_LEFT];
        
        for (jack_nframes_t i = 0; i < nframes; i++) {
            
            outl[i] = voll * inl[i];
            outr[i] = volr * inr[i];
            
            is_sat_ = is_sat_ || outl[i] > 1 || outl[i] < -1 || outr[i] > 1 || outr[i] < -1;
            max = ( max < outl[i] )?outl[i] : ( max < outr[i] )? outr[i] : max;
        }
    }else{
        
        memcpy( outl, inl, sizeof(sample_t) * nframes );
        memcpy( outr, inr, sizeof(sample_t) * nframes );
    }
    
    return 0;
}

/*
int EndModule::bypass ( jack_nframes_t nframes, void *arg ){
    
    sample_t *outl = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);
    sample_t *outr = (sample_t*)jack_port_get_buffer( audio_out_[1], nframes);
    
    memset( outl, 0, sizeof(sample_t) * nframes );
    memset( outr, 0, sizeof(sample_t) * nframes );
    
    return 0;
}
*/

bool EndModule::is_saturated(){
    
    if ( is_sat_ ) cout << "Max recorded : " << max << endl;
    
    return is_sat_;
}

void EndModule::reset_saturated(){
    
    max = 0;
    
    is_sat_ = false;
}

void EndModule::change_param(int idx, float value){}

void EndModule::change_param(const float *values){}

string EndModule::return_param_name(int idx){

    return END_PARAM_NAMES[idx];
}

string EndModule::return_formated_param(int idx){
    
    string n = END_PARAM_NAMES[idx];
    
    switch (idx){
        
        case END_RIGHT :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
            
        case END_LEFT :
            
            n += "  ";
            n += f_ftos( param_[idx] );
            break;
            
        default :
            break;
    }
    
    return n;
}

void EndModule::new_bank(){
    
    this->add_bank( MOD_COUNT + END_PARAM_COUNT, END_DEFAULT_PARAMS );
}