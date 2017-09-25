#include "io.h"

void io_init_spi(){

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

inline float io_map(float val, float fmin, float fmax, float tmin, float tmax){
	
	return ( ( ((val-fmin)/(fmax-fmin))*(tmax-tmin) ) + tmin );
}

/*
*	---------------------------------------------------------------------------
*	IO Potentiometer stuff
*	---------------------------------------------------------------------------
*/
IO_Potentiometer::IO_Potentiometer(int idx, char* name, float min, float max): idx_(idx), min_(min), max_(max){
	
	this->value_ = analogRead(SPI_BASE + idx);
	stpcpy(this->name_, name);
}

void IO_Potentiometer::set_name(char* name){
	
	strcpy(this->name_, name);
}

void IO_Potentiometer::set_plage(float min, float max){
	
	this->min_ = min;
	this->max_ = max;
	this->update();
}

void IO_Potentiometer::update(){
	
	float value = analogRead(SPI_BASE + this->idx_);
	float diff = (float)value - (float)this->value_;
	if( (diff > SPI_HYSTERESIS * SPI_MAX && diff > 0) || (diff < -SPI_HYSTERESIS * SPI_MAX && diff < 0)){
	
		this->value_ = value;
		float display = io_map((float)value, 0.0f, SPI_MAX, this->min_, this->max_);
		
		char p[BUF_SIZE] = {};
		sprintf(p, "%s %d", this->name_, (int)display);
	}
}

float IO_Potentiometer::get_value() const{
	
	return this->value_;
}

void io_init_potar_tab(IO_Potentiometer **pot){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		char n[BUF_SIZE];
		sprintf(n, "Potar-%d", i);
		pot[i] = new IO_Potentiometer(i, n, 0, 999);
	}
}

void io_update_potar_tab(IO_Potentiometer **pot, int *tab){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		tab[i] = analogRead(SPI_BASE + i);
		pot[i]->update();
	}
}

/*
*	---------------------------------------------------------------------------
*	Screen stuff
*	---------------------------------------------------------------------------
*/