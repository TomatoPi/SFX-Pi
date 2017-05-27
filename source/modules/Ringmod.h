#ifndef DEF_RINGMOD_H
#define DEF_RINGMOD_H

#include <jack/jack.h>
#include "../core/Modules.h"
#include "../core/Utility.h"

#define R_DEPTH 1.0

class Ringmod : public Module{

	public:
	
		Ringmod(const char *server);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
	private :
		/*
		*	RingMod / Tremolo unique parameters are :
		*	
		*	float : depth
		*	dry wet
		*/
};

#endif
