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
