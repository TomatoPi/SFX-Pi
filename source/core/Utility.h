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
sample_t f_clip(sample_t in, float min, float max);

/*
*	Compute dual tanh soft clipping, see wiki for more information
*/
sample_t f_soft(sample_t in, float max, float soft, float shape);

/*
* Compute absolute value of given sample
*/
sample_t f_abs(sample_t in);

/*
* Convert db to gain ( ex : +20dB ==> *100 )
*/
float f_dbtorms(float d);

/*
* Convert gain to db ( ex : *0.5 ==> -3dB )
*/
float f_rmstodb(float g); 

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

  	//Band gain
	float gl; // low
	float gm; // mid
	float gh; // high

} f_tripole;

#endif
