#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "./modules/Drive.h"
#include "./modules/LFO.h"

int SAMPLERATE = 48000;

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	Drive test = Drive(SERVER_NAME, (char*)"HC");
	LFO test2 = LFO(SERVER_NAME, (char*)"LFO");
	sleep(-1);
}
