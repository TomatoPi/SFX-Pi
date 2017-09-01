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

/*
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*                                   Front panel
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*/

static mcp23017 *MCP0 = NULL;
static mcp23017 *MCP1 = NULL;

/*
*   -----------------------------------
*            Buttons adresses
*   -----------------------------------
*/
#define HEX_UP      0x08     // MCP0 GPIOB 0
#define HEX_DOWN    0x04     // MCP0 GPIOB 1
#define HEX_NEXT    0x02     // MCP0 GPIOB 2
#define HEX_PREV    0x40    // MCP0 GPIOA 3
#define HEX_ADD     0x80    // MCP0 GPIOA 4
#define HEX_DEL     0x01     // MCP0 GPIOB 5
#define HEX_ENTER   0x10    // MCP0 GPIOB 6
#define HEX_ESC     0x20    // MCP0 GPIOB 7
#define HEX_OK      0x40    // MCP0 GPIOB 8
#define HEX_EDIT    0x80    // MCP0 GPIOB 9

#define MASK_ADRRA 0xc0     // mask for select only PREV and ADD values
#define MASK_ADRRB 0xff     // mask for select others buttons values

typedef enum{
    
    WAIT_DEL_BANK,
    WAIT_DEL_PRESET,
    WAIT_EXIT_SELECTMOD,
    WAIT_EXIT_EDITBANK,
    WAIT_EXIT_PARAM,
    WAIT_NOTHING
    
}Wait_status;

typedef enum{
    
    MAIN_MENU,
    
        MAIN_CHANGE_PRESSET,
        MAIN_SELECT_MODULE,
    
            SELMOD,
            SELMOD_CHANGE_BANK,
            SELMOD_ADD_BANK,
            SELMOD_EDIT_BANK,
    
                SELMOD_EDITB_SELPAR,
                    SELMOD_EDITB_EDITPAR,
    
        EDIT_ADD_MODULE,
            EDIT_CHOOSE_MODTYPE,
            
        EDIT_SELECT_MODULE,
        
            EDITMOD,
            EDITMOD_ADD_CONNECTION,
            
                EDITMOD_CHOOSE_CSOURCE,
                EDITMOD_CHOOSE_CTARGET,
            
            EDITMOD_ADD_ACCESSOR,
            
                EDITMOD_ACC_CHOOSE_TARGET_M,
                EDITMOD_ACC_CHOOSE_TARGET_P,
                EDITMOD_ACC_CHOOSE_POT,
                EDITMOD_ACC_CHOOSE_MIN,
                EDITMOD_ACC_CHOOSE_MAX,
                EDITMOD_ACC_CHOOSE_CURVE,
                EDITMOD_ACC_CHOOSE_ISDB,
                EDITMOD_ACC_CHOOSE_ISINV,
    
    INIT
    
}Menu_status;

typedef enum{
    
    MOVE_UP,
    MOVE_DOWN,
    
    MOVE_NEXT,
    MOVE_PREV,
    
    MOVE_ADD,
    MOVE_DEL,
    
    MOVE_ENTER,
    
    MOVE_ESC,
    MOVE_OK,
    
    MOVE_ENTER_EDIT,
    MOVE_EXIT_EDIT,
    
    MOVE_NONE
    
}Move_flag;

/**
*   Initialise front panel's buttons.
*   Set coresponding ports to inputs and reverse their state
*/
void io_init_frontPanel(Module_Node_List *Graph, string version);
/**
*   Check if a front button has been pushed.
*/
void io_update_frontPanel();

#endif