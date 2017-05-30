#ifndef DEF_UTILITY_H
#define DEF_UTILITY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
#include <jack/jack.h>

//Very small amount
static float vsa = (1.0 / 4294967295.0);

/*
* Simplify jack_default_audio_sample_t writing
*/
typedef jack_default_audio_sample_t sample_t;

/*
*	Constrain given sample between min and max, ( Hard-Clipping )
*/
sample_t spi_clip(sample_t in, float min, float max);

/*
*	Compute dual tanh soft clipping, see wiki for more information
*/
sample_t spi_soft(sample_t in, float max, float soft, float shape);

/*
* Compute absolute value of given sample
*/
sample_t spi_abs(sample_t in);

/*
* Mix between dry and wet signal with a : amout of wet signal
*/
sample_t spi_dry_wet(sample_t dry, sample_t wet, float a);

/*
* Convert db to gain ( ex : +20dB ==> *100 )
*/
float spi_dbtorms(float d);

/*
* Convert gain to db ( ex : *0.5 ==> -3dB )
*/
float spi_rmstodb(float g); 


/*
* Convert a length in ms to sample
*/
int spi_mstos(int ms, int sr);

/*
* Convert a length in sample to ms
*/
int spi_stoms(int sample, int sr);

/*
*	3 Bands EQ ( low, mid, high )
*/
typedef struct
{

	//Lowpass filter
	float fl; 	//Filter frequency
	float f1p0; //Filter pole
	float f1p1; 
	float f1p2;
	float f1p3;

	//Highpass filter
	float fh;   //Filter frequency
	float f2p0; //Filter pole
	float f2p1;
	float f2p2;
	float f2p3;

	//Save last samples
	float sm1; // Sample - 1
	float sm2; // - 2
	float sm3; // - 3

/*  	//Band gain
*	float gl; // low
*	float gm; // mid
*	float gh; // high
*/

} spi_tripole;

/*
*	Initialisating 3Bands EQ
*	f : EQ
*	fl : low freq
* 	fh : high freq
*	sr : current samplerate
*	gl : low band gain
*	gm : mid band gain
*	gh : high band gain
*/
void spi_init_tripole(spi_tripole *f);

void spi_init_tripole_freq(spi_tripole *f, int fl, int fh, int sr);

/*
*	Compute 3Bands EQ for given sample
*/
sample_t spi_do_tripole(spi_tripole* f, sample_t sample, float gl, float gm, float gh);

#endif
