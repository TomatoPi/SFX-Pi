#include "LFO.h"

LFO::LFO(const char *server): Module(server, MLFO, LFO_PARAMS_COUNT, 0, 1, 0, 0, "out"){
		
	this->update_type(L_TYPE);
	this->params[LFO_FREQ] = L_FREQ;
	this->params[LFO_SAMPLERATE] = jack_get_sample_rate(this->client);
	
	this->params[LFO_RAMP] = L_RAMP;
	this->params[LFO_PHASE] = L_PHASE;
	this->params[LFO_SIGN] = L_SIGN;
	
	this->params[LFO_VAR1] = L_PAR1;
	this->params[LFO_VAR2] = L_PAR2;
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

int LFO::process(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	
	float f = this->params[LFO_FREQ]; 		//LFO frequency
	float sr = this->params[LFO_SAMPLERATE];		//Client Samplerate
		
	float ramp = this->params[LFO_RAMP];		//Current LFO value
	float phase = this->params[LFO_PHASE];		//LFO Phase
	int s = (this->params[LFO_SIGN] < 0)? -1: 1;	//LFO sign
		
	float p1 = this->params[LFO_VAR1];		//waveshape param 1
	float p2 = this->params[LFO_VAR2];		//waveshape param 2
	
	for(jack_nframes_t i = 0; i < nframes; i++){
		
		ramp += f/sr;
		ramp = fmod(ramp, 1.0);	
		
		out[i] = (*(this->waveform))(ramp, s, p1, p2);
	}
	
	this->params[LFO_RAMP] = ramp;
	return 0;
}

int LFO::bypass(jack_nframes_t nframes, void *arg){
	
	sample_t *out = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	memset(out, 0.0, sizeof(sample_t) * nframes);
	return 0;
}

void LFO::update_type(LFO_Wave type){
	
	this->params[LFO_TYPE] = type;
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
	
	if(param == LFO_TYPE){
		
		this->update_type( static_cast<LFO_Wave>(var) );
		this->params[LFO_TYPE];
		return 0;
	}else{
		
		return this->Module::set_param(param, var);
	}
}

int LFO::set_param_list(int size, float *params){
	
	if(!this->Module::set_param_list(size, params)){
		
		this->update_type( static_cast<LFO_Wave>((int)this->params[LFO_TYPE]) );
		return 0;
	}
	return 1;
}

const char* LFO::get_param_name(int p) const{
	
	if(p < this->params_count)
		return lfo_param_names[p];
	return "NULL";
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