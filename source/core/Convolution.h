#ifndef DEF_CONVOLUTION_H
#define DEF_CONVOLUTION_H

#include "Ringbuffer.h"
#include "Utility.h"

sample_t do_convolution(sample_t *X, sample_t *H, int M);

sample_t do_convolution(const Ringbuffer *buff,const rng_reader reader,const sample_t *H,const int M);

#endif
