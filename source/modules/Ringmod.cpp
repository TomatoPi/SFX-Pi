#include "Ringmod.h"

Ringmod::Ringmod(const char *server, const char *name): Module(server, name, 1, 3, 2, 0, 0, "in_L", "in_R", "in_M", "out_L", "out_R"){

	this->params[0] = R_DEPTH;
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

int Ringmod::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *in_R, *out_L, *out_R, *in_Mod;
	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	out_L = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[4], nframes);
	
	in_Mod = (sample_t*)jack_port_get_buffer(this->port[2], nframes);
	
	float depth = this->params[0];
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		out_L[i] = (in_L[i] * (1 - depth)) + (in_L[i] * in_Mod[i] * depth);
		out_R[i] = (in_R[i] * (1 - depth)) + (in_R[i] * in_Mod[i] * depth);
	}
	return 0;
}

int Ringmod::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 3);
}
