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
		int read_block(float *block, int delay, int length);
	
		/*
		*	Write a new block and move the head
		*/
		int write_block(float *block, int length);
	
	protected :
		float *buffer;
	
		int buffer_size;
		int samplerate;
	
		int write_head;
};

#endif
