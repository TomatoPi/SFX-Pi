#ifndef DEF_TONESTACK_H
#define DEF_TONESTACK_H

#include "../core/Filter.h"
#include "../core/Modules.h"

class Tonestack : public Module{

	public:
	
		Tonestack(const char *server);
		~Tonestack();
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		int set_param(int param, float var);
		int set_param_list(int size, float *params);
		
	protected:
		
		Filter_3EQ *filter_L;
		Filter_3EQ *filter_R; // EQ ( bass, mid, high )
		
		/*
		*	float freq_low
		* 	float freq_high
		*	float gain_low
		*	float gain_mid
		*	float gain_high
		*/

};

#endif
