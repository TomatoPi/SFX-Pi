#ifndef DEF_UTILITY
#define DEF_UTILITY

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
#include <jack/jack.h>
/*
*	Simplification de l'ecriture du type jack_default_audio_sample_t
*/
typedef jack_default_audio_sample_t sample_t;

int fx_Process_Callback(jack_nframes_t nframes, void *u);

class Fx_Client{
	
	public:
	
		Fx_Client(const char *server, const char *name);
		
		virtual int process(jack_nframes_t nframes, void *arg){};
		
	protected:
	
		jack_client_t *client;
		char* name;
};

/*
*	Fonction qui contraint un sample "in" dans un interval donné [min, max]
*/
sample_t sfx_clip(sample_t in, float min, float max);
sample_t sfx_soft(sample_t in, float max, float soft, float shape);
sample_t sfx_abs(sample_t in);

float sfx_dbtorms(float d);
float sfx_rmstodb(float g);

static float vsa = (1.0 / 4294967295.0); //Très petite valeur

/*
*	Filtre trois bandes, low, mid, high
*/
typedef struct
{

	//Filtre passe bas
	float fl; 	// Frequence
	float f1p0;// Poles
	float f1p1; 
	float f1p2;
	float f1p3;

	// Filtre pass haut
	float fh; // Frequence
	float f2p0; // Poles
	float f2p1;
	float f2p2;
	float f2p3;

	// Sauvegarde des sample precedents

	float sm1; // Sample - 1
	float sm2; // - 2
	float sm3; // - 3

	// Gain des bandes

	float gl; // low
	float gm; // mid
	float gh; // high

} sfx_tripole; 

void sfx_init_tripole(sfx_tripole *f, int fl, int fh, int sr, float gl, float gm, float gh);
sample_t sfx_do_tripole(sfx_tripole* f, sample_t sample);

#endif