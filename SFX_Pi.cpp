#include "main.h"

const string VERSION = "1.1";

static Module_Node_List *MAIN_LIST_MODULE;
//static int MAIN_COUNT_MODULE = 0;

//static int MAIN_POTAR_VALUES[SPI_POTAR_COUNT] = {};

int last_vol = 0;

/*
*	---------------------------------------------------------------------------
*	Main programm function
*	---------------------------------------------------------------------------
*/
int main(int argc, char *argv[]){
	
	cout << "Bienvenu dans le fantastique et magnifique software Space-FX" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "VERSION:" << VERSION << " -- Run:45" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
    
    IOS::init_screen( string("Space-Fx"), string("1-45") );
    
	IO_Potentiometer MAIN_POTAR_TAB[SPI_POTAR_COUNT];
    
	io_init_spi();
    
	io_init_potar_tab(MAIN_POTAR_TAB);
    
    MAIN_LIST_MODULE = new Module_Node_List();
    
    io_init_frontPanel(MAIN_LIST_MODULE, VERSION);
    
    cout << endl;
    cout << "---------------------Base Setup OK--------------------------" << endl;
    cout << endl;
    
    float drive11[17] = { 0, 1, 0, 0, 5.51f, 1, 6, 0.35f, 0, 0, 0, 0, 350, 1500, 0.5f, 3.4f , 2.2f };
    float drive12[17] = { 0, 1, 0, 1, 198, 1, 22, 0.29f, 63, 1, 15, 0.1f, 200, 1500, 0.5f, 2.0f, 3.0f };
    float drive13[17] = { 0, 1, 0, 0, 43.1f, 1, 20, 0.23f, 0, 0, 0, 0, 350, 750, 17.12f, 5.6f, 0.56f };
    MAIN_LIST_MODULE->add_module( MOD_DRIVE , 1 );
    MAIN_LIST_MODULE->get(1)->get_module()->set_param(17, drive11);
    MAIN_LIST_MODULE->get(1)->get_module()->add_bank(17, drive12);
    MAIN_LIST_MODULE->get(1)->get_module()->add_bank(17, drive13);
    
    float drive21[17] = { 0, 1, 0, 1, 6.32f, 1, 7, 0.6f, 7.02, 1, 2, 0.8f, 350, 1500, 0.5f, 3.4f , 2.2f };
    float drive22[17] = { 0, 1, 0, 1, 100, 0, 0, 0, 177, 1, 30, 0.2f, 200, 1500, 0.7f, 1.41f, 2.5f };
    float drive23[17] = { 0, 1, 0, 0, 60.1f, 0, 0, 0, 0, 0, 0, 0, 350, 750, 14.6f, 5.6f, 0.97f };
    MAIN_LIST_MODULE->add_module( MOD_DRIVE , 2 );
    MAIN_LIST_MODULE->get(2)->get_module()->set_param(17, drive21);
    MAIN_LIST_MODULE->get(2)->get_module()->add_bank(17, drive22);
    MAIN_LIST_MODULE->get(2)->get_module()->add_bank(17, drive23);
    
    float chorus11[10] = { 0, 1, 0.0115f, 0.9f, 2, 5, 15, 15, 18, 23 };
    float chorus12[10] = { 0, 1, 0.018f, 0.9f, 2, 5, 8, 15, 18, 23 };
    float chorus13[10] = { 0, 1, 0.02f, 0.7f, 2, 10, 20, 15, 18, 23 };
    MAIN_LIST_MODULE->add_module( MOD_CHORUS , 3 );
    MAIN_LIST_MODULE->get(3)->get_module()->set_param(10, chorus11);
    MAIN_LIST_MODULE->get(3)->get_module()->add_bank(10, chorus12);
    MAIN_LIST_MODULE->get(3)->get_module()->add_bank(10, chorus13);
    
    float chorus21[10] = { 0, 1, -0.0115f, 0.9f, 2, 8, 10, 13, 18, 23 };
    float chorus22[10] = { 0, 1, -0.018f, 0.9f, 2, 5, 8, 15, 18, 23 };
    float chorus23[10] = { 0, 1, -0.02f, 0.7f, 2, 20, 25, 15, 18, 23 };
    MAIN_LIST_MODULE->add_module( MOD_CHORUS , 4 );
    MAIN_LIST_MODULE->get(4)->get_module()->set_param(10, chorus21);
    MAIN_LIST_MODULE->get(4)->get_module()->add_bank(10, chorus22);
    MAIN_LIST_MODULE->get(4)->get_module()->add_bank(10, chorus23);
    
    float lfo11[] = { 0, 1, 0, 3.5f, 0, 1, 0, 0, 0.5f, 0};
    float lfo12[] = { 0, 1, 0, 5.9f, 0, 1, 0, 0, 0.5f, 0};
    float lfo13[] = { 0, 1, 0, 0.9f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE->add_module( MOD_LFO , 5 );
    MAIN_LIST_MODULE->get(5)->get_module()->set_param(10, lfo11);
    MAIN_LIST_MODULE->get(5)->get_module()->add_bank(10, lfo12);
    MAIN_LIST_MODULE->get(5)->get_module()->add_bank(10, lfo13);
    
    float lfo21[] = { 0, 1, 2, 2.0f, 0, 1, 0, 0, 0.5f, 0};
    float lfo22[] = { 0, 1, 0, 3.5f, 0, 1, 0, 0, 0.5f, 0};
    float lfo23[] = { 0, 1, 2, 1.1f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE->add_module( MOD_LFO , 6 );
    MAIN_LIST_MODULE->get(6)->get_module()->set_param(10, lfo21);
    MAIN_LIST_MODULE->get(6)->get_module()->add_bank(10, lfo22);
    MAIN_LIST_MODULE->get(6)->get_module()->add_bank(10, lfo23);
    
    float tone11[7] = { 0, 0.70f, 150, 1500, 0.5f, 2.1f, 4.1f };
    float tone12[7] = { 0, 0.15f, 150, 5000, 0.7f, 1.9f, 0.5f };
    float tone13[7] = { 0, 0.15f, 100, 350, 4, 2.79f, 0.74f };
    MAIN_LIST_MODULE->add_module( MOD_TONE , 10 );
    MAIN_LIST_MODULE->get(10)->get_module()->set_param(7, tone11);
    MAIN_LIST_MODULE->get(10)->get_module()->add_bank(7, tone12);
    MAIN_LIST_MODULE->get(10)->get_module()->add_bank(7, tone13);
    
    float tone21[7] = { 0, 0.70f, 150, 1500, 0.5f, 2.1f, 4.1f };
    float tone22[7] = { 0, 0.15f, 150, 3000, 0.7f, 1.9f, 0.5f };
    float tone23[7] = { 0, 0.15f, 100, 350, 4, 2.19f, 1.3f };
    MAIN_LIST_MODULE->add_module( MOD_TONE , 11 );
    MAIN_LIST_MODULE->get(11)->get_module()->set_param(7, tone21);
    MAIN_LIST_MODULE->get(11)->get_module()->add_bank(7, tone22);
    MAIN_LIST_MODULE->get(11)->get_module()->add_bank(7, tone23);
    
    float delay11[5] = { 0, 1, 641, 0.65f, 0.4f};
    float delay12[5] = { 0, 1, 641, 0.55f, 0.3f};
    float delay13[5] = { 1, 1, 641, 0.65f, 0};
    MAIN_LIST_MODULE->add_module( MOD_DELAY , 12 );
    MAIN_LIST_MODULE->get(12)->get_module()->set_param(5, delay11);
    MAIN_LIST_MODULE->get(12)->get_module()->add_bank(5, delay12);
    MAIN_LIST_MODULE->get(12)->get_module()->add_bank(5, delay13);
    
    float delay21[5] = { 0, 1, 427, 0.65f, 0.4f};
    float delay22[5] = { 0, 1, 427, 0.55f, 0.3f};
    float delay23[5] = { 1, 1, 427, 0.65f, 0};
    MAIN_LIST_MODULE->add_module( MOD_DELAY , 13 );
    MAIN_LIST_MODULE->get(13)->get_module()->set_param(5, delay21);
    MAIN_LIST_MODULE->get(13)->get_module()->add_bank(5, delay22);
    MAIN_LIST_MODULE->get(13)->get_module()->add_bank(5, delay23);
    
    
    MAIN_LIST_MODULE->add_connection(12, 1,  12, 1);
    MAIN_LIST_MODULE->add_connection(13, 1,  13, 1);
    
    MAIN_LIST_MODULE->add_connection(BEGIN_NODE, 0,  1, 0);
    MAIN_LIST_MODULE->add_connection(BEGIN_NODE, 0,  2, 0);
    
    MAIN_LIST_MODULE->add_connection( 1, 0,  3, 0);
    MAIN_LIST_MODULE->add_connection( 2, 0,  4, 0);
    
    MAIN_LIST_MODULE->add_connection( 3, 0, 10, 0);
    MAIN_LIST_MODULE->add_connection( 4, 0, 11, 0);
    
    MAIN_LIST_MODULE->add_connection( 9, 0,  3, 1);
    MAIN_LIST_MODULE->add_connection( 9, 0,  4, 1);
    
    MAIN_LIST_MODULE->add_connection( 4, 0,  3, 2);
    MAIN_LIST_MODULE->add_connection( 4, 0,  4, 2);
    
    MAIN_LIST_MODULE->add_connection( 6, 0,  3, 3);
    MAIN_LIST_MODULE->add_connection( 6, 0,  4, 3);
    
    MAIN_LIST_MODULE->add_connection( 7, 0,  3, 4);
    MAIN_LIST_MODULE->add_connection( 7, 0,  4, 4);
    
    MAIN_LIST_MODULE->add_connection( 8, 0,  3, 5);
    MAIN_LIST_MODULE->add_connection( 8, 0,  4, 5);
    
    MAIN_LIST_MODULE->add_connection(10, 0, 12, 0);
    MAIN_LIST_MODULE->add_connection(11, 0, 13, 0);
    
    MAIN_LIST_MODULE->add_connection(12, 0, END_NODE, 0);
    MAIN_LIST_MODULE->add_connection(13, 0, END_NODE, 1);
    
    save_preset( string("DefaultPreset"), VERSION, MAIN_LIST_MODULE, MAIN_POTAR_TAB );
    
    for ( int i = 0 ; i <  MAIN_LIST_MODULE->list_.size() ; i++ ){
        
        MAIN_LIST_MODULE->list_[i]->get_module()->set_bank(0);
    }
    
	load_preset( string("DefaultPreset"), VERSION, &MAIN_LIST_MODULE, MAIN_POTAR_TAB );
    save_preset( string("DefaultPresetCpy"), VERSION, MAIN_LIST_MODULE, MAIN_POTAR_TAB );
	
	/*
	*	---------------------Main Loop-------------------------------
	*/
	while(1){
	
		/*
		*	Read potar values and store values in MAIN_POTAR_VALUES
		*/
		io_update_potar_tab(MAIN_POTAR_TAB, MAIN_LIST_MODULE);
        
        /*
        *   Check front pannel buttons
        */
        //io_update_frontPanel();
		
        
        /*
        * Check if input is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE->begin_.get_module())->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Input");
            IOS::printm( string(n), IOS::DEFAULT_TEMP , IOS::TEMP);
            static_cast<EndModule*>(MAIN_LIST_MODULE->begin_.get_module())->reset_saturated();
        }
        
        /*
        * Check if output is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE->end_.get_module())->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Output");
            IOS::printm( string(n), IOS::DEFAULT_TEMP , IOS::TEMP );
            static_cast<EndModule*>(MAIN_LIST_MODULE->end_.get_module())->reset_saturated();
        }
		
		/*
		*	Pause main loop for 50ms ( 20Hz )
		*/
		usleep(50000);
	}
    
    exit(0);
}