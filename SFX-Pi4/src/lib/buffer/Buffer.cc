/**********************************************************************
 * @file Buffer.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing data structures to store and read samples
 **********************************************************************/
#include "Buffer.h"
#include <iostream>

using namespace sfx;
using namespace std;

/**********************************************************************
 *	                    Basic buffer stuff
***********************************************************************/
Buffer::Buffer(int length, int samplerate){

    length = length <= 0 ? 1 : length;

	this->size_ = sfx::mstos(length, samplerate);
	
	this->buffer_ = new sfx::sample_t[this->size_];
	this->write_i_ = 0;
}

Buffer::~Buffer(){
	
	delete this->buffer_;
}

void Buffer::write(sfx::sample_t value){

	this->write_i_ = ( this->write_i_ + 1 ) % this->size_;
    this->buffer_[(int)this->write_i_] = value;
}

void Buffer::set_length(int l, int sr){
	
	this->set_length(sfx::mstos(l, sr));
}

void Buffer::set_length(int s){
	
    //cout << "Try resize buffer : " << this->size_ << endl;
    s = s <= 0 ? 1 : s;
    
	delete this->buffer_;
    this->buffer_ = new sfx::sample_t[s];
    
    this->size_ = s;
    this->write_i_ = 0;
        
    //cout << "Buffer resized : now : " << s << endl;
    
    this->clear();
}

int Buffer::get_length() const{
	
	return this->size_;
}

int Buffer::get_length(int sr) const{
	
	return sfx::stoms(this->size_, sr);
}

void Buffer::clear(){
    
    for ( int i = 0; i < size_; i++ ){
        
        buffer_[i] = 0;
    }
}

/*
*	---------------------------------------------------------------------------
*	Singletap buffer
*	---------------------------------------------------------------------------
*/
Buffer_S::Buffer_S(int length, int samplerate, int delay):Buffer( (length>delay)?length:delay, samplerate){
	
	this->read_i_ = this->size_ - sfx::mstos(delay, samplerate);
    if ( this->read_i_ < 0 ) this->read_i_ = 0;
}

Buffer_S::Buffer_S(int length, int samplerate):Buffer(length, samplerate){
	
	this->read_i_ = 0;
}

void Buffer_S::set_length(int l, int sr){
	
	this->Buffer::set_length(l, sr);
	this->read_i_ = 0;
}

void Buffer_S::set_length(int s){
	
	this->Buffer::set_length(s);
	this->read_i_ = 0;
}

sfx::sample_t Buffer_S::read(){
	
	this->read_i_ = fmod( this->read_i_ +1, this->size_);
	return this->buffer_[(int)this->read_i_];
}

sfx::sample_t Buffer_S::read(float speed){
	
	this->read_i_ = fmod(this->read_i_ + speed, this->size_);
	return ( (1 - fmod(this->read_i_, 1.0f)) * this->buffer_[(int)(this->read_i_)] )
        + ( fmod(this->read_i_, 1.0f) * this->buffer_[((int)(this->read_i_)+1)%this->size_] );
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
	this->read_i_ = new float[count];
    
    //cout << "Params entered : " << length << " samplerate : " << samplerate << endl;
    //cout << "Created buffer of : " << size_ << " samples and with : " << count_ << " Readers" << endl;
    
	for(int i = 0; i < count; i++){
        
        this->read_i_[i] = (this->size_ - sfx::mstos(delay[i], samplerate)) % this->size_;
        //cout << "R : " << read_i_[i] << "frames ( " << delay[i] << " ms ) -- ";
    }
    
  // cout << endl << "Buffer created : Write Head : " << write_i_ << endl;
}

Buffer_M::~Buffer_M(){
    
    delete[] read_i_;
}

void Buffer_M::set_length(int l, int sr){

	this->set_length(sfx::mstos(l, sr));
}

void Buffer_M::set_length(int s){
	
	for(int i = 0; i < this->count_; i++) this->read_i_[i] = (this->read_i_[i] * s) / this->size_;
	int w = (this->write_i_ * s) / this->size_;
	
	this->Buffer::set_length(s);
	this->write_i_ = w;
}

sfx::sample_t Buffer_M::read(int idx){
	
	if(idx < this->count_){
		
		this->read_i_[idx] = fmod(++(this->read_i_[idx]), this->size_);
		return this->buffer_[(int)this->read_i_[idx]];
	}
	return 0;
}

sfx::sample_t Buffer_M::read(int idx, float speed){
		
	if(idx < this->count_){
		
		this->read_i_[idx] = fmod(this->read_i_[idx] + speed, this->size_);
		return ( (1 - fmod(this->read_i_[idx], 1.0f)) * this->buffer_[(int)(this->read_i_[idx])] )
            + ( fmod(this->read_i_[idx], 1.0f) * this->buffer_[((int)(this->read_i_[idx])+1)%this->size_] );
	}
	return 0;
}

void Buffer_M::set_reader(int count, int *delay, int sr){

	for(int i = 0; i < count; i++) delay[i] = sfx::mstos(delay[i], sr);
	this->set_reader(count, delay);
}

void Buffer_M::set_reader(int count, int *delay){
	
	
	int max = 0;
	for(int i = 0; i < count; i++) max = (max < delay[i])?delay[i]:max;	
	if(max > this->size_) this->set_length(max);
	
	if(count != this->count_){
		
        delete this->read_i_;
        float *n = new float[count];
		this->read_i_ = n;
    
        this->count_ = count;
	}
	
	this->write_i_ = 0;
	for(int i = 0; i < count ; i++) this->read_i_[i] = this->size_ - delay[i];
}

int Buffer_M::get_reader_count() const{
	
	return this->count_;
}

/*
*   ---------------------------------------------------------------------------
*                               Running summer
*   ---------------------------------------------------------------------------
*/
Buffer_R::Buffer_R( int length, int samplerate ): Buffer( length, samplerate ),
    summ_(0)
{
    
}

void Buffer_R::set_length( int length, int samplerate ){
    
    this->Buffer::set_length( length, samplerate );
    summ_ = 0;
}

void Buffer_R::set_length( int length ){
    
    this->Buffer::set_length( length );
    summ_ = 0;
}

sfx::sample_t Buffer_R::read(){
    
    return buffer_[write_i_];
}

float Buffer_R::get_rms2(){
    
    return summ_;
}

void Buffer_R::write( sfx::sample_t data ){
    
    // Remove oldest sample from the summ
    summ_ -= ( buffer_[write_i_]*buffer_[write_i_] ) / (float)size_;
    
    // Write new value
    buffer_[write_i_] = data;
    
    // Add new value to the summ
    summ_ += ( data*data ) / (float)size_;
    
    // Move write head
    write_i_ = (write_i_ + 1) % size_;
    
}

/**********************************************************************
 *                      Anchored Buffer
 **********************************************************************/
Buffer_A::Buffer_A(int length, int samplerate, int count, int *delay):
    Buffer_M( length, samplerate, count, delay )
{

    read_anchor_ = new float[count];
    
    memcpy( read_anchor_, read_i_, count * sizeof( float ) );
}

Buffer_A::~Buffer_A(){

    delete read_anchor_;
}

sfx::sample_t Buffer_A::read(int idx, float speed){

    if ( idx < count_ ){

        read_anchor_[idx] = fmod(read_anchor_[idx] + 1, size_);
    }
    return Buffer_M::read( idx, speed );
}

void Buffer_A::sync( int idx ){

    if ( idx < count_ && idx >= 0 ){
        
        read_i_[idx] = read_anchor_[idx];
    }
}
