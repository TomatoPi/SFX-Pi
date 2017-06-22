#ifndef DEF_TONESTACK_H
#define DEF_TONESTACK_H

#include "../core/Filter.h"
#include "../core/Modules.h"

#define TONE_PARAMS 5

#define TONE_FLOW 	0
#define TONE_FHIGH 	1
#define TONE_GL		2
#define TONE_GM		3
#define TONE_GH		4

static const char* tone_param_names[] = {"Lowcut", "Highcut", "Lowgain", "Midgain", "Highgain"};
static const float tone_default_params[] = {200, 1200, 0.75f, 3.0f, 7.0f};

class Tonestack_voice : public Module_voice{
	
	public :
	
		Tonestack_voice(jack_client_t *client, int idx);
		~Tonestack_voice();
		
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
		
		Filter_3EQ* get_filter() const;
		
	protected :
	
		Filter_3EQ *filter;
};

class Tonestack : public Module{

	public:
	
		Tonestack(const char *server, int vc);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
		
	protected:
		
		/*
		*	float freq_low
		* 	float freq_high
		*	float gain_low
		*	float gain_mid
		*	float gain_high
		*/

};

#endif
