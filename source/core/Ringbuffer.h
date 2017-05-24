#ifndef DEF_RINGBUFFER_H
#define DEF_RINGBUFFER_H

#include "Utility.h"

class Ringbuffer{

	public :
	
		/*
		*	Create a ring buffer
		*	length : ringbuffer's lenght in ms
		*	samplerate : samplerate in Hz
		*/
		Ringbuffer(int length, int samplerate);
		
		/*
		*	Read the block starting at (writing head) - delay
		*/
		int read_block(sample_t *block, int delay, int length);
	
		/*
		*	Write a new block and move the head
		*/
		int write_block(sample_t *block, int length);
	
	protected :
		sample_t *buffer;
	
		int buffer_size;
		int samplerate;
	
		int write_head;
};

#endif
