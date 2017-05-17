#ifndef DEF_UTILITY
#define DEF_UTILITY



static float vsa = (1.0 / 4294967295.0); //Très petite valeur

/*
*	Filtre trois bandes, low, mid, high
*/
typedef struct
{

	//Filtre passe bas
	float fl; 	// Frequence du filtre
	float f1p0;// Poles
	float f1p1; 
	float f1p2;
	float f1p3;

	// Filtre pass haut
	float fh; // Frequence du filtre
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

/*
*	Initialisation d'un EQ trois poles
*	f : EQ
*	fl : frequence basse
* 	fh : frequence haute
*	sr : current samplerate
*	gl : gain bande basse
*	gm : gain bande medium
*	gh : gain bande haute
*/
void sfx_init_tripole(sfx_tripole *f, int fl, int fh, int sr, float gl, float gm, float gh);

/*
*	Egalisation du sample donné
*/
sample_t sfx_do_tripole(sfx_tripole* f, sample_t sample);

#endif
