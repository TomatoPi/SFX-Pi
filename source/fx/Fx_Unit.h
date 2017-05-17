#ifndef DEF_FX_UNIT
#define DEF_FX_UNIT

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "../utility/utility.h"

/*
*	Basic class for all effects
*/
class Fx_Unit : public Fx_Client{
	
	public:
	
		Fx_Unit(const char *server, const char *unit_name);
		
		virtual int process(jack_nframes_t nframes, void *arg){};
		
	protected:
		jack_port_t **a_port; // Client's IO port
};

/*
*
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Fx_Unit{
	
	public:
		
		Drive(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
	
	protected :
		
		float gp, gn; //Gain applied to the signal, positive or negative
		float sp, sn; //Soft-Clipping Softness, positive or negative
		float shp, shn; //Soft-Clipping Shape, positive or negative
		sfx_tripole *filter; // EQ ( bass, mid, high )
		
		int is_abs; //for full-wave rectification
		int is_asm; //for asymmetrical clipping : _p parameter for positives half-cycles and _n for negatives half-cycles
		int is_soft_clip_p, is_soft_clip_n; //for Soft or Hard-clipping
};

/*class Fx_Mod : public Fx_Client{
	
};*/
#endif
