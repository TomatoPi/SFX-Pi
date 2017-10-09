#include "LFO.h"

static int RandSeed = 48172;

LFO::LFO( int id ): Module(MOD_LFO, id, LFO_PARAMS_COUNT,
    0, 1, 0, 0, "Out"),
    samplerate_(jack_get_sample_rate(client_)),
    ramp_(0),
    rbs_(0)
{
    
    this->set_param(MOD_COUNT + LFO_PARAMS_COUNT, LFO_DEFAULT_PARAMS);
    
    if (jack_activate (this->client_)) {
        
        throw string("Failed activate client");
    }
}

inline int LFO::do_process(jack_nframes_t nframes){

    sample_t *out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
    
    if ( !is_bypassed_ ){
        
        float vol = param_[MOD_VOLUME];
        
        float d = param_[LFO_PHD];
        float o = param_[LFO_PHO];
        
        for(jack_nframes_t i = 0; i < nframes; i++){
            
            // Calculate Phase
            ramp_ += rbs_;
            ramp_ = fmod(ramp_ - o, 1.0f);
            
            // Compute Phase distortion
            float p = ramp_;
            if ( d != 0.5f )
               p = (ramp_>d)?( (ramp_ + 1.0f - 2.0f*d)/(2.0f*(1.0f-d)) ):( ramp_ / (2.0f*d) );
            p += o;
            p = fmod( p, 1.0f );
        
            // Get Sign
            int s = (param_[LFO_SIGN] < 0)? -1: 1;
            
            // Compute waveform and output LFO
            out[i] = vol * (*waveform_)(p, s, param_[LFO_VAR1], param_[LFO_VAR2]);
        }
    }else{
        
        memset( out, 0, sizeof(sample_t) * nframes );
    }

    return 0;
}

void LFO::sync(){
    
    ramp_ = param_[LFO_PHASE];
}

void LFO::change_param(int idx, float value){
    
    if (idx == LFO_TYPE) {
        
        value = ( value < 0 )? 0 : ( value >= WAVE_COUNT )? WAVE_COUNT - 1 : value;
        
        this->update_type(static_cast<LFO_wave>((int)value));
        param_[LFO_TYPE] = (int)value;
    }
    else if ( idx == LFO_FREQ ){
        
        rbs_ = value/(float)samplerate_;
    }
}

void LFO::change_param(const float *values){
    
    int v = (int)param_[LFO_TYPE];
    param_[LFO_TYPE] = ( v < 0 )? 0 : ( v >= WAVE_COUNT )? WAVE_COUNT - 1 : v;
    param_[LFO_TYPE] = v;
    this->update_type(static_cast<LFO_wave>((int)param_[LFO_TYPE]));
    
    rbs_ = param_[LFO_FREQ]/samplerate_;
}

void LFO::new_bank(){
    
    this->add_bank( MOD_COUNT + LFO_PARAMS_COUNT, LFO_DEFAULT_PARAMS);
}

string LFO::return_param_name(int idx) {
    
    return LFO_PARAM_NAMES[idx];
}

string LFO::return_formated_param(int idx){
    
    string n = string("");
    
    if ( idx != LFO_TYPE ){
        
        n += LFO_PARAM_NAMES[idx];
    } 
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case LFO_TYPE :
        
            switch ( static_cast<LFO_wave>((int)param_[LFO_TYPE]) ){
                
                case WAVE_SIN:
                
                    n += "Sinus";
                    break;
                    
                case WAVE_SQR:
                    
                    n += "Square";
                    break;
                    
                case WAVE_TRI:
                    
                    n += "Triangle";
                    break;
                    
                case WAVE_SAW:
                    
                    n += "Saw";
                    break;
                    
                case WAVE_VAR:
                    
                    n += "Varislope";
                    break;
                    
                case WAVE_NPH:
                
                    n += "NPhase";
                    break;
                    
                case WAVE_WHI:
                
                    n += "White Noise";
                    break;
                    
                default:
                    break;
            }
            break;
        
        case LFO_FREQ :
            
            n += "   ";
            n += f_ftos( param_[idx] );
            break;
            
        case LFO_PHASE :
            
            n += "  ";
            n += f_ftos( param_[idx] );
            break;
        
        case LFO_SIGN :
            
            n += (param_[idx] < 0)?"    pos":"    neg";
            break;
            
        case LFO_VAR1 :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case LFO_VAR2 :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        default :
            break;
    }
    
    
    return n;
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
        case WAVE_CST:
            this->waveform_ = w_cst;
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

inline sample_t w_var(float in, float sign, float p1, float p2){
    
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
    
    return (sample_t)( ((((float) rand()) / (float)RAND_MAX ) * ( 2.0f )) - 1.0f );
}

inline sample_t w_cst(float in, float sign, float p1, float p2){

    return p1;
}
