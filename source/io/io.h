#ifndef DEF_IO_H
#define DEF_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringShift.h>
#include <mcp3004.h>
#include <unistd.h>
#include "../core/rs232.h"

#include "../core/Utility.h"
#include "../core/Modules.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023
#define SPI_HYSTERESIS 0.01f
#define SPI_POTAR_COUNT 8

#define BUF_SIZE 128

void io_init_spi();
void io_get_potentiometer(int *potar_tab);

typedef enum{
	
	CURVE_LIN=0,
	CURVE_SIG=1,
	CURVE_HAN=2,
	CURVEIHAN=3
}IO_CURVE;

float curve_lin(float v);	//	Identity tranfer function
float curve_sig(float v);	//	Tanh transfert function scaled between 0 and 1
float curve_han(float v);	//	Hanning window function
float curveihan(float v);	//	1 - Hanning window

class IO_Accessor{
 
	public :
	
		IO_Accessor(Module *target, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);
		
		int update(int *potar_tab);
		
		void set_target(Module *target, int target_param);
		int get_target() const;
		
		int is_dead() const;
	
	private :
	
		float (*curve)(float value);
		Module *target;
		int target_param;
		
		int potentiometer;
		int is_db, is_inv;
		float min, max;
	
		int value;
		int state;
};

class Screen{
	
	public :
	
		Screen();
		
	private :
	
};

static int io_screen_port = 16;
static int io_screen_bdrate = 57600;
static char io_screen_mode[]={'8','N','1',0};

void io_init_screen();
void io_print(char *text, int time);

#endif
