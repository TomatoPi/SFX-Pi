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

#include "../presset/Presset.h"

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
	
        IO_Button();
        
        void update();
        
	private :
	
        string name_;
        
        IO_PUSH_TYPE type_;
        
        bool status_;
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

/*
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*                                   Front panel
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*/

static mcp23017 *MCP0;
static mcp23017 *MCP1;

/*
*   -----------------------------------
*            Buttons adresses
*   -----------------------------------
*/
#define HEX_UP      0x8     // MCP0 GPIOB 0
#define HEX_DOWN    0x4     // MCP0 GPIOB 1
#define HEX_NEXT    0x2     // MCP0 GPIOB 2
#define HEX_PREV    0x40    // MCP0 GPIOA 3
#define HEX_ADD     0x80    // MCP0 GPIOA 4
#define HEX_DEL     0x1     // MCP0 GPIOB 5
#define HEX_ENTER   0x10    // MCP0 GPIOB 6
#define HEX_ESC     0x20    // MCP0 GPIOB 7
#define HEX_OK      0x40    // MCP0 GPIOB 8
#define HEX_EDIT    0x80    // MCP0 GPIOB 9

#define MASK_ADRRA 0xc0     // mask for select only PREV and ADD values
#define MASK_ADRRB 0xff     // mask for select others buttons values

#define EDIT_MODE   9       // 

/**
*   Initialise front panel's buttons.
*   Set coresponding ports to inputs and reverse their state
*/
void io_init_frontPanel(Module_Node_List *Graph, string version);
/**
*   Check if a front button has been pushed.
*/
void io_update_frontPanel();

void func_button_up(bool s);
void func_button_down(bool s);
void func_button_next(bool s);
void func_button_prev(bool s);
void func_button_add(bool s);
void func_button_del(bool s);
void func_button_enter(bool s);
void func_button_esc(bool s);
void func_button_ok(bool s);
void func_button_edit(bool s);

#endif