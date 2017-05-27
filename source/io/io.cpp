#include "io.h"

void io_init_spi(){
  
	printf ("Raspberry Pi wiringPi test program\n") ;

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN);
}

int io_get_param(int param){
	
	if(param < 8) return analogRead(SPI_BASE + param);
	return -1;
}
