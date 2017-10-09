#include "Drive.h"

sample_t clip_hard( sample_t, float, float);
sample_t clip_soft( sample_t, float, float);
sample_t clip_pow2( sample_t, float, float);
sample_t clip_vars( sample_t, float, float);

/**********************************************************************
 *                      Drive's Functions Bodies
 **********************************************************************/
Drive::Drive( int id ):Module(MOD_DRIVE, id, DRIVE_PARAM_COUNT,
    1, 1, 0, 0, "In", "Out"),
    filter_(200, 1000, jack_get_sample_rate(client_))
{
    
    this->set_param(MOD_COUNT + DRIVE_PARAM_COUNT, DRIVE_DEFAULT_PARAMS);
    
    if (jack_activate(this->client_)) {
        
		throw string("Failed activate client");
	}
}

inline int Drive::do_process(jack_nframes_t nframes){
    
    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( audio_in_[0], nframes);
    out = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);

    if ( !is_bypassed_ ){
        
        // Collect Drive settings
        float gp, sp, shp, gn, sn, shn;
        int is_abs;
        
        float vol = param_[MOD_VOLUME];
        
        is_abs = (int)!!param_[DRIVE_ABS];
        int n  = ((int)!param_[DRIVE_ASM])?0:4;
        
        gp = 		 param_[DRIVE_GAIN_P];
        sp = 		 param_[DRIVE_SOFT_P];
        shp = 		 param_[DRIVE_SHAPE_P];
        
        gn = 		 param_[DRIVE_GAIN_P +n];
        sn = 		 param_[DRIVE_SOFT_P +n];
        shn = 		 param_[DRIVE_SHAPE_P +n];

        float o = param_[DRIVE_OFFSET];
        
        float gl, gm, gh;
        gl = param_[DRIVE_F_GBASS];
        gm = param_[DRIVE_F_GMID];
        gh = param_[DRIVE_F_GHIGH];
            
        for (jack_nframes_t i = 0; i < nframes; i++) {									
            
            // Compte 3Band Filtering
            sample_t l = filter_.compute(in[i], gl, gm, gh);
            
            // Call right Clipping function if signal is pos or negative
            if ( l > 0 ){															
                out[i] = vol * (*clip_p_)(l*gp + o, sp, shp);						
            }else{
                out[i] = vol * (*clip_n_)(l*gn + o, sn, shn);
            }
            
            // If full Wave rectification enabled, compute it
            if(is_abs){
                out[i] = spi_abs(out[i]) * 2 - 1;
            }
        }
    }else{
        
        memcpy( out, in, sizeof(sample_t) * nframes );
    }
    
    return 0;
}


void Drive::change_param(int idx, float value){
    
    if (idx == DRIVE_F_BASS) {
		
		filter_.set_lf(value);
	}else if (idx == DRIVE_F_HIGH) {
		
		filter_.set_hf(value);
	}
    
    this->update();
}

void Drive::change_param(const float *values){
    
    filter_.set_freq(param_[DRIVE_F_BASS], param_[DRIVE_F_HIGH]);

    this->update();
}

void Drive::new_bank(){
    
    this->add_bank( MOD_COUNT + DRIVE_PARAM_COUNT, DRIVE_DEFAULT_PARAMS);
}

string Drive::return_param_name(int idx){
    
    return DRIVE_PARAM_NAMES[idx];
}

string Drive::return_formated_param(int idx){
    
    string n = DRIVE_PARAM_NAMES[idx];
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_ABS :
        
            n += (!!param_[idx])?"  Y":"  N";
            break;
        
        case DRIVE_ASM :
        
            n += (!!param_[idx])?" Y":" N";
            break;
            
        case DRIVE_GAIN_P :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
           
        case DRIVE_TYPE_P :
            
            n += (!!param_[idx])?" Soft":" Hard";
            break;
            
        case DRIVE_SOFT_P :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
           
        case DRIVE_SHAPE_P :
            
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_GAIN_N :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
           
        case DRIVE_TYPE_N :
            
            n += (!!param_[idx])?" Soft":" Hard";
            break;
            
        case DRIVE_SOFT_N :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_SHAPE_N :
            
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_F_BASS :
            
            n += " ";
            n += f_ftos( param_[idx] );
            break;
            
        case DRIVE_F_HIGH :
            
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_F_GBASS :
            
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_F_GMID :
            
            n += f_ftos( param_[idx] );
            break;
        
        case DRIVE_F_GHIGH :
            
            n += f_ftos( param_[idx] );
            break;
        
        default :
            break;
    }
    
    return n;
}

void Drive::update(){
    
    param_[DRIVE_TYPE_P] = (int)param_[DRIVE_TYPE_P];
    param_[DRIVE_TYPE_N] = (int)param_[DRIVE_TYPE_N];
    
    this->set_clipping( static_cast<DISTORTION_FORM>(param_[DRIVE_TYPE_P]),
                        static_cast<DISTORTION_FORM>(param_[DRIVE_TYPE_N]) );
    
    param_[DRIVE_ABS] = !!param_[DRIVE_ABS];
    param_[DRIVE_ASM] = !!param_[DRIVE_ASM];
    param_[DRIVE_TYPE_P] = !!param_[DRIVE_TYPE_P];
    param_[DRIVE_TYPE_N] = !!param_[DRIVE_TYPE_N];
}

void Drive::set_clipping( DISTORTION_FORM formp , DISTORTION_FORM formn){
    
    switch ( formn ){
        
        case SOFT_CLIP :
            this->clip_n_ = clip_soft;
            break;
        case HARD_CLIP :
            this->clip_n_ = clip_hard;
            break;
        case POW2 :
            this->clip_n_ = clip_pow2;
            break;
        case VAR_SOFT :
            this->clip_n_ = clip_vars;
            break;
        default :
            this->clip_n_ = clip_hard;
            break;
    }
    switch ( formp ){
        
        case SOFT_CLIP :
            this->clip_p_ = clip_soft;
            break;
        case HARD_CLIP :
            this->clip_p_ = clip_hard;
            break;
        case POW2 :
            this->clip_p_ = clip_pow2;
            break;
        case VAR_SOFT :
            this->clip_p_ = clip_vars;
            break;
        default :
            this->clip_p_ = clip_hard;
            break;
    }
}

inline sample_t clip_hard( sample_t in, float p1, float p2 ){

    return spi_clip( in, -1.0, 1.0 );
}

inline sample_t clip_pow2( sample_t in, float p1, float p2 ){

    return spi_clip( in*in - 1 , -1.0, 1.0 );
}

inline sample_t clip_soft( sample_t in, float p1, float p2 ){
    
    return spi_soft( in, 1.0f, p1, p2 );
}

inline sample_t clip_vars( sample_t in, float p1, float p2 ){

    float a = sin( (p1 + 1)*M_PI / (202) );
    float k = 2*a*(1 - a); 

    return (1 + k)*in / ( 1 + k * spi_abs(in) );
}
