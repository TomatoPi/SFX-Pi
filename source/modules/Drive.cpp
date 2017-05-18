#include "Drive.h"

/*
*	Distortion effect constructor
*	EQ creation
*/
Drive::Drive(const char *server, const char *name): Module(server, name, 4, (const char **){"in_L", "in_R", "out_L", "out_R"}
                                                                          , (const char **){JACK_DEFAULT_AUDIO_TYPE, JACK_DEFAULT_AUDIO_TYPE, JACK_DEFAULT_AUDIO_TYPE, JACK_DEFAULT_AUDIO_TYPE}
                                                                          , (unsigned long *){JackPortIsInput, JackPortIsInput, JackPortIsOutput, JackPortIsOutput}
                                                                          )
                                                    , gp(70), gn(30), sp(10), sn(5), shp(0.29), shn(0.8), is_abs(0), is_asm(1), is_soft_clip_p(1), is_soft_clip_n(0){
	
	this->filter = (spi_tripole*)malloc(sizeof(spi_tripole));
	spi_init_tripole(this->filter, 200, 1000, (int)jack_get_sample_rate(this->client), 0.75, 3.0, 7.0);
	
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
	
	float g1 = this->gp, g2 = this->gn; //collecting positive & negative gains
	
	for(jack_nframes_t i = 0; i < nframes; i++){						//For each sample
		
		sample_t l = spi_do_tripole(this->filter, s_in_l[i]);				//Current Left sample
		sample_t r = spi_do_tripole(this->filter, s_in_r[i]);				//Current Right sample
		
		if(this->is_asm){ 									//if asymmetrical clipping
														//Left Channel
			if(l>0){ 										//if positive
				if(this->is_soft_clip_p){								//soft-clipping or hard-clipping
					s_out_l[i] = spi_soft(l*g1, 0.5, this->sp, this->shp);					//soft-clipping
				}else{											//else
					s_out_l[i] = spi_clip(l*g1, -0.5, 0.5);							//hard-clipping
				}																
			}else{											//if negative	
				if(this->is_soft_clip_n){                                      				//soft-clipping or hard-clipping
					s_out_l[i] = spi_soft(l*g2, 0.5, this->sn, this->shn);          			//soft-clipping
				}else{                                                              			//else
					s_out_l[i] = spi_clip(l*g2, -0.5, 0.5);                         			//hard-clipping
				}
			}
			if(r>0){										//Same for Right Channel
				if(this->is_soft_clip_p){
					s_out_r[i] = spi_soft(r*g1, 0.5, this->sp, this->shp);
				}else{
					s_out_r[i] = spi_clip(r*g1, -0.5, 0.5);
				}
			}else{
				if(this->is_soft_clip_n){
					s_out_r[i] = spi_soft(r*g2, 0.5, this->sn, this->shn);
				}else{
					s_out_r[i] = spi_clip(r*g2, -0.5, 0.5);
				}
			}
		}else{											//if symmetrical clipping
			if(this->is_soft_clip_p){								//soft-clipping or hard-clipping
					s_out_l[i] = spi_soft(l*g1, 0.5, this->sp, this->shp);				//soft-clipping
				}else{										//else
					s_out_l[i] = spi_clip(l*g1, -0.5, 0.5);						//hard-clipping
				}
			if(this->is_soft_clip_p){							//Same for Right Channel
					s_out_r[i] = spi_soft(r*g1, 0.5, this->sp, this->shp);
				}else{
					s_out_r[i] = spi_clip(r*g1, -0.5, 0.5);
				}
				
		}
		if(this->is_abs){									//if absolute value mode is true, full-wave rectification
			s_out_l[i] = spi_abs(s_out_l[i]);
			s_out_r[i] = spi_abs(s_out_r[i]);
		}
	}
	return 0;
}
