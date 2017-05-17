#ifndef DEF_DRIVE_H
#define DEF_DRIVE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "../core/Utility.h"
#include "../core/Modules.h"

/*
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:
		
		Drive(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
	
	protected :
		
		float gp, gn; //Gain applied to the signal
		float sp, sn; //Soft-Clipping Softness
		float shp, shn; //Soft-Clipping Shape
		sfx_tripole *filter; // EQ ( bass, mid, high )
		
		int is_abs; //for full-wave rectification
		int is_asm; //for asymmetrical clipping : _p parameter for positives half-cycles and _n for negatives half-cycles
		int is_soft_clip_p, is_soft_clip_n; //for Soft or Hard-clipping
};

#endif
