#include "Utility.h"

sample_t spi_clip(sample_t in, float min, float max){
	
	return (in > max)? max : ((in < min)?min : in);
}

sample_t spi_soft(sample_t in, float max, float soft, float shape){
	
	return max * ((1 - shape)*tanh(in) + shape * tanh(in/soft));
}

sample_t spi_abs(sample_t in){
	
	return (in < 0)?-in:in;
}

sample_t spi_dry_wet(sample_t dry, sample_t wet, float a){
	
	return ( (1-(a*a)) * dry) + (a * a * wet);
}

float spi_dbtorms(float d){
	
	return pow(10, d/20.0f);
}

float spi_rmstodb(float g){
	
	return 20.0f * log10(g);
}

int spi_mstos(int ms, int sr){
	
	return (int)( (float)(ms*sr) / 1000.0f );
}

int spi_stoms(int sample, int sr){
	
	return (int)( (float)(sample * 1000.0f) / (float)(sr) );
}

float spi_frand(float min, float max){
	
	if(max < min){
	
		float a = min;
		min = max;
		max = a;
	}
	
	return ((float)rand()/(float)RAND_MAX) * (max - min) + min;
}