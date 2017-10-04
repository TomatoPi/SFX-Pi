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

#include "../modules/EndModule.h"

#include "./menu/Menu.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023.0f
#define SPI_HYSTERESIS 0.005f
#define SPI_POTAR_COUNT 6

#define BUF_SIZE 128

void io_init_spi();

float io_map(float val, float fmin, float fmax, float tmin, float tmax); // remap val from [fmin;fmax] to [tmin;tmax]

/**
* Class used for store data about potentiometers.
* One instance of this class is created for each potentiometer ( actually 6 )
* Each potentiometer have a display name, a display plage and 
* a vector of accessors, used for modulate module's params
*/
class IO_Potentiometer{
	
	public :
        
        static const int D_NAME; // Flag for display potentiometer Name
        static const int D_VAL;  // Flag used for display pot value
        static const int D_ALL;  // Flag used for display everything
		
        /**
        * Potentiometer Constructor.
        * @param idx potentiometer's idx , must be inferior than SPI_POTAR_COUNT
        * @param name potentiometer's name must be 11 charachters length max
        * @param min display min
        * @param max display max
        */
		IO_Potentiometer( int idx=0, string name=string("DPot"), float min=0, float max=999 );
        
        /**
        * Change potentiomer's index
        * @param idx new potentiometer's index
        */
        void set_index( int idx );
		
        /**
        * Change potentiomer's name
        * @param name new name, must be 11 characters length max
        */
		void set_name( string name );
        /**
        * Get Potentiometer's name
        * @return potentiomer's name
        */
        string get_name() const;
        
        /**
        * Change potentiometer's display plage
        * @param min display min
        * @param max display max
        */
		void set_plage( float min, float max );
        
        /**
        * Get minimal display value
        * @return minimal display value
        */
        float get_min() const;
        
        /**
        * Get maximal display value
        * @param maximal display value
        */
        float get_max() const;
		
        /**
        * Update potentiometer's value.
        * Read actual value using gpio and update it if it differs from the old one
        * If value has changed, update it's accessor array ( update module's params )
        * @param graph module's graph
        * @param disp flag used for specify what will be displayed
        */
		void update( Module_Node_List* graph , int disp=0 );
        
        /**
        * Return current potentiometer's value.
        * @return current value mapped between -1 and 1;
        */
		float 	get_value() const;
        
        /**
        * Add given accessor to accessor list
        * @param acc accessor to add
        */
        void add_accessor( Accessor acc );
        
        /**
        * Delete given accessor if exist
        * @param given accessor idx
        */
        void del_accessor( int idx );
        
        int get_accessor_count();
        
        vector<Accessor> get_accessor();
        
		
	private :
        
        void display( int flag );
		
		string  name_;
		int 	idx_;
		
		float 	min_, max_;
		
		int 	value_;
        
        vector< Accessor > accs_;
};

void io_init_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT] );
void io_update_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT], Module_Node_List* graph );

namespace IOS{
    
    static const int SEG7   = 0x01; //**< Given message will be displayed on the 7Segment display */
    static const int STACK  = 0x02; //**< Given message will be stacked in screen's buffer */
    static const int OVERIDE= 0x04; //**< Given message will replace current one */
    static const int CLEAR  = 0x08; //**< Clear screen's messages buffer */
    static const int NEXT   = 0x10; //**< Force screen to switch to next message */
    static const int PREV   = 0x20; //**< Force screen to switch to previous message */
    static const int FORCE  = 0x40; //**< Force screen to switch to given message ( index given instead of time ) */
    static const int TEMP   = 0x80; //**< Given message will be a temp message */
    
    static const int DEFAULT_TEMP = 1500;
    
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

typedef enum{
    
    PUSH,
    PULL,
    PUSH_PUSH,
    MOMENT
}IO_PUSH_TYPE;

typedef enum{

    FUNC_MENU,
    FUNC_BANK_NEXT,
    FUNC_BANK_PREV
}IO_BUTTON_FUNCTION;

/**
 * Object containning all datas for adress a button or an io
 * It contain on wich register the adress refers to
 * On wich register the adress is
 * The hex value of the adress
 */
class IO_Adress{

    public :
    
        /**
         * Adress Constructor
         */
        IO_Adress( unsigned char adr, bool gb, bool m1 ):
                    adr_(adr), gpiob_(gb), mcp1_(m1)
        {
        }

        unsigned char adr_; /**< Store Hex adress inside register */
        bool gpiob_;    /**< True if Adress is on gpioB */
        bool mcp1_;     /**< true if Adress is on MCP1 */
};

/**
 * Class used for store datas about buttons
 */
class IO_Button{

    public :

        /**
         * Button Constructor.
         * The button adress can only be set at button definition
         * @param type define if button is considered as a toggle
         *              or in momentary mode.
         * @param func define button function inside program
         * @param adr button adress on MCP registers
         * @see IO_PUSH_TYPE
         * @see IO_BUTTON_FUNCTION
         */
        IO_Button( IO_PUSH_TYPE type, IO_BUTTON_FUNCTION func, IO_Adress adr );

        /**
         * Change button mode
         * Define on witch state button activate it function
         * @see IO_PUSH_TYPE
         */
        void set_type( IO_PUSH_TYPE type );

        /**
         * Get Button Mode
         */
        IO_PUSH_TYPE get_type() const;

        /**
         * Change button function
         * Define what action will be performed at button activation
         * @see IO_BUTTON_FUNCTION
         */
        void set_func( IO_BUTTON_FUNCTION func );

        /**
         * Get Button Function
         */
        IO_BUTTON_FUNCTION get_func() const;

        /**
         * Compute given structure depending on curent button's mode
         */
        void* compute( void* data );

        /**
         * Get Button adress
         */
        IO_Adress get_adr() const;
        

    protected :

        IO_PUSH_TYPE type_;
        IO_BUTTON_FUNCTION func_;
        
        IO_Adress adr_;

        void* data_;
};

namespace HEX{

    #define HEX_UP      0x08    // MCP0 GPIOB 0
    #define HEX_DOWN    0x04    // MCP0 GPIOB 1
    #define HEX_NEXT    0x02    // MCP0 GPIOB 2
    #define HEX_PREV    0x40    // MCP0 GPIOA 3
    #define HEX_ADD     0x80    // MCP0 GPIOA 4
    #define HEX_DEL     0x01    // MCP0 GPIOB 5
    #define HEX_ENTER   0x10    // MCP0 GPIOB 6
    #define HEX_ESC     0x20    // MCP0 GPIOB 7
    #define HEX_OK      0x40    // MCP0 GPIOB 8
    #define HEX_EDIT    0x80    // MCP0 GPIOB 9

    #define MASK_ADRRA_MENU 0xc0// mask for select only Menu buttons
    #define MASK_ADRRB_MENU 0xff// mask for select only Menu buttons
    
    #define HEX_FOOT_NEXT 0x20  // MCP0 GPIOA 10
    #define HEX_FOOT_PREV 0x10  // MCP0 GPIOA 11

    #define MASK_ADRRA_FOOT 0x30 // mask for select only footswitches
}

/**
*   Initialise front panel's buttons.
*   Set coresponding ports to inputs and reverse their state
*   @param version current program version
*/
void io_init_frontPanel();

/**
*   Check if a front button has been pushed.
* 	@param graph pointer to process graph
* 	@param pot potentiometers array
*/
void io_update_frontPanel( Module_Node_List* graph );

#endif
