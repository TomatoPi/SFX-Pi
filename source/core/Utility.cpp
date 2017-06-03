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

void spi_init_tripole(spi_tripole *f){
	
	//initialisating values to 0
	memset(f,0,sizeof(spi_tripole));
	
/*	//set gains
*	f->gl = gl;
*	f->gm = gm;
*	f->gh = gh;
*/
	
}

void spi_init_tripole_freq(spi_tripole *f, float fl, float fh, int sr){
	
	//Scalling filters frequencies with samplerate
	f->fl = 2.0f * sin(M_PI * (fl / (float)sr)); 
	f->fh = 2.0f * sin(M_PI * (fh / (float)sr));
	
	f->fl_bak = fl;
	f->fh_bak = fh;
}

sample_t spi_do_tripole(spi_tripole* f, sample_t sample, float gl, float gm, float gh){
	
	sample_t l,m,h; // Low / Mid / High

	// Compute lowpass if gain non null
	if(gl != 0.0f || gm != 0.0f){
		f->f1p0 += (f->fl * (sample - f->f1p0)) + vsa;
		f->f1p1 += (f->fl * (f->f1p0 - f->f1p1));
		f->f1p2 += (f->fl * (f->f1p1 - f->f1p2));
		f->f1p3 += (f->fl * (f->f1p2 - f->f1p3));

		l = f->f1p3;
	}else{
		l = 0.0f;
	}
	
	// Compute highpass if gain non null
	if(gh != 0.0f || gm != 0.0f){
		f->f2p0 += (f->fh * (sample - f->f2p0)) + vsa;
		f->f2p1 += (f->fh * (f->f2p0 - f->f2p1));
		f->f2p2 += (f->fh * (f->f2p1 - f->f2p2));
		f->f2p3 += (f->fh * (f->f2p2 - f->f2p3));

		h = f->sm3 - f->f2p3;
	}else{
		h = 0.0f;
	}

	// Compute bandpass ( sample - (low + high))
	m = f->sm3 - (h + l);

	// Compute gains
	l *= gl;
	m *= gm;
	h *= gh;

	// Suffle buffer
	f->sm3 = f->sm2;
	f->sm2 = f->sm1;
	f->sm1 = sample; 

	return(l + m + h);
}

void spi_init_onepole(spi_onepole *f){
	
	memset(f, 0, sizeof(spi_onepole));
}

void spi_init_onepole_freq(spi_onepole *f, float ff, int sr){
	
	f->f = 2.0f * sin(M_PI * (ff / (float)sr));
	f->f_bak = ff;
}

sample_t spi_do_lowpass(spi_onepole* f, sample_t sample){
	
	f->fp0 += (f->f * (sample - f->fp0)) + vsa;
	f->fp1 += (f->f * (f->fp0 - f->fp1));
	f->fp2 += (f->f * (f->fp1 - f->fp2));
	f->fp3 += (f->f * (f->fp2 - f->fp3));
	
	return (sample_t)f->fp3;
}

sample_t spi_do_highpass(spi_onepole* f, sample_t sample){
	
	f->fp0 += (f->f * (sample - f->fp0)) + vsa;
	f->fp1 += (f->f * (f->fp0 - f->fp1));
	f->fp2 += (f->f * (f->fp1 - f->fp2));
	f->fp3 += (f->f * (f->fp2 - f->fp3));

	sample_t h = f->sm3 - f->fp3;
	
	f->sm3 = f->sm2;
	f->sm2 = f->sm1;
	f->sm1 = sample;
	
	return h;
}