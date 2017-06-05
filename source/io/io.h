#ifndef DEF_IO_H
#define DEF_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>

#include "../core/Utility.h"
#include "../core/Modules.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023
#define SPI_HYSTERESIS 0.01f

void io_init_spi();
int io_get_potentiometer(int potentiometer);

class IO_Accessor{
 
	public :
	
		IO_Accessor(Module *target, int param, int potentiometer, float min, float max, int is_db, int is_inv);
		
		void update();
		
		void set_target(Module *target, int param);
		
		int is_dead() const;
	
	private :
	
		Module *target;
		int param;
		
		int potentiometer;
		int is_db, is_inv;
		float min, max;
	
		int value;
};

#endif
