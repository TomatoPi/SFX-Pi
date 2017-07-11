#include "LFO.h"
LFO::LFO(const char *server): Module(server, MOD_LFO, LFO_PARAMS_COUNT,
    0, 1, 0, 0, "Out"),
    samplerate_(jack_get_sample_rate(client_)),
    ramp_(0)
{
    
    this->change_param(LFO_DEFAULT_PARAMS);
    
    if (jack_activate (this->client_)) {
        
        throw string("Failed activate client");
    }
}

int LFO::process(jack_nframes_t nframes, void *arg){

    sample_t *out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    for(jack_nframes_t i = 0; i < nframes; i++){
        
        ramp_ += param_[LFO_FREQ]/samplerate_;
        ramp_ = fmod(ramp_, 1.0);
    
        int s = (param_[LFO_SIGN] < 0)? -1: 1;
        
        out[i] = (*waveform_)(ramp_, s, param_[LFO_VAR1], param_[LFO_VAR2]);
    }

    return 0;
}

void LFO::sync(){
    
    ramp_ = param_[LFO_PHASE];
}

int LFO::bypass(jack_nframes_t nframes, void *arg){

    sample_t *out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    memset(out, 0.0f, sizeof(sample_t) * nframes);
    
    return 0;
}

void LFO::change_param(int idx, float value){
    
    param_[idx] = value;
    
    if (idx == LFO_TYPE) {
        
        this->update_type(static_cast<LFO_wave>((int)value));
    }
}

void LFO::change_param(const float *values){
    
    memcpy(param_, values, sizeof(float) * param_c_);
    
    this->update_type(static_cast<LFO_wave>((int)param_[LFO_TYPE]));
}

string LFO::return_param_name(int idx) {
    
    return LFO_PARAM_NAMES[idx];
}

void LFO::update_type(LFO_wave type){
    
    switch(type){
        case WAVE_SIN:
            this->waveform_ = w_sin;
            break;
        case WAVE_SQR:
            this->waveform_ = w_sqr;
            break;
        case WAVE_TRI:
            this->waveform_ = w_tri;
            break;
        case WAVE_SAW:
            this->waveform_ = w_saw;
            break;
        case WAVE_VAR:
            this->waveform_ = w_var;
            break;
        case WAVE_NPH:
            this->waveform_ = w_nph;
            break;
        case WAVE_WHI:
            this->waveform_ = w_whi;
            break;
        default:
            this->waveform_ = w_sin;
            break;
    }
}

inline sample_t w_sin(float in, float sign, float p1, float p2){
    
    return (sample_t) (sign*sin(M_PI * 2.0 * in));
}

inline sample_t w_sqr(float in, float sign, float p1, float p2){
    
    return (sample_t) ((in-0.5 < 0)? sign * (-1.0) : sign*1.0);
}

inline sample_t w_tri(float in, float sign, float p1, float p2){
    
    return (sample_t) (sign * ( 2.0*spi_abs(2.0*in -1.0) - 1.0));
}

inline sample_t w_saw(float in, float sign, float p1, float p2){
    
    return (sample_t) (sign * (2.0*in -1.0));
}

sample_t w_var(float in, float sign, float p1, float p2){
    
    if(in < p1){
        return (sample_t)(sign * (2.0*((p2*in)/p1) - 1.0));
    }else{
        return (sample_t)(sign * (1.0 - 2.0*p2*(in-p1)/(1-p1)));
    }
}

inline sample_t w_nph(float in, float sign, float p1, float p2){
    
    return (sample_t)(sign * sin(M_PI * 2.0 * (in + (((float)((int)(in*(p1+1))))/p2))));
}

inline sample_t w_whi(float in, float sign, float p1, float p2){
    
    return ((float)(((RandSeed = RandSeed * 214013L + 2531011L) >> 16) & 0x7fff)/RAND_MAX) * 2.0f - 1.0f;
}