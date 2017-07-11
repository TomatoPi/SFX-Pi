#include "Buffer.h"
/*
*	---------------------------------------------------------------------------
*	Basic buffer stuff
*	---------------------------------------------------------------------------
*/
Buffer::Buffer(int length, int samplerate){

	this->size_ = spi_mstos(length, samplerate);
	
	this->buffer_ = (sample_t*) calloc(this->size_, sizeof(sample_t));
	if(this->buffer_ == NULL){
		fprintf(stderr, "Failed create buffer\n");
		exit(1);
	}
	this->write_i_ = 0;
}

Buffer::~Buffer(){
	
	free(this->buffer_);
}

void Buffer::write(sample_t value){

	this->write_i_ = ++(this->write_i_)%this->size_;
	this->buffer_[this->write_i_] = value;
}

void Buffer::set_length(int l, int sr){
	
	this->set_size(spi_mstos(l, sr));
}

void Buffer::set_size(int s){
	
	sample_t *bak = (sample_t*)realloc(this->buffer_, s * sizeof(sample_t));
	if(bak != NULL){
		//free(this->buffer);
		this->buffer_ = bak;
		this->size_ = s;
		this->write_i_ = 0;
	}
}

int Buffer::get_size() const{
	
	return this->size_;
}

int Buffer::get_length(int sr) const{
	
	return spi_stoms(this->size_, sr);
}

/*
*	---------------------------------------------------------------------------
*	Singletap buffer
*	---------------------------------------------------------------------------
*/
Buffer_S::Buffer_S(int length, int samplerate, int delay):Buffer( (length>delay)?length:delay, samplerate){
	
	this->read_i_ = this->size_ - spi_mstos(delay, samplerate);
}

Buffer_S::Buffer_S(int length, int samplerate):Buffer(length, samplerate){
	
	this->read_i_ = 0;
}

void Buffer_S::set_length(int l, int sr){
	
	this->Buffer::set_length(l, sr);
	this->read_i_ = 0;
}

void Buffer_S::set_size(int s){
	
	this->Buffer::set_size(s);
	this->read_i_ = 0;
}

sample_t Buffer_S::read(){
	
	this->read_i_ = fmod(++(this->read_i_), this->size_);
	return this->buffer_[(int)this->read_i_];
}

sample_t Buffer_S::read(float speed){
	
	this->read_i_ = fmod(this->read_i_ + speed, this->size_);
	return ( (1 - fmod(this->read_i_, 1.0f)) * this->buffer_[(int)(this->read_i_)] ) + ( fmod(this->read_i_, 1.0f) * this->buffer_[((int)(this->read_i_)+1)%this->size_] );
}

/*
*	---------------------------------------------------------------------------
*	Multitap buffer
*	---------------------------------------------------------------------------
*/
Buffer_M::Buffer_M(int length, int samplerate, int count, int *delay):Buffer(length, samplerate){
	
	int max = 0;
	for(int i = 0; i < count; i++) max = (max < delay[i])?delay[i]:max;
	if(max > length)this->set_length(max, samplerate);
	
	this->count_ = count;
	this->read_i_ = (float*)calloc(count, sizeof(float));
    
    cout << "Created buffer of : " << size_ << " samples and with : " << count_ << "Readers" << endl;
    
	for(int i = 0; i < count; i++){
        
        this->read_i_[i] = (this->size_ - spi_mstos(delay[i], samplerate)) % this->size_;
        cout << " R : " << read_i_[i];
    }
    
    cout << endl << "Buffer created" << endl;
}

void Buffer_M::set_length(int l, int sr){

	this->set_size(spi_mstos(l, sr));
}

void Buffer_M::set_size(int s){
	
	for(int i = 0; i < this->count_; i++) this->read_i_[i] = (this->read_i_[i] * s) / this->size_;
	int w = (this->write_i_ * s) / this->size_;
	
	this->Buffer::set_size(s);
	this->write_i_ = w;
}

sample_t Buffer_M::read(int idx){
	
	if(idx < this->count_){
		
		this->read_i_[idx] = fmod(++(this->read_i_[idx]), this->size_);
		return this->buffer_[(int)this->read_i_[idx]];
	}
	return 0;
}

sample_t Buffer_M::read(int idx, float speed){
		
	if(idx < this->count_){
		
		this->read_i_[idx] = fmod(this->read_i_[idx] + speed, this->size_);
		return ( (1 - fmod(this->read_i_[idx], 1.0f)) * this->buffer_[(int)(this->read_i_[idx])] ) + ( fmod(this->read_i_[idx], 1.0f) * this->buffer_[((int)(this->read_i_[idx])+1)%this->size_] );
	}
	return 0;
}

void Buffer_M::set_reader_l(int count, int *delay, int sr){

	for(int i = 0; i < count; i++) delay[i] = spi_mstos(delay[i], sr);
	this->set_reader_s(count, delay);
}

void Buffer_M::set_reader_s(int count, int *delay){
	
	
	int max = 0;
	for(int i = 0; i < count; i++) max = (max < delay[i])?delay[i]:max;	
	if(max > this->size_) this->set_size(max);
	
	if(count != this->count_){
		
		float *bak = (float*)realloc(this->read_i_, count * sizeof(int));
		if(bak != NULL){
			this->read_i_ = bak;
			this->count_ = count;
		}else{
			return;
		}
	}
	
	this->write_i_ = 0;
	for(int i = 0; i < count ; i++) this->read_i_[i] = this->size_ - delay[i];
}

int Buffer_M::get_reader_count() const{
	
	return this->count_;
}