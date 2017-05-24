#ifndef DEF_RINGBUFFER_H
#define DEF_RINGBUFFER_H

class Ringbuffer{

	public :
	
		Ringbuffer(int length);
		
	
	protected :
		float *buffer;
		int buffer_size;
		int write_head;
};

#endif
