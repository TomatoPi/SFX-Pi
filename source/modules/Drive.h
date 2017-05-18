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
*	Default values for drive module
*/
#define D_GAIN 50.0
#define D_SOFT 2.0
#define D_SHAPE 0.5
#define D_TYPE 1
#define D_ABS 0
#define D_ASM 0

/*
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:
		
		Drive(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
	
		/*
		* set given param , for gain, soft, shape and type : b true for positive param, false for negative
		*/
		void setParams_gain(float p, int b);
		void setParams_soft(float p, int b);
		void setParams_shap(float p, int b);
		void setParams_type(int p, int b);
		void setParams_filt(spi_tripole *f);
		void setParams_abs(int b);
		void setParams_asm(int b);
	
		spi_tripole *get_filter();
	
	protected :
			
		spi_tripole *filter; // EQ ( bass, mid, high )
	
		float gp, gn; //Gain applied to the signal
		float sp, sn; //Soft-Clipping Softness
		float shp, shn; //Soft-Clipping Shape
		int is_soft_clip_p, is_soft_clip_n; //for Soft or Hard-clipping
		
		int is_abs; //for full-wave rectification
		int is_asm; //for asymmetrical clipping : _p parameter for positives half-cycles and _n for negatives half-cycles
};

#endif
