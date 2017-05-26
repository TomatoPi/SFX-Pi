#include "Reglages.h"

void io_init_spi(){
  
	int chan;
	int x;
	printf ("Raspberry Pi wiringPi test program\n") ;

	if (wiringPiSetup () == -1)
	  exit (1) ;

	mcp3004Setup (BASE, SPI_CHAN);

	for (chan = 0 ; chan < 8 ; ++chan) {
		
	  	x = analogRead (BASE + chan) ;
		printf("%d\n", x);
	}
}

int io_get_param(int param){
  
}
