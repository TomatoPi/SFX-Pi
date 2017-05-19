#include "Drive.h"

/*
*	Distortion effect constructor
*	EQ creation
*/
Drive::Drive(const char *server, const char *name): Module(server, name, 10, 2, 2, 0, 0, "in_L", "in_R", "out_L", "out_R"){
	
	this->filter = (spi_tripole*)malloc(sizeof(spi_tripole));
	spi_init_tripole(this->filter, 200, 1000, (int)jack_get_sample_rate(this->client), 0.75, 3.0, 7.0);

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
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

/*
*	Drive effect's Callback
*/
int Drive::process(jack_nframes_t nframes, void *arg){
	
	sample_t *s_in_l, *s_out_l;	
	s_in_l = (sample_t*)jack_port_get_buffer(this->port[0], nframes);	//collecting Left input buffer
	s_out_l = (sample_t*)jack_port_get_buffer(this->port[2], nframes);	//collecting Right input buffer
	
	sample_t *s_in_r, *s_out_r;	
	s_in_r = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	s_out_r = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
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
	
	for(jack_nframes_t i = 0; i < nframes; i++){						//For each sample
		
		sample_t l = spi_do_tripole(this->filter, s_in_l[i]);				//Current Left sample
		sample_t r = spi_do_tripole(this->filter, s_in_r[i]);				//Current Right sample
											//Left Channel
		if(l>0){ 										//if positive
			if(isp){								//soft-clipping or hard-clipping
				s_out_l[i] = spi_soft(l*gp, 0.5, sp, shp);					//soft-clipping
			}else{											//else
				s_out_l[i] = spi_clip(l*gp, -0.5, 0.5);							//hard-clipping
			}																
		}else{											//if negative	
			if(isn){                                      				//soft-clipping or hard-clipping
				s_out_l[i] = spi_soft(l*gn, 0.5, sn, shn);          			//soft-clipping
			}else{                                                              			//else
				s_out_l[i] = spi_clip(l*gn, -0.5, 0.5);                         			//hard-clipping
			}
		}
		if(r>0){										//Same for Right Channel
			if(isp){
				s_out_r[i] = spi_soft(r*gp, 0.5, sp, shp);
			}else{
				s_out_r[i] = spi_clip(r*gp, -0.5, 0.5);
			}
		}else{
			if(isn){
				s_out_r[i] = spi_soft(r*gn, 0.5, sn, shn);
			}else{
				s_out_r[i] = spi_clip(r*gn, -0.5, 0.5);
			}
		}
		if(is_abs){									//if absolute value mode is true, full-wave rectification
			s_out_l[i] = spi_abs(s_out_l[i]);
			s_out_r[i] = spi_abs(s_out_r[i]);
		}
	}
	return 0;
}
