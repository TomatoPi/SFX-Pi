#include "LFO.h"
/*
*	---------------------------------------------------------------------------
*	LFO voice
*	---------------------------------------------------------------------------
*/
LFO_voice::LFO_voice(jack_client_t *client, int idx):Module_voice(client, LFO_PARAMS, 0, 1, 0, 0){
	
	this->samplerate_ = jack_get_sample_rate(client);
	this->set_param_list(LFO_PARAMS, (float*)lfo_default_params);
	
	char n[10];
	sprintf(n, "Out_%d", idx);
	register_port(client, this->port_audio_out_, PORT_AO, 1, n);
	
	this->is_ready_ = 1;
}


void LFO_voice::set_param(int param, float var){

	if(param == LFO_TYPE){
		
		this->update_type( static_cast<LFO_wave>(var) );
		this->param_[LFO_TYPE];
	}else{
		
		this->Module_voice::set_param(param, var);
	}	
}

void LFO_voice::set_param_list(int size, float *pl){
	
	if(size == this->param_count_){
		this->Module_voice::set_param_list(size, pl);
		this->update_type( static_cast<LFO_wave>((int)this->param_[LFO_TYPE]) );
	}
}

int LFO_voice::get_sr() const{
	
	return this->samplerate_;
}

void LFO_voice::update_type(LFO_wave type){
	
	this->param_[LFO_TYPE] = type;
	switch(type){
		case WAVE_SIN:
			this->waveform_ = w_sin;
			break;
		case WAVE_SQR:
			this->waveform_ = w_sqr;
			break;
		case WAVE_TRI:
			this->waveform_ = w_tri;
			break;
		case WAVE_SAW:
			this->waveform_ = w_saw;
			break;
		case WAVE_VAR:
			this->waveform_ = w_var;
			break;
		case WAVE_NPH:
			this->waveform_ = w_nph;
			break;
		case WAVE_WHI:
			this->waveform_ = w_whi;
			break;
		default:
			this->waveform_ = w_sin;
			break;
	}
}

/*
*	---------------------------------------------------------------------------
*	LFO Stuff
*	---------------------------------------------------------------------------
*/
LFO::LFO(const char *server, int vc): Module(server, MLFO, vc){
	
	for(int i = 0; i < vc; i++){
		
		this->voice_.push_back(new LFO_voice(this->client_, i));
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
}

int LFO::process(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			
			sample_t *out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
			
			float f 	= (*itr)->get_param(LFO_FREQ); 	//LFO frequency
			float sr 	= ((LFO_voice*)*itr)->get_sr();	//Client Samplerate
				
			float ramp 	= (*itr)->get_param(LFO_RAMP);				//Current LFO value
			float phase = (*itr)->get_param(LFO_PHASE);				//LFO Phase
			int s 		= ((*itr)->get_param(LFO_SIGN) < 0)? -1: 1;	//LFO sign
				
			float p1 = (*itr)->get_param(LFO_VAR1);		//waveshape param 1
			float p2 = (*itr)->get_param(LFO_VAR2);		//waveshape param 2
			
			for(jack_nframes_t i = 0; i < nframes; i++){
				
				ramp += f/sr;
				ramp = fmod(ramp, 1.0);	
				
				out[i] = (*(((LFO_voice*)*itr)->waveform_))(ramp, s, p1, p2);
			}
			
			(*itr)->set_param(LFO_RAMP, ramp);
		}
	}
	return 0;
}

void LFO::add_voice(){
	
	int idx = this->voice_.size();
	this->voice_.push_back(new LFO_voice(this->client_, idx));
	this->sync();
}

void LFO::sync(){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			(*itr)->set_param(LFO_RAMP, 0.0f);
		}
	}
}

int LFO::bypass(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			sample_t *out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
			memset(out, 0.0f, sizeof(sample_t) * nframes);
		}
	}
	return 0;
}

const char* LFO::get_param_name(int p) const{
	
	if(p < this->get_voice(0)->get_param_count())
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