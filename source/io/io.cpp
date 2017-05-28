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

int io_set_param_accessor(io_param_accessor *accessor, int potentiometer, float min, float max, float *target, int is_db){
	
	if(potentiometer < 0 || potentiometer > 7 || target == NULL)
		return 1;
	
	accessor->potentiometer = potentiometer;
	accessor->min = min;
	accessor->max = max;
	accessor->target = target;
	accessor->is_db = is_db;
	
	return 0;
}

void io_update_param(io_param_accessor *accessor){
	
	float value = (float)io_get_potentiometer(accessor->potentiometer);
	float param = ((value/SPI_MAX) * (accessor->max - accessor->min)) + accessor->min;
	
	if(is_db){
		*(accessor->target) = spi_dbtorms(param);
	}else{
		*(accessor->target) = param;
	}
}
