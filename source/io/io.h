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
#include <bits/stdc++.h>

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

namespace IOS{
    
    static const int SEG7   = 0x01; //**< Given message will be displayed on the 7Segment display */
    static const int STACK  = 0x02; //**< Given message will be stacked in screen's buffer */
    static const int OVERIDE= 0x04; //**< Given message will replace current one */
    static const int CLEAR  = 0x08; //**< Clear screen's messages buffer */
    static const int NEXT   = 0x10; //**< Force screen to switch to next message */
    static const int PREV   = 0x20; //**< Force screen to switch to previous message */
    static const int FORCE  = 0x40; //**< Force screen to switch to given message ( index given instead of time ) */
    static const int TEMP   = 0x80; //**< Given message will be a temp message */
    
    /**
    * Open connection with arduino
    * @param msg Splash message sent to arduino
    * @param version current program version printed to 7Seg display
    */
    void init_screen( string msg, string version );
    
    /**
    * Send given message to the 14Segment display.
    * Given message will be displayed instantly to screen and will erase current message buffer
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    */
    void printm( string msg );
    /**
    * Send given message to the 14Segments display
    * Given message will be send with given flags, viewing delay will be set to forever
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    * @param flag special params for parsing and viewing message
    */
    void printm( string msg, int flag );
    /**
    * Send given message to the 14Segments display
    * Given message will be send with given flags, and given viewing delay
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    * @param t viewing time in ms ( 0 means forever )
    * @param flag special params for parsing and viewing message
    */
    void printm( string msg, int t , int flag);
    
}

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

#define MASK_ADRRA 0xf0     // mask for select only PREV and ADD values
#define MASK_ADRRB 0xff     // mask for select others buttons values

#define HEX_FOOT_NEXT 0x20 // MCP0 GPIOA 10
#define HEX_FOOT_PREV 0x10 // MCP0 GPIOA 11

#define BUTTON_COUNT 12

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
void io_init_frontPanel(Module_Node_List* Graph, string version);
/**
*   Check if a front button has been pushed.
*/
void io_update_frontPanel();

#endif