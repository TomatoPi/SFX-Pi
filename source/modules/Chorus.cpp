#include "Chorus.h"
Chorus::Chorus(const char* server):Module(server, MOD_CHORUS, CHORUS_PARAMS_COUNT,
    1+MAX_CHORUS_SIZE, 1, 0, 0, "In", "Mod-1", "Mod-2", "Mod-3", "Mod-4", "Mod-5", "Out"),
    samplerate_(jack_get_sample_rate(client_))
{

    buffer_ = new Buffer_M( CHORUS_BUFFER, this->samplerate_, MAX_CHORUS_SIZE, (int*)CHORUS_DELAYS_LENGTH );
	this->set_param(MOD_COUNT + CHORUS_PARAMS_COUNT, CHORUS_DEFAULT_PARAMS);
    
    if (jack_activate(this->client_)) {
        
		throw string("Failed activate client");
	}
    
    //cout << "depth : " << param_[CHORUS_DEPTH] << endl;
}

Chorus::~Chorus(){

    jack_deactivate( client_ );
    
    delete buffer_;
}

inline int Chorus::do_process(jack_nframes_t nframes){

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
        
    if ( !is_bypassed_ ){
        
        int size = param_[CHORUS_SIZE];
        float *depths = param_ + CHORUS_WEIGTH;
        
        sample_t *mod[size];
        for( int j = 0; j < size; j++ ){

            mod[j] = (sample_t*)jack_port_get_buffer(audio_in_[1+j], nframes);
        }
        /*
        if ( 
        sample_t *mods[mod_port_.size()];
        for ( int j = 0; j < mod_port_.size(); j++ ){
            
            mods[j]  = (sample_t*) jack_port_get_buffer( mod_port_[j], nframes );
            
        }
        */
        
        float vol = param_[MOD_VOLUME];
        
        float dph = param_[CHORUS_DEPTH];
        float dw  = param_[CHORUS_DRYWET];
        
        
        for(jack_nframes_t i = 0; i < nframes; i++){
            
            sample_t w = 0;
            for(int k = 0; k < size; k++)
                w += depths[k] * buffer_->read(k, 1 + (dph * mod[k][i]));
            
            buffer_->write(in[i]);
            
            out[i] = vol * spi_dry_wet(in[i], w / (float)size, dw);
        }
        
    }else{
        
        memcpy( out, in, sizeof(sample_t) * nframes );
    }

    return 0;
}

/*
int Chorus::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( audio_in_[0], nframes);	// Collecting input buffer
    out = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);	// Collecting output buffer
    memcpy(out, in, sizeof(sample_t) * nframes);
    
    return 0;
}
*/

void Chorus::change_param(int idx, float value){
    
    if ( idx == CHORUS_SIZE && value > 0 && value <= MAX_CHORUS_SIZE){
        
        int *d = new int[MAX_CHORUS_SIZE];
        for( int i = 0; i < MAX_CHORUS_SIZE; i++ ){
            
            d[i] = (int)param_[CHORUS_DELAY + i];
        }
        
        buffer_->set_reader( MAX_CHORUS_SIZE, d, samplerate_ );
        
        delete d;
    }
    else if ( idx >= CHORUS_DELAY && idx < CHORUS_DELAY + MAX_CHORUS_SIZE ){
        
        int *d = new int[MAX_CHORUS_SIZE];
        for( int i = 0; i < MAX_CHORUS_SIZE; i++ ){
            
            d[i] = (int)param_[CHORUS_DELAY + i];
        }
        
        buffer_->set_reader( MAX_CHORUS_SIZE, d, samplerate_ );
        
        delete d;
    }
}

void Chorus::change_param(const float *values){
    
    int *d = new int[MAX_CHORUS_SIZE];
    for( int i = 0; i < MAX_CHORUS_SIZE; i++ ){
        
        d[i] = (int)param_[CHORUS_DELAY + i];
    }
        
    buffer_->set_reader( MAX_CHORUS_SIZE, d, samplerate_ );
    
    delete d;
}

void Chorus::new_bank(){
    
    this->add_bank( MOD_COUNT + CHORUS_PARAMS_COUNT, CHORUS_DEFAULT_PARAMS);
}
    
string Chorus::return_param_name(int idx){
    
    return CHORUS_PARAM_NAMES[idx];
}

string Chorus::return_formated_param(int idx){
    
    string n = CHORUS_PARAM_NAMES[idx];
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            break;
        
        case CHORUS_DEPTH :
        
            n += "  ";
            break;
        
        case CHORUS_DRYWET :
            
            n += " ";
            break;
            
        default :
            break;
    }
    
    n += f_ftos( param_[idx] );
    
    return n;
} 
