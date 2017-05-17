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
*	Classe de base pour tout les effets
*/
class Fx_Unit : public Fx_Client{
	
	public:
	
		Fx_Unit(const char *server, const char *unit_name);
		
		virtual int process(jack_nframes_t nframes, void *arg){};
		
	protected:
		jack_port_t **a_port;
};

class Drive : public Fx_Unit{
	
	public:
		
		Drive(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
	
	protected :
		
		float gp, gn;
		float sp, sn;
		float shp, shn;
		sfx_tripole *filter;
		
		int is_abs;
		int is_asm;
		int is_soft_clip_p;
		int is_soft_clip_n;
};

class Fx_Mod : public 
#endif