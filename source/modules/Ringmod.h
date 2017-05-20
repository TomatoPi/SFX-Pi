#ifndef DEF_RINGMOD_H
#define DEF_RINGMOD_H

#include <jack/jack.h>
#include "../core/Modules.h"

class Ringmod : public Module{

	public:
	
		Ringmod(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
	private :
		/*
		*
		*/
};

#endif
