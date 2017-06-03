#include "Convolution.h"
	
sample_t do_convolution(sample_t *X, sample_t *H, int M){
	
	sample_t yn = 0.0;
	for(int k = 0; k < M; k++){

		yn += X[-k]*H[k];
	}
	return yn;
}

sample_t do_convolution(const Ringbuffer *buff,const rng_reader reader,const sample_t *H,const int M){
	
	rng_reader r = {reader.index + 1, 0.0f, reader.delay};
	rng_reader r2 = {0.0f, 0.0f, 0};

	sample_t yn = 0.0;		

	buff->reverse_read_value(&r);
	yn += r.value;
	
	//buff->reverse_read_value(&r2);
	
	for(int k = 1; k < M; k++){
			
		//yn += r.value * 0.0f;
	}
	return yn;
}
