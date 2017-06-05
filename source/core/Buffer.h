#ifndef DEF_BUFFER_H
#define DEF_BUFFER_H

#include "Utility.h"

/*
*	Single tape buffer
*/
class Buffer_S{

	public :
	
		/*
		*	Create a buffer
		*	length : buffer's lenght in ms
		*	samplerate : samplerate in Hz
		*/
		Buffer_S(int length, int samplerate);
		~Buffer_S();
		
		/*
		*	Move the reader and get the value
		*/
		sample_t read();
		sample_t v_read(float speed);
	
		/*
		*	Write the value
		*/
		void write(sample_t value);
		
		void set_length(int l, int sr);
		void set_size(int s);
		
		int get_size() const;
		int get_length(int sr) const;
	
	protected :
	
		sample_t *buffer;
		int size;
	
		int write_i;
		float read_i;
};

#endif
