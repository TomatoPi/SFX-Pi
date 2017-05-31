#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include "Delay.h"

class Reverb : public Delay{
	
	public:
		
		Reverb(const char *server);
		~Reverb();
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
		
		
};

#endif
