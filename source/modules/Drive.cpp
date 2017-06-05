#include "Drive.h"

/*
*	Distortion effect constructor
*	EQ creation
*/
Drive::Drive(const char *server): Module(server, MDRIVE, 15, 2, 2, 0, 0, "in_L", "in_R", "out_L", "out_R"){

	this->params[0] = D_ABS;
	this->params[1] = D_ASM;
	
	this->params[2] = D_GAIN;
	this->params[3] = D_TYPE;
	this->params[4] = D_SOFT;
	this->params[5] = D_SHAPE;
	
	this->params[6] = D_GAIN;
	this->params[7] = D_TYPE;
	this->params[8] = D_SOFT;
	this->params[9] = D_SHAPE;
	
	this->params[10] = 200;
	this->params[11] = 1000;
	this->params[12] = 0.75;
	this->params[13] = 3.0;
	this->params[14] = 7.0;
	
	filter_L = new Filter_3EQ(this->params[10], this->params[11], (int)jack_get_sample_rate(this->client));
	filter_R = new Filter_3EQ(this->params[10], this->params[11], (int)jack_get_sample_rate(this->client));
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

Drive::~Drive(){
	
	delete this->filter_L;
	delete this->filter_R;
}

/*
*	Drive effect's Callback
*/
int Drive::process(jack_nframes_t nframes, void *arg){
	
	sample_t *in_l, *out_l;	
	in_l = (sample_t*)jack_port_get_buffer(this->port[0], nframes);	//collecting Left input buffer
	out_l = (sample_t*)jack_port_get_buffer(this->port[2], nframes);	//collecting Right input buffer
	
	sample_t *in_r, *out_r;	
	in_r = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	out_r = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	float gp, sp, shp, gn, sn, shn;			//Collecting drive settings
	int is_abs, isp, isn;
	
	is_abs = (int)this->params[0];
	int n = (this->params[1] == 0.0)?0:4;
	
	gp = this->params[2];
	isp = (int)this->params[3];
	sp = this->params[4];
	shp = this->params[5];
	
	gn = this->params[2+n];
	isn = (int)this->params[3+n];
	sn = this->params[4+n];
	shn = this->params[5+n];
	
	float gl, gm, gh;
	gl = this->params[12];
	gm = this->params[13];
	gh = this->params[14];
		
	for(jack_nframes_t i = 0; i < nframes; i++){						//For each sample
		
		sample_t l = this->filter_L->compute(in_l[i], gl, gm, gh);				//Current Left sample
		sample_t r = this->filter_R->compute(in_r[i], gl, gm, gh);				//Current Right sample
											//Left Channel
		if(l>0){ 										//if positive
			if(isp){								//soft-clipping or hard-clipping
				out_l[i] = spi_soft(l*gp, 1.0, sp, shp);					//soft-clipping
			}else{											//else
				out_l[i] = spi_clip(l*gp, -1.0, 1.0);							//hard-clipping
			}																
		}else{											//if negative	
			if(isn){                                      				//soft-clipping or hard-clipping
				out_l[i] = spi_soft(l*gn, 1.0, sn, shn);          			//soft-clipping
			}else{                                                              			//else
				out_l[i] = spi_clip(l*gn, -1.0, 1.0);                         			//hard-clipping
			}
		}
		if(r>0){										//Same for Right Channel
			if(isp){
				out_r[i] = spi_soft(r*gp, 1.0, sp, shp);
			}else{
				out_r[i] = spi_clip(r*gp, -1.0, 1.0);
			}
		}else{
			if(isn){
				out_r[i] = spi_soft(r*gn, 1.0, sn, shn);
			}else{
				out_r[i] = spi_clip(r*gn, -1.0, 1.0);
			}
		}
		if(is_abs){									//if absolute value mode is true, full-wave rectification
			out_l[i] = spi_abs(out_l[i]);
			out_r[i] = spi_abs(out_r[i]);
		}
	}
	return 0;
}

int Drive::bypass(jack_nframes_t nframes, void *arg){
	
	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}

int Drive::set_param(int param, float var){
	
	if(param == 10){
		
		this->filter_L->set_lf(var);
		this->filter_R->set_lf(var);
		this->params[10] = var;
		return 0;
	}else if(param == 11){
		
		this->filter_L->set_hf(var);
		this->filter_R->set_hf(var);
		this->params[11] = var;
		return 0;
	}else{
		
		return this->Module::set_param(param, var);
	}
}


int Drive::set_param_list(int size, float *params){
	
	if(!this->Module::set_param_list(size, params)){
		this->filter_L->set_freq(this->params[10], this->params[11]);
		this->filter_R->set_freq(this->params[10], this->params[11]);
		return 0;
	}
	return 1;
}