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
		virtual ~Buffer();
	
		/*
		*	Write the value
		*/
		virtual void write(sample_t value);
		
		virtual void set_length(int l, int sr);
		virtual void set_length(int s);
		
		int get_length() const;
		int get_length(int sr) const;
	
	protected :
	
		sample_t *buffer_;
		int size_;
	
		int write_i_;
};

/**
*   Single tape buffer.
*   Provide buffer with a single read head
*   It can be directly after read head ( signal will be delayed by an amount equal of buffer length )
*   Or placed at a choosen delay time
*   In order to perfom delaying without issues, signal will be first readed, and secondly writed
*/
class Buffer_S : public Buffer{
	
	public :
	
		Buffer_S(int length, int samplerate);
		Buffer_S(int length, int samplerate, int delay);
		
		virtual void set_length(int l, int sr);
		virtual void set_length(int s);
		
		sample_t read();
		sample_t read(float speed);
        
        virtual void write(sample_t value) { return Buffer::write( value ); };
		
	protected :
	
		float read_i_;
};

class Buffer_M : public Buffer{
	
	public :
	
		Buffer_M(int length, int samplerate, int count, int *delay);
        ~Buffer_M();
		
		virtual void set_length(int l, int sr);
		virtual void set_length(int s);
		
		sample_t read(int idx);
		sample_t read(int idx, float speed);
		
		void set_reader(int count, int *delay, int sr);
		void set_reader(int count, int *delay);
		
		int get_reader_count() const;
        
        virtual void write(sample_t value) { return Buffer::write( value ); };
		
	protected :
	
		int count_;
		float *read_i_;
};

/**
*   Running summ buffer
*   perform same operation as single tape buffer but also perform a 
*/
class Buffer_R : public Buffer{
    
    public :
        
        Buffer_R( int lenght, int samplerate );
        
        void set_length( int length, int samplerate );
        void set_length( int length );
        
        sample_t read();
        
        float get_rms2();
        
        virtual void write( sample_t data );
        
    protected :
    
        float summ_;
    
};
#endif