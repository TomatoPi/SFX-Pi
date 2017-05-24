#include "Ringbuffer.h"

Ringbuffer::Ringbuffer(int length, int samplerate){

	this->samplerate = samplerate;
	this->buffer_size = (int)( (float)(length * samplerate) / 1000.0 );
	
	this->buffer = malloc(this->buffer_size * sizeof(float));
	if(this->buffer == NULL){
		exit(0);
	}
	memset( this->buffer, 0.0, this->buffer_size * sizeof(float) );
	
	this->write_head = 0;
}

Ringbuffer::int read_block(float *block, int delay, int length){

}

Ringbuffer::int write_block(float *block, int length){

}
