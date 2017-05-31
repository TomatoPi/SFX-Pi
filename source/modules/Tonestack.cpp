#include "Tonestack.h"

Tonestack::Tonestack(const char *server): Module(server, MTONE, 5, 2, 2, 0, 0, "in_L", "in_R", "out_L", "out_R"){
	
	this->filter = (spi_tripole*)malloc(sizeof(spi_tripole));
	spi_init_tripole(this->filter);
	
	this->params[0] = 200;
	this->params[1] = 1000;
	this->params[2] = 0.75;
	this->params[3] = 3.0;
	this->params[4] = 7.0;
	
	spi_init_tripole_freq(this->filter, this->params[0], this->params[1], (int)jack_get_sample_rate(this->client));
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

Tonestack::~Tonestack(){

	free(this->filter);
}

int Tonestack::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *out_L;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	out_L = (sample_t*)jack_port_get_buffer(this->port[2], nframes);
	
	sample_t *in_R, *out_R;	
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	float gl, gm, gh;
	gl = this->params[2];
	gm = this->params[3];
	gh = this->params[4];
	
	if(this->filter->fl_bak != this->params[0] || this->filter->fh_bak != this->params[1])
		spi_init_tripole_freq(this->filter, this->params[0], this->params[1] ,(int)jack_get_sample_rate(this->client));
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		out_L[i] = spi_do_tripole(this->filter, in_L[i], gl, gm, gh);
		out_R[i] = spi_do_tripole(this->filter, in_R[i], gl, gm, gh);
	}
	return 0;	
}

int Tonestack::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}
