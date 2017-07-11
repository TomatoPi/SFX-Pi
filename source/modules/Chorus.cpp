#include "Chorus.h"
Chorus::Chorus(const char* server):Module(server, MOD_CHORUS, CHORUS_PARAMS_COUNT,
    2, 1, 0, 0, "In", "Mod", "Out"),
    samplerate_(jack_get_sample_rate(client_))
{

    buffer_ = new Buffer_M( (CHORUS_SIZE+2)*9, this->samplerate_, CHORUS_SIZE, (int*)CHORUS_DELAYS_LENGTH);
	this->change_param(CHORUS_DEFAULT_PARAMS);
    
    if (jack_activate(this->client_)) {
        
		throw string("Failed activate client");
	}
}

Chorus::~Chorus(){
    
    delete buffer_;
}

int Chorus::process(jack_nframes_t nframes, void *arg){
	
    sample_t *in, *mod, *out;
    in  = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    mod = (sample_t*)jack_port_get_buffer(audio_in_[1], nframes);
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    float dph = param_[CHORUS_DEPTH];
    float dw  = param_[CHORUS_DRYWET];
    
    for(int i = 0; i < nframes; i++){
        
        sample_t w = 0;
        for(int k = 0; k < CHORUS_SIZE; k++)
            w += buffer_->read(k, 1 + (dph * mod[i]));
        
        buffer_->write(in[i]);
        
        out[i] = spi_dry_wet(in[i], w / (float)CHORUS_SIZE, dw);
    }

    return 0;
}

int Chorus::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( audio_in_[0], nframes);	// Collecting input buffer
    out = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);	// Collecting output buffer
    memcpy(out, in, sizeof(sample_t) * nframes);
    
    return 0;
}

void Chorus::change_param(int idx, float value){
    
    param_[idx] = value;
}

void Chorus::change_param(const float *values){
    
    memcpy(param_, values, sizeof(float) * param_c_);
}
    
string Chorus::return_param_name(int idx){
    
    return CHORUS_PARAM_NAMES[idx];
}