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

float spi_dbtorms(float d){
	
	return pow(10, d/20.0);
}

float spi_rmstodb(float g){
	
	return 20 * log10(g);
}

void spi_init_tripole(spi_tripole *f, int fl, int fh, int sr, float gl, float gm, float gh){
	
	//initialisating values to 0
	memset(f,0,sizeof(spi_tripole));
	
	//set gains
	f->gl = gl;
	f->gm = gm;
	f->gh = gh;

	//Scalling filters frequencies with samplerate
	f->fl = 2 * sin(M_PI * ((float)fl / (float)sr)); 
	f->fh = 2 * sin(M_PI * ((float)fh / (float)sr));
}

sample_t spi_do_tripole(spi_tripole* f, sample_t sample){
	
	sample_t l,m,h; // Low / Mid / High

	// Compute lowpass if gain non null
	if(f->gl != 0.0 || f->gm != 0){
		f->f1p0 += (f->fl * (sample - f->f1p0)) + vsa;
		f->f1p1 += (f->fl * (f->f1p0 - f->f1p1));
		f->f1p2 += (f->fl * (f->f1p1 - f->f1p2));
		f->f1p3 += (f->fl * (f->f1p2 - f->f1p3));

		l = f->f1p3;
	}else{
		l = 0.0;
	}
	
	// Compute highpass if gain non null
	if(f->gh != 0.0 || f->gm != 0){
		f->f2p0 += (f->fh * (sample - f->f2p0)) + vsa;
		f->f2p1 += (f->fh * (f->f2p0 - f->f2p1));
		f->f2p2 += (f->fh * (f->f2p1 - f->f2p2));
		f->f2p3 += (f->fh * (f->f2p2 - f->f2p3));

		h = f->sm3 - f->f2p3;
	}else{
		h = 0.0;
	}

	// Compute bandpass ( sample - (low + high))
	m = f->sm3 - (h + l);

	// Compute gains
	l *= f->gl;
	m *= f->gm;
	h *= f->gh;

	// Suffle buffer
	f->sm3 = f->sm2;
	f->sm2 = f->sm1;
	f->sm1 = sample; 

	return(l + m + h);
}
