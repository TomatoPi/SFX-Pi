#include "LFO.h"

LFO::LFO(const char *server, const char *name): Module(server, name, 8, 0, 1, 0, 0, "out"){
		
	lfo_set_type(this, L_TYPE);
	this->params[1] = L_FREQ;
	this->params[2] = jack_get_sample_rate(this->client);
	
	this->params[3] = L_RAMP;
	this->params[4] = L_PHASE;
	this->params[5] = L_SIGN;
	
	this->params[6] = L_PAR1;
	this->params[7] = L_PAR2;
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

int LFO::process(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	
	float f = this->params[1]; 		//LFO frequency
	float sr = this->params[2];		//Client Samplerate
		
	float ramp = this->params[3];		//Current LFO value
	float phase = this->params[4];		//LFO Phase
	int s = (this->params[5] < 0)? -1: 1;	//LFO sign
		
	float p1 = this->params[6];		//waveshape param 1
	float p2 = this->params[7];		//waveshape param 2
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		ramp += f/sr;
		ramp = fmod(ramp, 1.0);	
		
		out[i] = (*(this->waveform))(ramp, s, p1, p2);
	}
	
	this->params[3] = ramp;
	return 0;
}

int Module::bypass(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	memset(out, 0.0, sizeof(sample_t) * nframes);
	return 0;
}

void lfo_set_type(LFO *lfo, LFO_Wave type){
	lfo->params[0] = type;
	switch(type){
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
