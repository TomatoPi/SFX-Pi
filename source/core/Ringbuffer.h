#ifndef DEF_RINGBUFFER_H
#define DEF_RINGBUFFER_H

#include "Utility.h"

typedef struct{
	int index;
	sample_t value;
	
	int delay;
}rng_reader;
class Ringbuffer{

	public :
	
		/*
		*	Create a ring buffer
		*	length : ringbuffer's lenght in ms
		*	samplerate : samplerate in Hz
		*/
		Ringbuffer(int length, int samplerate);
		~Ringbuffer();
		
		/*
		*	Move the reader and get the value
		*/
		void read_value(rng_reader *reader);
	
		/*
		*	Write the value and move the writing head
		*/
		void write_value(sample_t value);
	
		rng_reader new_read_head(int ms);
	
	protected :
		sample_t *buffer;
	
		int buffer_size;
		int samplerate;
	
		int write_head;
};

#endif
