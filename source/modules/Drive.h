#ifndef DEF_DRIVE_H
#define DEF_DRIVE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "../core/Filter.h"
#include "../core/Modules.h"

/*
*	Default values for drive module
*/
#define D_GAIN 50.0
#define D_SOFT 2.0
#define D_SHAPE 0.5
#define D_TYPE 1.0
#define D_ABS 0.0
#define D_ASM 0.0

/*
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:	
		
		Drive(const char *server);
		~Drive();
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		int set_param(int param, float var);
		int set_param_list(int size, float *params);
	
	protected :
			
		Filter_3EQ *filter_L;
		Filter_3EQ *filter_R; // EQ ( bass, mid, high )
	
		/*
		* Specific parameters for drive module are, in order :	unique parameters -> positives parameters -> negatives parameters
		*
		*	int is_abs; //for full-wave rectification
		*	int is_asm; //for asymmetrical clipping : _p parameter for positives half-cycles and _n for negatives half-cycles
		*
		*	float gp, gn; //Gain applied to the signal
		*	int is_soft_clip_p, is_soft_clip_n; //for Soft or Hard-clipping
		*	float sp, sn; //Soft-Clipping Softness
		*	float shp, shn; //Soft-Clipping Shape
		*
		*	float freq_low
		* 	float freq_high
		*	float gain_low
		*	float gain_mid
		*	float gain_high
		*/
};

#endif
