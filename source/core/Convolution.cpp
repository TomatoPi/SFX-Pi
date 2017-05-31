#include "Convolution.h"
	
sample_t do_convolution(sample_t *X, sample_t *H, M){
	
	sample_t yn = 0.0;
	for(int k = 0; k < M; k++){

		yn += X[-k]*H[k];
	}
	return yn;
}
