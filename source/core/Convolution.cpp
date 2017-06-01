#include "Convolution.h"
	
sample_t do_convolution(sample_t *X, sample_t *H, int M){
	
	sample_t yn = 0.0;
	for(int k = 0; k < M; k++){

		yn += X[-k]*H[k];
	}
	return yn;
}

sample_t do_convolution(Ringbuffer *buff, int offset, sample_t *H, int M, int m){

	sample_t yn = 0.0;
	if(m > M) return 0.0;
	rng_reader reader = buff->new_read_head(offset);
	for(int k = 0; k < m; k++){
		
		buff->reverse_read_value(&reader);
		yn += reader.value * H[(int)(((float)k/(float)m)*(float)M)];
	}
	return yn;
}
