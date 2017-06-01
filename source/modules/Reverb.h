#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Ringbuffer.h"

class Reverb : public Module{
	
	public:
		
		Reverb(const char *server);
		~Reverb();
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
		
		
};

#endif
