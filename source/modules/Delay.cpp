#include "Delay.h"
Delay::Delay(const char *server) : Module(server, MDELAY, 6, 2, 2, 0, 0, "in_L", "in_R", "out_L", "out_R"){
    
    this->buffer_L = new Ringbuffer(D_DMAX, jack_get_sample_rate(this->client));
    this->buffer_R = new Ringbuffer(D_DMAX, jack_get_sample_rate(this->client));
	
	if(this->buffer_L == NULL || this->buffer_R == NULL){
		exit(1);
	}
  
    this->reader_L = this->buffer_L->new_read_head(D_DELAY);
    this->reader_R = this->buffer_R->new_read_head(D_DELAY);
  
    this->params[0] = D_DMAX;
  
    this->params[1] = D_DELAY;
    this->params[2] = D_DELAY;
    this->params[3] = D_FEED;
    this->params[4] = D_FEED;
	
		this->params[5] = 1.0;
	
  
    if (jack_activate (this->client)) {
      fprintf (stderr, "Echec de l'activation du client\n");
      exit (1);
    }
}
		
int Delay::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *out_L;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);	//collecting Left input buffer
	out_L = (sample_t*)jack_port_get_buffer(this->port[2], nframes);	//collecting Right input buffer

	sample_t *in_R, *out_R;	
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	float fl = this->params[3];
	float fr = this->params[4];
	float dw = this->params[5];

	for(jack_nframes_t i = 0; i < nframes; i++){
		
		this->buffer_L->read_value(&(this->reader_L));
		this->buffer_R->read_value(&(this->reader_R));
			
		sample_t wl = this->reader_L.value;
		sample_t wr = this->reader_R.value;
		
		this->buffer_L->write_value(in_L[i] + fl*wl);
		this->buffer_R->write_value(in_R[i] + fr*wr);
		
		out_L[i] = spi_dry_wet(in_L[i], wl, dw);
		out_R[i] = spi_dry_wet(in_R[i], wr, dw);
	}
	return 0;
}

int Delay::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}
