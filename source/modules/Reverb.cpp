#include "Reverb.h"

Reverb::Reverb(const char *server) : Module(server, MREV, REVERB_PARAMS_COUNT, 2, 4, 0, 0, "in_L", "in_R", "out_L", "out_R", "reverb_L", "reverb_R"){
	
	int sr = jack_get_sample_rate(this->client);

	for(int i = 0; i < COMBCOUNT; i++){
		this->comb_L[i] = new Filter_Comb(R_DAMP, R_ROOM, combsize[i] * sr / 44100, sr, 0);
		this->comb_R[i] = new Filter_Comb(R_DAMP, R_ROOM, STEREOSPREAD + (combsize[i] * sr / 44100), sr, 0);
	}
	for(int i = 0; i < ALLPCOUNT; i++){
		
		this->allp_L[i] = new Filter_Allpass(R_ALLF, allpsize[i]);
		this->allp_R[i] = new Filter_Allpass(R_ALLF, STEREOSPREAD + allpsize[i]);
	}
	
	this->params[REVERB_GAIN] = R_GAIN;
	
	this->params[REVERB_DRY] = R_DRY;
	this->params[REVERB_WET1] = (1.0f/scalewet)*((R_WIDTH/2.0f) + 0.5f);
	this->params[REVERB_WET2] = (1.0f/scalewet)*((1-R_WIDTH)/2.0f);
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

Reverb::~Reverb(){

	for(int i = 0; i < COMBCOUNT; i++){
	
		delete this->comb_L[i];
		delete this->comb_R[i];
	}

	for(int i = 0; i < ALLPCOUNT; i++){
		
		delete this->allp_L[i];
		delete this->allp_R[i];
	}
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
	
	float gain = this->params[REVERB_GAIN];
	
	float dry = this->params[REVERB_DRY];
	float wet1 = this->params[REVERB_WET1];
	float wet2 = this->params[REVERB_WET2];
	
	for(jack_nframes_t i = 0; i < nframes; i++){

		sample_t input = (in_L[i] + in_R[i]) * gain;
		rev_L[i] = 0;
		rev_R[i] = 0;
		
		for(int k = 0; k < COMBCOUNT; k++){
			
			rev_L[i] += this->comb_L[k]->compute(input);
			rev_R[i] += this->comb_R[k]->compute(input);
		}
		
		for(int k = 0; k < ALLPCOUNT; k++){
			
			rev_L[i] = this->allp_L[k]->compute(rev_L[i]);
			rev_R[i] = this->allp_R[k]->compute(rev_R[i]);
		}
		
		out_L[i] = rev_L[i]*wet1 + rev_R[i]*wet2 + in_L[i]*dry;
		out_R[i] = rev_R[i]*wet1 + rev_L[i]*wet2 + in_R[i]*dry;
	}
	
	return 0;
}

int Reverb::bypass(jack_nframes_t nframes, void *arg){

	return mod_GenericStereoBypass_Callback(nframes, this->port, 2);
}
