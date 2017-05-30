#include "io.h"

void io_init_spi(){
  
	printf ("Raspberry Pi wiringPi test program\n") ;

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

int io_get_potentiometer(int potentiometer){
	
	if(potentiometer < 8) return analogRead(SPI_BASE + potentiometer);
	return -1;
}

io_param_accessor::io_param_accessor(int potentiometer, float min, float max, float *target, int is_db, int is_inv):min(min), max(max), target(target), is_db(is_db), is_inv(is_inv), value(0.0){

	if(potentiometer < 0 || potentiometer > 7)
		this->potentiometer = 0;
	else
		this->potentiometer = potentiometer;
}

void io_param_accessor::io_update_param(){
	
	float value = (float)io_get_potentiometer(this->potentiometer);
	if( (value > this->value * (1.0 + SPI_HYSTERESIS)) || (value  < this->value * (1.0 - SPI_HYSTERESIS)) ){
		
		this->value = value;
		if(this->is_inv)value = SPI_MAX - value;
		float param = ((value/SPI_MAX) * (this->max - this->min)) + this->min;

		if(is_db){
			*(this->target) = spi_dbtorms(param);
		}else{
			*(this->target) = param;
		}
	}
}
