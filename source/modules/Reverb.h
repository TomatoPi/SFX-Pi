#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Ringbuffer.h"
#include "../core/Convolution.h"

#define R_PREDELAY 20
#define R_DECAYMAX 20000
#define R_DECAY 1000

class Reverb : public Module{
	
	public:
		
		Reverb(const char *server);
		~Reverb();
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
		
		Ringbuffer *buffer_L, *buffer_R;
	
		sample_t* reverb_reponse;
	
		/*
		*	float samplerate
		*	int reverb_reponse_size
		*
		*	float predelay
		*	float decay
		*
		*	float dry_wet
		*/
};

#endif
