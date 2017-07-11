#include "Drive.h"
Drive::Drive(const char *server):Module(server, MOD_DRIVE, DRIVE_PARAM_COUNT,
    1, 1, 0, 0, "In", "Out"),
    filter_(200, 1000, jack_get_sample_rate(client_))
{
    
    this->change_param(DRIVE_DEFAULT_PARAMS);
    
    if (jack_activate(this->client_)) {
        
		throw string("Failed activate client");
	}
}

int Drive::process(jack_nframes_t nframes, void *arg){
    
    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( audio_in_[0], nframes);	// Collecting input buffer
    out = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);	// Collecting output buffer

    float gp, sp, shp, gn, sn, shn;			// Collecting drive settings
    int is_abs, isp, isn;
    
    is_abs = (int)!!param_[DRIVE_ABS];
    int n = ((int)!!param_[DRIVE_ASM])?0:4;
    
    gp = 		 param_[DRIVE_GAIN_P];
    isp = (int)!!param_[DRIVE_TYPE_P];
    sp = 		 param_[DRIVE_SOFT_P];
    shp = 		 param_[DRIVE_SHAPE_P];
    
    gn = 		 param_[DRIVE_GAIN_P +n];
    isn = (int)!!param_[DRIVE_TYPE_P +n];
    sn = 		 param_[DRIVE_SOFT_P +n];
    shn = 		 param_[DRIVE_SHAPE_P +n];
    
    float gl, gm, gh;
    gl = param_[DRIVE_F_GBASS];
    gm = param_[DRIVE_F_GMID];
    gh = param_[DRIVE_F_GHIGH];
        
    for (jack_nframes_t i = 0; i < nframes; i++) {									//For each sample
        
        sample_t l = filter_.compute(in[i], gl, gm, gh);	//Compute 3bands EQ
        
        if(l>0){ 																	//if positive
            if(isp){																	//soft-clipping or hard-clipping
                out[i] = spi_soft(l*gp, 1.0, sp, shp);										//soft-clipping
            }else{																		//else
                out[i] = spi_clip(l*gp, -1.0, 1.0);											//hard-clipping
            }																
        }else{																		//if negative	
            if(isn){                                      								//soft-clipping or hard-clipping
                out[i] = spi_soft(l*gn, 1.0, sn, shn);          							//soft-clipping
            }else{                                                              		//else
                out[i] = spi_clip(l*gn, -1.0, 1.0);                         				//hard-clipping
            }
        }
        
        if(is_abs){							//if absolute value mode is true, full-wave rectification
            out[i] = spi_abs(out[i]);
        }
    }
    
    return 0;
}

int Drive::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( audio_in_[0], nframes);	// Collecting input buffer
    out = (sample_t*)jack_port_get_buffer( audio_out_[0], nframes);	// Collecting output buffer
    memcpy(out, in, sizeof(sample_t) * nframes);
    
    return 0;
}

void Drive::change_param(int idx, float value){
    
	param_[idx] = value;
    
    if (idx == DRIVE_F_BASS) {
		
		filter_.set_lf(value);
	}else if (idx == DRIVE_F_HIGH) {
		
		filter_.set_hf(value);
	}
}

void Drive::change_param(const float *values){
    
    memcpy(param_, values, sizeof(float) * param_c_);
    
    filter_.set_freq(param_[DRIVE_F_BASS], param_[DRIVE_F_HIGH]);
}

string Drive::return_param_name(int idx){
    
    return DRIVE_PARAM_NAMES[idx];
}