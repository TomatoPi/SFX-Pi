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

int SAMPLERATE = 48000;
const char *SERVER_NAME = "Space_Fx";

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	Drive test = Drive(SERVER_NAME, (char*)"HC");
	LFO test2 = LFO(SERVER_NAME, (char*)"LFO");
	Ringmod test3 = Ringmod(SERVER_NAME, (char*)"RG");
	Delay test4 = Delay(SERVER_NAME, (char*)"DL");
	sleep(-1);
}
