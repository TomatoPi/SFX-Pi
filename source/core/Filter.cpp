#include "Filter.h"

Filter_3EQ::Filter_3EQ(float lf, float hf, int sr){
	
	memset(this, 0, sizeof(Filter_3EQ));
	
	this->samplerate = sr;
	this->fl_bak = lf;
	this->fh_bak = hf;
	
	this->fl = spi_scale_frequency(lf, sr);
	this->fh = spi_scale_frequency(hf, sr);
}

sample_t Filter_3EQ::compute(sample_t sample, float gl, float gm, float gh){
	
	sample_t l,m,h; // Low / Mid / High

	// Compute lowpass if gain non null
	if(gl != 0.0f || gm != 0.0f){
		this->f1p0 += (this->fl * (sample - this->f1p0)) + vsa;
		this->f1p1 += (this->fl * (this->f1p0 - this->f1p1));
		this->f1p2 += (this->fl * (this->f1p1 - this->f1p2));
		this->f1p3 += (this->fl * (this->f1p2 - this->f1p3));

		l = this->f1p3;
	}else{
		l = 0.0f;
	}
	
	// Compute highpass if gain non null
	if(gh != 0.0f || gm != 0.0f){
		this->f2p0 += (this->fh * (sample - this->f2p0)) + vsa;
		this->f2p1 += (this->fh * (this->f2p0 - this->f2p1));
		this->f2p2 += (this->fh * (this->f2p1 - this->f2p2));
		this->f2p3 += (this->fh * (this->f2p2 - this->f2p3));

		h = this->sm3 - this->f2p3;
	}else{
		h = 0.0f;
	}

	// Compute bandpass ( sample - (low + high))
	m = this->sm3 - (h + l);

	// Compute gains
	l *= gl;
	m *= gm;
	h *= gh;

	// Suffle buffer
	this->sm3 = this->sm2;
	this->sm2 = this->sm1;
	this->sm1 = sample; 

	return(l + m + h);
}

void Filter_3EQ::set_freq(float lf, float hf){
	
	this->set_lf(lf);
	this->set_hf(hf);
}

void Filter_3EQ::set_lf(float lf){
	
	this->fl = spi_scale_frequency(lf, this->samplerate);
}

void Filter_3EQ::set_hf(float hf){
	
	this->fh = spi_scale_frequency(hf, this->samplerate);
}

float Filter_3EQ::get_lf() const{
	
	return this->fl_bak;
}

float Filter_3EQ::get_hf() const{
	
	return this->fh_bak;
}

inline float spi_scale_frequency(float f, int sr){
	
	return 2.0f * sin(M_PI * (f / (float)sr));
}

Filter_Comb::Filter_Comb(float f, float d, float fb, int sr){
	
	memset(this, 0, sizeof(Filter_Comb));
	
	this->samplerate = sr;
	
	this->size = spi_scale_comb(f, sr);
	this->buffer = (sample_t*) calloc(this->size, sizeof(sample_t));
	
	if(this->buffer == NULL){
		fprintf(stderr, "Memory Error\n");
		exit(1);
	}
	
	this->damp1 = d;
	this->damp2 = 1 - d;
	
	this->feedback = fb;
}

Filter_Comb::~Filter_Comb(){
	
	free(this->buffer);
}

sample_t Filter_Comb::compute(sample_t sample){
	
	sample_t out;
	int idx = this->index;
	
	out = this->buffer[idx];
	this->store = (out * this->damp2) + (this->store * this->damp1);
	
	this->buffer[idx] = sample + (this->store * this->feedback);
	
	this->index = (++this->index)%this->size;
	
	return out;
}

void Filter_Comb::set_size(int s){
	
	sample_t *bak = (sample_t*)realloc(this->buffer, s * sizeof(sample_t));
	if(bak != NULL){
		free(this->buffer);
		this->buffer = bak;
		this->size = s;
		this->index = 0;
	}
}

void Filter_Comb::set_freq(float f){
	
	this->set_size(spi_scale_comb(f, this->samplerate));
}

void Filter_Comb::set_damp(float d){
	
	this->damp1 = d;
	this->damp2 = 1 - d;
}

void Filter_Comb::set_feedback(float f){
	
	this->feedback = f;
}

int Filter_Comb::get_size() const{
	
	return this->size;
}

float Filter_Comb::get_freq() const{
	
	return (float)this->samplerate / (float)this->size;
}

float Filter_Comb::get_damp() const{
	
	return this->damp1;
}

float Filter_Comb::get_feed() const{
	
	return this->feedback;
}

inline int spi_scale_comb(float f, int sr){
	
	return (int)((float)sr / f);
}

Filter_Allpass::Filter_Allpass(float f, int s){
	
	memset(this, 0, sizeof(Filter_Allpass));
	
	this->feedback = f;
	this->size = s;
	this->index = 0;
	
	this->buffer = (sample_t*) calloc(s, sizeof(sample_t));
	if(this->buffer == NULL){
		fprintf(stderr, "Memory Error\n");
		exit(1);
	}
}


Filter_Allpass::~Filter_Allpass(){
	
	free(this->buffer);
}

sample_t Filter_Allpass::compute(sample_t sample){
	
	sample_t out;
	sample_t bout;
	int idx = this->index;
	
	bout = this->buffer[idx];
	out = -sample + bout;
	this->buffer[idx] = sample + (bout * this->feedback);
	
	this->index = (++this->index)%this->size;
	
	return out;
}

void Filter_Allpass::set_size(int s){
	
	sample_t *bak = (sample_t*)realloc(this->buffer, s * sizeof(sample_t));
	if(bak != NULL){
		free(this->buffer);
		this->buffer = bak;
		this->size = s;
		this->index = 0;
	}
}

void Filter_Allpass::set_feedback(float f){
	
	this->feedback = f;
}

int Filter_Allpass::get_size() const{
	
	return this->size;
}

float Filter_Allpass::get_feedback() const{
	
	return this->feedback;
}