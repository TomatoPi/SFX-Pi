#ifndef DEF_UTILITY_H
#define DEF_UTILITY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
#include <jack/jack.h>

/*
* Simplify jack_default_audio_sample_t writing
*/
typedef jack_default_audio_sample_t sample_t;

/*
*	Constrain given sample between min and max, ( Hard-Clipping )
*/
sample_t sfx_clip(sample_t in, float min, float max);

/*
*	Compute dual tanh soft clipping, see wiki for more information
*/
sample_t sfx_soft(sample_t in, float max, float soft, float shape);

/*
* Compute absolute value of given sample
*/
sample_t sfx_abs(sample_t in);

/*
* Convert db to gain ( ex : +20dB ==> *100 )
*/
float sfx_dbtorms(float d);

/*
* Convert gain to db ( ex : *0.5 ==> -3dB )
*/
float sfx_rmstodb(float g); 

#endif
