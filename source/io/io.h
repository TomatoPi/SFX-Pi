#ifndef DEF_IO_H
#define DEF_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringShift.h>
#include <mcp3004.h>
#include <unistd.h>
#include "rs232.h"
#include "mcp23017.h"

#include <time.h>

#include "../core/Utility.h"
#include "../core/Modules.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023.0f
#define SPI_HYSTERESIS 0.01f
#define SPI_POTAR_COUNT 6

#define BUF_SIZE 128

void io_init_spi();

float io_map(float val, float fmin, float fmax, float tmin, float tmax); // remap val from [fmin;fmax] to [tmin;tmax]

class IO_Potentiometer{
	
	public :
		
		IO_Potentiometer(int idx, char* name, float min, float max);
		
		void set_name(char* name);
		void set_plage(float min, float max);
		
		void update();
		
		float 	get_value() const;
		
	private :
		
		char 	name_[40];
		int 	idx_;
		
		float 	min_, max_;
		
		int 	value_;
};

typedef enum{
	
	BTOGGLE=0,
	BMOMENT=1
}IO_PUSH_TYPE;

class IO_Button{
	
	public :
	
	private :
	
	
};

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
	
		IO_Accessor(Module *module, int target_idx, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);
		
		int update(int *potar_tab);
		
		int is_dead() const;
		
		int 	get_target_idx() const;
		int 	get_target_param() const;
		int 	get_potar() const;
		float 	get_min() const;
		float 	get_max() const;
		int 	get_curve() const;
		int 	get_db() const;
		int 	get_inv() const;
	
	private :
		Module *module_;
	
		float (*curve)(float value);
		IO_CURVE curve_type;
		int target_idx;
		int target_param;
		
		int potentiometer;
		int is_db, is_inv;
		float min, max;
	
		int value;
		int state;
};

class IO_screen{
	
	public :
	
		IO_screen();
		
		void print(const char* text, int t);
		void update();
		
	private :
	
		time_t 	time_;	/* Used to store last screen modification time */
		int 	fix_;	/* Store delay before screen reinitialisation */
};

static IO_screen *MAIN_SCREEN;

void io_init_potar_tab(IO_Potentiometer **pot);
void io_update_potar_tab(IO_Potentiometer **pot, int *tab);

static int 		io_screen_port 	= 16; 				/* /dev/ttyUSB0 */
static int 		io_screen_bdrate = 57600;			/* 9600 baud */
static char 	io_screen_mode[]={'8','N','1',0};	/* 8 data bits, no parity, 1 stop bit */

static const char* io_screen_default = "Space-Fx--!";

#endif