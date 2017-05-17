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
		jack_port_t **a_port; //io du client
};

/*
* 	FX de drive, Soft Clip, Hard clip
*	Filtrage trois bande avant le clip
*	Possibilité de clipping asymetrique
*/
class Drive : public Fx_Unit{
	
	public:
		
		Drive(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
	
	protected :
		
		float gp, gn; //Gain appliqué au signal, positif - negatif
		float sp, sn; // Softness du soft clip, positif - negatif
		float shp, shn; //Shape -----------------------------
		sfx_tripole *filter; // Filtre trois bandes ( bass, mid, high )
		
		int is_abs; //Si redressage double alternace du signal
		int is_asm; //Si clipping asymetrique : paramètres _p utilisés pour les alternaces positives, _n pour les alternaces negatives
		int is_soft_clip_p, is_soft_clip_n; //si le clipping est soft ou hard
};

/*class Fx_Mod : public Fx_Client{
	
};*/
#endif
