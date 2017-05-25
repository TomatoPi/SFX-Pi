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

void Ringbuffer::read_value(rng_reader *reader){
	
	reader->index = ++(reader->index)%this->buffer_size;
	reader->value = this->buffer[reader->index];
}

void Ringbuffer::write_value(sample_t value){

	this->write_head = ++(this->write_head)%this->buffer_size;
	this->buffer[this->write_head] = value;
}

rng_reader Ringbuffer::new_read_head(int ms){

	int delay_s = spi_mstos(ms, this->samplerate);
	int index = (delay_s > this->samplerate)?(this->buffer_size + this->write_head - delay_s):(this->write_head - delay_s);
	rng_reader r = { index, this->buffer[index]};
	
	return r;
}
