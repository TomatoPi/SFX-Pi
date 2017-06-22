#ifndef DEF_RINGMOD_H
#define DEF_RINGMOD_H

#include <jack/jack.h>
#include "../core/Modules.h"
#include "../core/Utility.h"

#define RINGMOD_PARAMS 1

#define RING_DEPTH 0

static const char* ringmod_param_names[] = {"Depth"};
static const float ringmod_default_params[] = {1.0f};

class Ringmod_voice : public Module_voice{
	
	public :
		
		Ringmod_voice(jack_client_t *client, int idx);
		
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
};

class Ringmod : public Module{

	public:
	
		Ringmod(const char *server, int vc);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
		
	private :
		/*
		*	RingMod / Tremolo unique parameters are :
		*	
		*	float : depth
		*/
};

#endif
