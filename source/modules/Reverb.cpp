#include "Reverb.h"

Reverb::Reverb(const char *server) : Module(server, MREV, 8, 2, 4, 0, 0, "in_L", "in_R", "out_L", "out_R", "reverb_L", "reverb_R"){
	
	this->params[0] = jack_get_sample_rate(this->client);
	
	this->params[1] = R_TAP;
	this->params[2] = R_SEP;
	
	this->params[3] = R_PREDELAY;
	this->params[4] = R_DECAY;
	
	this->params[5] = 0.6f;
	
	this->params[6] = R_CUT;
	
	this->params[7] = 0.5f;
	
	this->buffer_L = new Ringbuffer(R_DECAYMAX, this->params[0]);
	this->buffer_R = new Ringbuffer(R_DECAYMAX, this->params[0]);

	this->reader_L = (rng_reader*) malloc(this->params[1] * sizeof(rng_reader));
	this->reader_R = (rng_reader*) malloc(this->params[1] * sizeof(rng_reader));
	
	for(int i = 0; i < this->params[1]; i++){
		
		this->reader_L[i] = this->buffer_L->new_read_head(this->params[3] + (float)i * spi_frand(0.7f, 1.3f)*this->params[2]);
		this->reader_R[i] = this->buffer_R->new_read_head(this->params[3] + (float)i * spi_frand(0.7f, 1.3f)*this->params[2]);
	}
	
	this->filter_L = (spi_tripole*)malloc(sizeof(spi_tripole));
	this->filter_R = (spi_tripole*)malloc(sizeof(spi_tripole));
	spi_init_tripole(this->filter_L);
	spi_init_tripole(this->filter_R);
	spi_init_tripole_freq(this->filter_L, 200.0, this->params[6], this->params[0]);
	spi_init_tripole_freq(this->filter_R, 200.0, this->params[6], this->params[0]);
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

Reverb::~Reverb(){

	delete this->buffer_L;
	delete this->buffer_R;
	free(this->filter_L);
	free(this->filter_R);
	free(this->reader_L);
	free(this->reader_R);
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
	
	float tapcount = this->params[1];
	
	float predelay = this->params[3];
	
	float fb = pow(10.0f, -3.0f * this->params[2] / this->params[4]);
	float ff = this->params[5];
	
	float dw = this->params[7];
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		sample_t l = 0.0f;
		sample_t r = 0.0f;
	//*
		for(int i = 0; i < tapcount; i++){
			
			this->buffer_L->read_value(this->reader_L +i);
			this->buffer_R->read_value(this->reader_R +i);
			
			l += this->reader_L[i].value / (float)tapcount;
			r += this->reader_R[i].value / (float)tapcount;
		}
	//*/
	
		rev_L[i] = l ;//- ff*in_L[i];
		rev_R[i] = r ;//- ff*in_R[i];
		
		this->buffer_L->write_value(0.5f * (in_L[i] + fb * (spi_do_tripole(this->filter_L, l, 0.6, 1.0, 0.1)) ) );
		this->buffer_R->write_value(0.5f * (in_R[i] + fb * (spi_do_tripole(this->filter_R, r, 0.6, 1.0, 0.1)) ) );
	
		out_L[i] = spi_dry_wet(in_L[i], rev_L[i], dw);
		out_R[i] = spi_dry_wet(in_R[i], rev_R[i], dw);
	}
	
	return 0;
}

int Reverb::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}
