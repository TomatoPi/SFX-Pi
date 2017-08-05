#include "main.h"

const string VERSION = "1.0";

static Module_Node_List MAIN_LIST_MODULE;
static int MAIN_COUNT_MODULE = 0;

static int MAIN_POTAR_VALUES[SPI_POTAR_COUNT] = {};

/*
*	---------------------------------------------------------------------------
*	Main programm function
*	---------------------------------------------------------------------------
*/
int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n-----------------v0.2\n");
	MAIN_SCREEN = new IO_screen();
	IO_Potentiometer* MAIN_POTAR_TAB[SPI_POTAR_COUNT];
	io_init_spi();
	io_init_potar_tab(MAIN_POTAR_TAB);
    
    io_init_frontPanel(&MAIN_LIST_MODULE, VERSION);
	
	MAIN_LIST_MODULE.add_module(MOD_LFO);
	
	save_preset(string("Test") , VERSION, MAIN_LIST_MODULE);
	load_preset(string("Test2"), VERSION, &MAIN_LIST_MODULE);
	save_preset(string("Test3"), VERSION, MAIN_LIST_MODULE);
	/*
	main_add_module(MDRIVE, 2);
	int drive = MAIN_COUNT_MODULE;
	float drivep[] = {0, 1, 20.0, 1, 8.0, 0.26, 10.0, 1, 10.0, 0.6, 440, 1200, 3.0, 0.75, 5.0};
	float hardp[] = {0, 1, 20.0, 0, 0.0, 0.0, 10.0, 1, 5.0, 0.4, 200, 1200, 8.0, 1.5, 1.0};
	MAIN_LIST_MODULE[drive]->get_module()->get_voice(0)->set_param_list(15, drivep);
	MAIN_LIST_MODULE[drive]->get_module()->get_voice(1)->set_param_list(15, hardp);
	main_add_connection(-1, 0, 0, drive, 0, 0);
	main_add_connection(-1, 0, 0, drive, 1, 0);
	main_add_accessor(drive, 0, 2, 0, 10.0, 60.0, CURVE_LIN, 1, 0);
	main_add_accessor(drive, 0, 6, 0, 10.0, 40.0, CURVE_LIN, 1, 1);
	main_add_accessor(drive, 1, 2, 1, 10.0, 60.0, CURVE_LIN, 1, 0);
	main_add_accessor(drive, 1, 6, 1, 10.0, 40.0, CURVE_LIN, 1, 0);
	char n[20];
	strcpy(n, "Disto-1");
	MAIN_POTAR_TAB[0]->set_name(n);
	MAIN_POTAR_TAB[0]->set_plage(10, 60);
	strcpy(n, "Disto-2");
	MAIN_POTAR_TAB[1]->set_name(n);
	MAIN_POTAR_TAB[1]->set_plage(10, 60);
	
	main_add_module(MDELAY, 2);
	int delay = MAIN_COUNT_MODULE;
	float delayr[] = {166, 0.6f, 0.4f};
	float delayl[] = {250, 0.6f, 0.4f};
	((Delay_voice*)MAIN_LIST_MODULE[delay]->get_module()->get_voice(0))->set_param_list(3, delayr);
	((Delay_voice*)MAIN_LIST_MODULE[delay]->get_module()->get_voice(1))->set_param_list(3, delayl);
	main_add_connection(drive, 0, 0, delay, 0, 0);
	main_add_connection(drive, 1, 0, delay, 1, 0);
	main_add_accessor(delay, 0, 0, 2, 100, 500, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 1, 0, 2, 150, 750, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 0, 1, 3, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 1, 1, 3, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	strcpy(n, "Delay-T");
	MAIN_POTAR_TAB[2]->set_name(n);
	MAIN_POTAR_TAB[2]->set_plage(100, 500);
	strcpy(n, "Delay-F");
	MAIN_POTAR_TAB[3]->set_name(n);
	MAIN_POTAR_TAB[3]->set_plage(0, 90);
	
	main_add_module(MREV, 2);
	int rev = MAIN_COUNT_MODULE;
	float reverb[] = {0.7f, 0.4f, 1.0f, 0.0f, 0.6f};
	MAIN_LIST_MODULE[rev]->get_module()->get_voice(0)->set_param_list(5, reverb);
	MAIN_LIST_MODULE[rev]->get_module()->get_voice(1)->set_param_list(5, reverb);
	main_add_connection(delay, 0, 1, rev, 0, 0);
	main_add_connection(delay, 1, 1, rev, 1, 0);
	main_add_connection(rev, 0, 0, delay, 0, 1);
	main_add_connection(rev, 1, 0, delay, 1, 1);
	main_add_accessor(rev, 0, 0, 4, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 1, 0, 4, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 0, 4, 5, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 1, 4, 5, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	strcpy(n, "R-Room");
	MAIN_POTAR_TAB[4]->set_name(n);
	MAIN_POTAR_TAB[4]->set_plage(0, 90);
	strcpy(n, "R-Feed");
	MAIN_POTAR_TAB[5]->set_name(n);
	MAIN_POTAR_TAB[5]->set_plage(0, 90);

	main_add_connection(delay, 0, 0, -1, 0, 0);
	main_add_connection(delay, 1, 0, -1, 0, 1);
	*/
	
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
		*	Pause main loop for 50ms ( 20Hz )
		*/
		usleep(50000);
	}
}