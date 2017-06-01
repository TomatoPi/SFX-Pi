#include "Reverb.h"

Reverb::Reverb(const char *server) : Module(server, MREV, 5, 2, 4, 0, 0, "in_L", "in_R", "out_L", "out_R", "reverb_L", "reverb_R"){

	this->buffer_L = new Ringbuffer(R_DECAYMAX, jack_get_sample_rate(this->client));
	this->buffer_R = new Ringbuffer(R_DECAYMAX, jack_get_sample_rate(this->client));
	
	this->params[0] = jack_get_sample_rate(this->client);
	this->params[1] = (int)(R_DECAYMAX/1000)*this->params[0];
	
	this->params[2] = R_PREDELAY;
	this->params[3] = R_DECAY;
	
	this->params[4] = 0.5;
	
	this->reverb_reponse = (sample_t*)malloc( (int)(this->params[1]) * sizeof(sample_t) );
	
	for(int i = 0; i < this->params[1]; i++){
	
		this->reverb_reponse[i] = exp( (i * -6.9078) / (R_DECAYMAX) ) * ( ((float)rand()/(float)RAND_MAX) * 2.0 - 1.0 );
	}
}

Reverb::~Reverb(){

	delete this->buffer_L;
	delete this->buffer_R;
	free(this->reverb_reponse);
}
	
int Reverb::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *in_R;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	
	sample_t *out_L, *out_R;
	out_L = (sample_t*)jack_port_get_buffer(this->port[2], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
		
	sample_t *rev_L, *rev_R;
	rev_L = (sample_t*)jack_port_get_buffer(this->port[4], nframes);
	rev_R = (sample_t*)jack_port_get_buffer(this->port[5], nframes);
	
	float reponse_size = this->params[1];
	float predelay = this->params[2];
	float decay = this->params[3];
	float dw = this->params[4];
	
	for(jack_nframes_t i = 0; i < nframes; i++){
	
		this->buffer_L->write_value(in_L[i]);
		this->buffer_R->write_value(in_R[i]);
	
		rev_L[i] = do_convolution(this->buffer_L, predelay, this->reverb_reponse, reponse_size, decay);
		rev_R[i] = do_convolution(this->buffer_R, predelay, this->reverb_reponse, reponse_size, decay);
		
		out_L[i] = spi_dry_wet(in_L[i], rev_L[i], dw);
		out_R[i] = spi_dry_wet(in_R[i], rev_R[i], dw);
	}
	
	return 0;
}

int Reverb::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}
