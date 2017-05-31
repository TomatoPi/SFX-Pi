#ifndef DEF_CONVOLUTION_H
#define DEF_CONVOLUTION_H

#include "Ringbuffer.h"

class Convolution{

	public:
		
		Convolution();
		~Convolution();
		
		sample_t do_convolution(sample_t *X, int N, sample_t *H, M);
		
	protected:
		
		
};

#endif
