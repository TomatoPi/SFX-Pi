#include "Ringbuffer.h"

Ringbuffer::Ringbuffer(int length, int samplerate){

	this->samplerate = samplerate;
	this->buffer_size = spi_mstos(length, samplerate);
	
	this->buffer = (sample_t*) malloc(this->buffer_size * sizeof(sample_t));
	if(this->buffer == NULL){
		exit(0);
	}
	memset( this->buffer, 0.0, this->buffer_size * sizeof(sample_t) );
	
	this->write_head = 0;
}

Ringbuffer::~Ringbuffer(){
	
	free(this->buffer);
}

void Ringbuffer::read_value(rng_reader *reader) const{
	
	reader->index = fmod(++(reader->index),this->buffer_size);
	reader->value = this->buffer[(int)(reader->index)];
}

void Ringbuffer::reverse_read_value(rng_reader *reader) const{
	
	reader->index = fmod(--(reader->index),this->buffer_size);
	reader->value = this->buffer[(int)(reader->index)];
}

void Ringbuffer::variable_read_value(rng_reader *reader, float speed) const{
	
	reader->index = fmod(reader->index+speed,this->buffer_size);
	reader->value = ( (1 - fmod(reader->index, 1.0)) * this->buffer[(int)(reader->index)] ) + ( fmod(reader->index, 1.0) * this->buffer[((int)(reader->index)+1)%this->buffer_size] );
}

void Ringbuffer::write_value(sample_t value){

	this->write_head = ++(this->write_head)%this->buffer_size;
	this->buffer[this->write_head] = value;
}

rng_reader Ringbuffer::new_read_head(int ms) const{

	int delay_s = spi_mstos(ms, this->samplerate);
	int index = (delay_s > this->write_head)?(this->buffer_size + this->write_head - delay_s):(this->write_head - delay_s);
	rng_reader r = { (float)index, (float)this->buffer[index], ms};
	
	return r;
}
