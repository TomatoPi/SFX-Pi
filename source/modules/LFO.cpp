#include "LFO.h"

LFO::LFO(const char *server): Module(server, MLFO, 8, 0, 1, 0, 0, "out"){
		
	this->update_type(L_TYPE);
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
	
	if( (int)(this->waveform_bak) != (int)(this->params[0]) )
		this->update_type( static_cast<LFO_Wave>((int)(this->params[0])) );
	
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

int LFO::bypass(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	memset(out, 0.0, sizeof(sample_t) * nframes);
	return 0;
}

void LFO::update_type(LFO_Wave type){
	
	this->params[0] = type;
	this->waveform_bak = type;
	switch(type){
		case WAVE_SIN:
			this->waveform = w_sin;
			break;
		case WAVE_SQR:
			this->waveform = w_sqr;
			break;
		case WAVE_TRI:
			this->waveform = w_tri;
			break;
		case WAVE_SAW:
			this->waveform = w_saw;
			break;
		case WAVE_VAR:
			this->waveform = w_var;
			break;
		case WAVE_NPH:
			this->waveform = w_nph;
			break;
		case WAVE_WHI:
			this->waveform = w_whi;
			break;
		default:
			this->waveform = w_sin;
			break;
	}
}

int LFO::set_param(int param, float var){
	
	if(param == 0){
		
		this->update_type( static_cast<LFO_Wave>(var) );
		this->params[0];
		return 0;
	}else{
		
		return this->Module::set_param(param, var);
	}
}

int LFO::set_param_list(int size, float *params){
	
	if(!this->Module::set_param_list(size, params)){
		
		this->update_type( static_cast<LFO_Wave>((int)this->params[0]) );
		return 0;
	}
	return 1;
}

inline sample_t w_sin(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign*sin(M_PI * 2.0 * in));
}

inline sample_t w_sqr(float in, float sign, float p1, float p2){
	
	return (sample_t) ((in-0.5 < 0)? sign * (-1.0) : sign*1.0);
}

inline sample_t w_tri(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign * ( 2.0*spi_abs(2.0*in -1.0) - 1.0));
}

inline sample_t w_saw(float in, float sign, float p1, float p2){
	
	return (sample_t) (sign * (2.0*in -1.0));
}

sample_t w_var(float in, float sign, float p1, float p2){
	
	if(in < p1){
		return (sample_t)(sign * (2.0*((p2*in)/p1) - 1.0));
	}else{
		return (sample_t)(sign * (1.0 - 2.0*p2*(in-p1)/(1-p1)));
	}
}

inline sample_t w_nph(float in, float sign, float p1, float p2){
	
	return (sample_t)(sign * sin(M_PI * 2.0 * (in + (((float)((int)(in*(p1+1))))/p2))));
}

inline sample_t w_whi(float in, float sign, float p1, float p2){
	
	return ((float)(((RandSeed = RandSeed * 214013L + 2531011L) >> 16) & 0x7fff)/RAND_MAX) * 2.0f - 1.0f;
}