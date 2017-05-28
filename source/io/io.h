#ifndef DEF_IO_H
#define DEF_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>

#include "../core/Utility.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023

void io_init_spi();
int io_get_potentiometer(int potentiometer);

class io_param_accessor{
 
	public :
	
		io_param_accessor(int potentiometer, float min, float max, float *target, int is_db);
		int io_update_param(io_param_accessor *accessor);
	
		float *target;
		float min, max;
		int potentiometer;
		int is_db;
};

#endif
