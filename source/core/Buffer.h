#ifndef DEF_BUFFER_H
#define DEF_BUFFER_H

#include "Utility.h"

/*
*	Basic buffer class
*/
class Buffer{

	public :
	
		/*
		*	Create a buffer
		*	length : buffer's lenght in ms
		*	samplerate : samplerate in Hz
		*/
		Buffer(int length, int samplerate);
		~Buffer();
	
		/*
		*	Write the value
		*/
		void write(sample_t value);
		
		virtual void set_length(int l, int sr);
		virtual void set_size(int s);
		
		int get_size() const;
		int get_length(int sr) const;
	
	protected :
	
		sample_t *buffer_;
		int size_;
	
		int write_i_;
};

class Buffer_S : public Buffer{
	
	public :
	
		Buffer_S(int length, int samplerate);
		Buffer_S(int length, int samplerate, int delay);
		
		virtual void set_length(int l, int sr);
		virtual void set_size(int s);
		
		sample_t read();
		sample_t read(float speed);
		
	protected :
	
		float read_i_;
};

class Buffer_M : public Buffer{
	
	public :
	
		Buffer_M(int length, int samplerate, int count, int *delay);
		
		virtual void set_length(int l, int sr);
		virtual void set_size(int s);
		
		sample_t read(int idx);
		sample_t read(int idx, float speed);
		
		void set_reader_l(int count, int *delay, int sr);
		void set_reader_s(int count, int *delay);
		
		int get_reader_count() const;
		
	protected :
	
		int count_;
		float *read_i_;
};
#endif