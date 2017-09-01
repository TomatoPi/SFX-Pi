#ifndef DEF_UTILITY_H
#define DEF_UTILITY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <math.h>
#include <jack/jack.h>

using namespace std;

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
* Return random float between min and max
*/
float spi_frand(float min, float max);

/**
* Convert given float in a string of 4 char length
* @param f number to convert
* @return number converted in a string 
*/
string f_ftos(float f);

#endif