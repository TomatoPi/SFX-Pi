#ifndef DEF_CONVOLUTION_H
#define DEF_CONVOLUTION_H

#include "Ringbuffer.h"
#include "Utility.h"

sample_t do_convolution(sample_t *X, sample_t *H, int M);

sample_t do_convolution(Ringbuffer *buff, int offset, sample_t *H, int M, int m);

#endif
