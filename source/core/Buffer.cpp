#include "Buffer.h"

Buffer_S::Buffer_S(int length, int samplerate){

	this->size = spi_mstos(length, samplerate);
	
	this->buffer = (sample_t*) calloc(this->size, sizeof(sample_t));
	if(this->buffer == NULL){
		exit(1);
	}
	this->write_i = 0;
	this->read_i = 0;
}

Buffer_S::~Buffer_S(){
	
	free(this->buffer);
}

sample_t Buffer_S::read(){
	
	this->read_i = fmod(++(this->read_i), this->size);
	return this->buffer[(int)(this->read_i)];
}

sample_t Buffer_S::v_read(float speed){
	
	this->read_i = fmod(this->read_i + speed, this->size);
	return ( (1 - fmod(this->read_i, 1.0f)) * this->buffer[(int)(this->read_i)] ) + ( fmod(this->read_i, 1.0f) * this->buffer[((int)(this->read_i)+1)%this->size] );
}

void Buffer_S::write(sample_t value){

	this->write_i = ++(this->write_i)%this->size;
	this->buffer[this->write_i] = value;
}

void Buffer_S::set_length(int l, int sr){
	
	this->set_size(spi_mstos(l, sr));
}

void Buffer_S::set_size(int s){
	
	sample_t *bak = (sample_t*)realloc(this->buffer, s * sizeof(sample_t));
	if(bak != NULL){
		//free(this->buffer);
		this->buffer = bak;
		this->size = s;
		this->write_i = 0;
		this->read_i = 0;
	}
}

int Buffer_S::get_size() const{
	
	return this->size;
}

int Buffer_S::get_length(int sr) const{
	
	return spi_stoms(this->size, sr);
}