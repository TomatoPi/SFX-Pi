#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include <jack/jack.h>

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Filter.h"

#define R_DAMP 0.4f
#define R_ROOM 0.5f
#define R_ALLF 0.5f
#define R_GAIN 0.015f
#define R_DRY 0.5f
#define R_WIDTH 1.0f

#define REVERB_PARAMS_COUNT 4

#define REVERB_GAIN 0
#define REVERB_DRY 1
#define REVERB_WET1 2
#define REVERB_WET2 3

#define COMBCOUNT 8
#define ALLPCOUNT 4
#define STEREOSPREAD 23

#define scalewet 3.0f
#define scaledry 2.0f
static const int combsize[COMBCOUNT] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
static const int allpsize[ALLPCOUNT] = {556, 441, 341, 225};

class Reverb : public Module{
	
	public:
		
		Reverb(const char *server);
		~Reverb();
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
		
		Filter_Comb *comb_L[COMBCOUNT], *comb_R[COMBCOUNT];
		Filter_Allpass *allp_L[ALLPCOUNT], *allp_R[ALLPCOUNT];
	
		/*
		*	gain
		*
		*	dry
		*	wet1
		*	wet2
		*/
};

#endif
