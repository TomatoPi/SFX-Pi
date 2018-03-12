/**********************************************************************
 * @file Buffer.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing data structures to store and read samples
 **********************************************************************/
#ifndef DEF_BUFFER_H
#define DEF_BUFFER_H

#include "types.h"
#include "core/Utils.h"

#include <cstring>

/**
*	Basic buffer class
*/
class Buffer{

	public :
	
		/**
		*	Create a buffer
		*	@param length : buffer's lenght in ms
		*	@param samplerate : samplerate in Hz
		*/
		Buffer(int length, int samplerate);
		virtual ~Buffer();
	
		/**
		*	Write the value
		*/
		virtual void write(SFXP::sample_t value);
		
		virtual void set_length(int l, int sr);
		virtual void set_length(int s);
		
		int get_length() const;
		int get_length(int sr) const;
	
	protected :
	
		SFXP::sample_t *buffer_;
		int size_;
	
		int write_i_;
        
        void clear();
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
		
		SFXP::sample_t read();
		SFXP::sample_t read(float speed);
        
	protected :
	
		float read_i_;
};

class Buffer_M : public Buffer{
	
	public :
	
		Buffer_M(int length, int samplerate, int count, int *delay);
        ~Buffer_M();
		
		virtual void set_length(int l, int sr);
		virtual void set_length(int s);
		
		SFXP::sample_t read(int idx);
		virtual SFXP::sample_t read(int idx, float speed);
		
		void set_reader(int count, int *delay, int sr);
		void set_reader(int count, int *delay);
		
		int get_reader_count() const;
        
	protected :
	
		int count_;
		float *read_i_;
};

/**
*   Running summ buffer
*   perform same operation as single tape buffer but also perform a 
*   running summ calculation and an rms mesurement ( pow2 rms )
*/
class Buffer_R : public Buffer{
    
    public :
        
        Buffer_R( int lenght, int samplerate );
        
        void set_length( int length, int samplerate );
        void set_length( int length );
        
        SFXP::sample_t read();
        
        float get_rms2();
        
        virtual void write( SFXP::sample_t data );
        
    protected :
    
        float summ_;
    
};

/**
 * Anchored Buffer.
 * Perform a similar job as Multitape buffer
 * But provide an Advanced Variable speed read feature
 */
class Buffer_A : public Buffer_M{

    public :

        Buffer_A(int length, int samplerate, int count, int *delay);
        ~Buffer_A();

        virtual SFXP::sample_t read(int idx, float speed);

        /**
         * Function that reset Readers positions.
         */
        void sync( int idx );

    protected :

        float *read_anchor_;

};
#endif
