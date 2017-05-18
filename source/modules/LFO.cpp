#include "LFO.h"

LFO::LFO(const char *server, const char *name): Module(server, name, 1	, const char **p = {"out"}
						      			, const char **d = {JACK_DEFAULT_AUDIO_TYPE}
						      			, unsigned long *t = {JackPortIsOutput}
						      			)
						, type(WAVE_NPH), f(440), ramp(0.0), phase(0.0), sign(1.0), p1(20), p2(3){
	
	lfo_set_type(this, this->type);
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
	
	this->sr = jack_get_sample_rate(this->client);
}

int LFO::process(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->out, nframes);
	float ramp = this->ramp;
	int s = (this->sign < 0)? -1: 1;
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		ramp += f/(float)this->sr;
		ramp = fmod(ramp, 1.0);	
		
		out[i] = (*(this->waveform))(ramp, this->sign, this->p1, this->p2);
	}
	
	this->ramp = ramp;
	return 0;
}

void lfo_set_type(LFO *lfo, LFO_Wave type){
	
	switch(lfo->type = type){
		case WAVE_SIN:
			lfo->waveform = w_sin;
			break;
		case WAVE_SQR:
			lfo->waveform = w_sqr;
			break;
		case WAVE_TRI:
			lfo->waveform = w_tri;
			break;
		case WAVE_SAW:
			lfo->waveform = w_saw;
			break;
		case WAVE_VAR:
			lfo->waveform = w_var;
			break;
		case WAVE_NPH:
			lfo->waveform = w_nph;
			break;
		default:
			lfo->waveform = w_sin;
			break;
	}
}

sample_t w_sin(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign*0.5*sin(M_PI * 2.0 * in));
}

sample_t w_sqr(float in, float sign, float p1, float p2){
	
	return (sample_t) ((in-0.5 < 0)? sign * (-0.5) : sign*0.5);
}

sample_t w_tri(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign * 0.5 * ( 2.0*spi_abs(2.0*in -1.0) - 1.0));
}

sample_t w_saw(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign * 0.5 * (2.0*in -1.0));
}

sample_t w_var(float in, float sign, float p1, float p2){
	
	if(in < p1){
		return (sample_t)(sign * 0.5 * (2.0*((p2*in)/p1) - 1.0));
	}else{
		return (sample_t)(sign * 0.5 * (1.0 - 2.0*p2*(in-p1)/(1-p1)));
	}
}

sample_t w_nph(float in, float sign, float p1, float p2){
	
	return (sample_t)(sign * 0.5 * sin(M_PI * 2.0 * (in + (((float)((int)(in*(p1+1))))/p2))));
}
