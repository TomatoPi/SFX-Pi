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

/**
* Simplify jack_default_audio_sample_t writing
*/
typedef jack_default_audio_sample_t sample_t;

/**
*	Constrain given sample between min and max, ( Hard-Clipping )
*/
inline sample_t spi_clip(sample_t in, float min, float max){
	
	return (in > max)? max : ((in < min)?min : in);
}

/**
*	Compute dual tanh soft clipping, see wiki for more information
*/
inline sample_t spi_soft(sample_t in, float max, float soft, float shape){
	
	return max * ((1 - shape)*tanh(in) + shape * tanh(in/soft));
}

/**
* Compute absolute value of given sample
*/
inline sample_t spi_abs(sample_t in){
	
	return (in < 0)?-in:in;
}

/**
* Mix between dry and wet signal with a : amout of wet signal
*/
inline sample_t spi_dry_wet(sample_t dry, sample_t wet, float a){
	
	return ( (1-(a*a)) * dry) + (a * a * wet);
}

/**
* Convert db to gain ( ex : +20dB ==> *100 )
*/
inline float spi_dbtorms(float d){
	
	return pow(10, d/20.0f);
}

/**
* Convert gain to db ( ex : *0.5 ==> -3dB )
*/
inline float spi_rmstodb(float g){
	
	return 20.0f * log10(g);
}


/**
* Convert a length in ms to sample
*/
inline int spi_mstos(float ms, int sr){
	
	return (int)( (ms*sr) / 1000.0f );
}

/**
* Convert a length in sample to ms
*/
inline int spi_stoms(float sample, int sr){
	
	return (int)( (sample * 1000.0f) / (float)(sr) );
}

/**
* Return random float between min and max
*/
inline float spi_frand(float min, float max){
	
	if(max < min){
	
		float a = min;
		min = max;
		max = a;
	}
	
	return ((float)rand()/(float)RAND_MAX) * (max - min) + min;
}

/**
* Convert given float in a string of 4 char length
* @param f number to convert
* @return number converted in a string 
*/
string f_ftos(float f);

#endif
