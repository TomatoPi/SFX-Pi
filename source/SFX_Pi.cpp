#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "./modules/Drive.h"
#include "./modules/LFO.h"
#include "./modules/Ringmod.h"
#include "./modules/Delay.h"

#include <wiringPi.h>
#include <mcp3004.h>

#define BASE 200
#define SPI_CHAN 0

int SAMPLERATE = 48000;
const char *SERVER_NAME = "Space_Fx";

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	Drive test = Drive(SERVER_NAME, (char*)"HC");
	LFO test2 = LFO(SERVER_NAME, (char*)"LFO");
	Ringmod test3 = Ringmod(SERVER_NAME, (char*)"RG");
	Delay test4 = Delay(SERVER_NAME, (char*)"DL");
	
	int chan;
  	int x;
	printf ("Raspberry Pi wiringPi test program\n") ;

 	if (wiringPiSetup () == -1)
    		exit (1) ;

  	mcp3004Setup (BASE, SPI_CHAN); // 3004 and 3008 are the same 4/8 channels

  	for (chan = 0 ; chan < 8 ; ++chan) {
    		x = analogRead (BASE + chan) ;
  		printf("%d\n", x);
    	}
   	return 0 ;
	
//	sleep(-1);
}
