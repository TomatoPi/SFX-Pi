#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Ringbuffer.h"
#include "../core/Convolution.h"

#define R_TAP 1
#define R_SEP 5.0f
#define R_DECAY 1000.0f
#define R_PREDELAY 20.0f
#define R_CUT 1500.0f
#define R_DECAYMAX 20000.0f

class Reverb : public Module{
	
	public:
		
		Reverb(const char *server);
		~Reverb();
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
		
		Ringbuffer *buffer_L, *buffer_R;
		rng_reader *reader_L, *reader_R;
	
		spi_tripole *filter_L, *filter_R;
	
		/*
		*	float samplerate
		*
		*	int tap_number;
		*	float tap_separation;
		*
		*	float predelay
		*	float decay
		*
		*	float feedforward
		*
		*	float frequency cutoff
		*
		*	float dry_wet
		*/
};

#endif
