#include "main.h"

const string VERSION = "1.1";

static Module_Node_List MAIN_LIST_MODULE;
//static int MAIN_COUNT_MODULE = 0;

static int MAIN_POTAR_VALUES[SPI_POTAR_COUNT] = {};

/*
*	---------------------------------------------------------------------------
*	Main programm function
*	---------------------------------------------------------------------------
*/
int main(int argc, char *argv[]){
	
	cout << "Bienvenu dans le fantastique et magnifique software Space-FX" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "VERSION:" << VERSION << " -- Run:16" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
    
	MAIN_SCREEN = new IO_screen();
	IO_Potentiometer* MAIN_POTAR_TAB[SPI_POTAR_COUNT];
	io_init_spi();
	io_init_potar_tab(MAIN_POTAR_TAB);
    
    io_init_frontPanel(&MAIN_LIST_MODULE, VERSION);
    
    MAIN_LIST_MODULE.add_module( MOD_COMP );
    
    MAIN_LIST_MODULE.add_connection(-1, 0,  0, 0);
    MAIN_LIST_MODULE.add_connection(-1, 0,  0, 1);
    MAIN_LIST_MODULE.add_connection( 0, 0, -1, 0);
    MAIN_LIST_MODULE.add_connection( 0, 1, -1, 1);
    
    /*
    float drive1[16] = { 1, 0, 0, 122, 1, 20, 0.7f, 0, 0, 0, 0, 200, 1000, 0.95f, 2.12f, 0.56f };
    MAIN_LIST_MODULE.add_module( MOD_DRIVE );
    MAIN_LIST_MODULE.list_[0]->get_module()->set_param(16, drive1);
    
    MAIN_LIST_MODULE.add_module( MOD_CHORUS );
    MAIN_LIST_MODULE.add_module( MOD_CHORUS );
    
    MAIN_LIST_MODULE.list_[2]->get_module()->set_param(1, -0.01);
    
    float lfo1[] = {1, 0, 3.5f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE.add_module( MOD_LFO );
    MAIN_LIST_MODULE.list_[3]->get_module()->set_param(9, lfo1);
    
    float lfo2[] = {1, 2, 2.0f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE.add_module( MOD_LFO );
    MAIN_LIST_MODULE.list_[4]->get_module()->set_param(9, lfo2);
    
    float lfo3[] = {1, 0, 7.3f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE.add_module( MOD_LFO );
    MAIN_LIST_MODULE.list_[5]->get_module()->set_param(9, lfo3);
    
    float lfo4[] = {1, 2, 1.2f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE.add_module( MOD_LFO );
    MAIN_LIST_MODULE.list_[6]->get_module()->set_param(9, lfo4);
    
    float lfo5[] = {1, 0, 5.5f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE.add_module( MOD_LFO );
    MAIN_LIST_MODULE.list_[7]->get_module()->set_param(9, lfo5);
    
    float drive2[16] = { 1, 0, 0, 85.6f, 0, 0, 0, 0, 0, 0, 0, 200, 1000, 0.51f, 1.59f, 0.97f };
    MAIN_LIST_MODULE.add_module( MOD_DRIVE );
    MAIN_LIST_MODULE.list_[8]->get_module()->set_param(16, drive2);
    
    float tone1[6] = { 0.15f, 200, 100, 8, 0.74f, 1.79f };
    MAIN_LIST_MODULE.add_module( MOD_TONE );
    MAIN_LIST_MODULE.list_[9]->get_module()->set_param(6, tone1);
    
    float tone2[6] = { 0.15f, 200, 100, 8, 1.3f, 1.19f };
    MAIN_LIST_MODULE.add_module( MOD_TONE );
    MAIN_LIST_MODULE.list_[10]->get_module()->set_param(6, tone2);
    
    MAIN_LIST_MODULE.add_connection(-1, 0,  0, 0);
    MAIN_LIST_MODULE.add_connection(-1, 0,  8, 0);
    MAIN_LIST_MODULE.add_connection( 0, 0,  1, 0);
    MAIN_LIST_MODULE.add_connection( 8, 0,  2, 0);
    
    MAIN_LIST_MODULE.add_connection( 3, 0,  1, 1);
    MAIN_LIST_MODULE.add_connection( 3, 0,  2, 1);
    
    MAIN_LIST_MODULE.add_connection( 4, 0,  1, 2);
    MAIN_LIST_MODULE.add_connection( 4, 0,  2, 2);
    
    MAIN_LIST_MODULE.add_connection( 5, 0,  1, 3);
    MAIN_LIST_MODULE.add_connection( 5, 0,  2, 3);
    
    MAIN_LIST_MODULE.add_connection( 6, 0,  1, 4);
    MAIN_LIST_MODULE.add_connection( 6, 0,  2, 4);
    
    MAIN_LIST_MODULE.add_connection( 7, 0,  1, 5);
    MAIN_LIST_MODULE.add_connection( 7, 0,  2, 5);
    
    MAIN_LIST_MODULE.add_connection( 1, 0,  9, 0);
    MAIN_LIST_MODULE.add_connection( 2, 0, 10, 0);
    
    MAIN_LIST_MODULE.add_connection( 9, 0, -1, 0);
    MAIN_LIST_MODULE.add_connection(10, 0, -1, 1);
    */
    
	//load_preset(string("DefaultPreset"), VERSION, &MAIN_LIST_MODULE);
    
    //MAIN_LIST_MODULE.begin_.get_module()->set_bypass( true );
    //MAIN_LIST_MODULE.end_.get_module()->set_bypass( true );
	
	/*
	*	---------------------Main Loop-------------------------------
	*/
	while(1){
	
		/*
		*	Read potar values and store values in MAIN_POTAR_VALUES
		*/
		io_update_potar_tab(MAIN_POTAR_TAB, MAIN_POTAR_VALUES);
        
        /*
        *   Check front pannel buttons
        */
        io_update_frontPanel();
		
		/*
		*	Update each accesor for each module
		*/
		for(Module_List::iterator itr = MAIN_LIST_MODULE.list_.begin(); itr != MAIN_LIST_MODULE.list_.end(); itr++){
			
			(*itr)->accessor_update(MAIN_POTAR_VALUES);
		}
		
		/*
		*	Render screen
		*/
		MAIN_SCREEN->update();
        
        /*
        * Check if input is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE.begin_.get_module())->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Input");
            MAIN_SCREEN->print( n, 0 );
            static_cast<EndModule*>(MAIN_LIST_MODULE.begin_.get_module())->reset_saturated();
        }
        
        /*
        * Check if output is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE.end_.get_module())->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Output");
            MAIN_SCREEN->print( n, 0 );
            static_cast<EndModule*>(MAIN_LIST_MODULE.end_.get_module())->reset_saturated();
        }
		
		/*
		*	Pause main loop for 50ms ( 20Hz )
		*/
		usleep(50000);
	}
    
    exit(0);
}