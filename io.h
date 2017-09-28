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
#endif