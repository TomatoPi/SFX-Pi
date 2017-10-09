#include "main.h"

using namespace std;
using namespace PROG_CONST;

/**
 * Store Current program version.
 * Used by presset save and loading functions for prevent corruption or
 * bad files loads
 */
//string PROG_CONST::PROG_VERSION = "1.1";
/**
 * Store current program run.
 * Only used by splash screen
 */
//string PROG_CONST::PROG_RUN = "1-53";
/**
 * Splash screen send to screen at program boot
 */
//string PROG_CONST::PROG_SPLASH_SCREEN = "Space-Fx";
/**
 * JACK's server name
 */
//string PROG_CONST::PROG_JACK_SERVER = "Space_Fx";
/**
 * Path to presset folder
 */
//string PROG_CONST::PATH_PRESSET = "/home/sfx_pi/sfx/Files/";

static Module_Node_List *MAIN_LIST_MODULE;

int last_vol = 0;

/*
*	---------------------------------------------------------------------------
*	Main programm function
*	---------------------------------------------------------------------------
*/
int main(int argc, char *argv[]){
	
	cout << "Bienvenu dans le fantastique et magnifique software Space-FX" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "VERSION:" << PROG_VERSION << " -- Run:" << PROG_RUN << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
    
    IOS::init_screen();
    
    MAIN_LIST_MODULE = new Module_Node_List();
    
    io_init_frontPanel();
    
    cout << endl;
    cout << "---------------------Base Setup OK--------------------------" << endl;
    cout << endl;

    /*
    cout << "Test Linked list" << endl;

    LinkedList<string> test;
    cout << "List size : " << test.size() << endl;
    test.push_back( string("Test1") );
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.push_back( string("Test2") );
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.push_back( string("Test3") );
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.push_front( string("Test4") );
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    cout << "List First : " << test.first() << endl;
    test.remove(3);
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.remove(2);
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.remove(1);
    cout << "List size : " << test.size() << endl;
    cout << "List Last : " << test.last() << endl;
    test.remove(0);
    cout << "List size : " << test.size() << endl;
    test.push_front( string("Test4") );
    test.push_back( string("Test3") );
    test.push_back( string("Test2") );
    test.push_back( string("Test1") );
    cout << "List size : " << test.size() << endl;
    test.clear();
    cout << "List size : " << test.size() << endl;

    cout << "Linked List is Ok :)" << endl;

    cout << endl << "Test Tree Structure" << endl;
    Tree<string, int> tree;
    Tree<string, int>::Iterator itr = tree.root();
    cout << "Tree Size : " << tree.size() << endl;
    
    tree.add( string("NodeA"), 0 );
    itr = tree.root();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    tree.add( string("NodeB"), 1 );
    itr.next();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    tree.add( string("NodeC"), 2 );
    itr.next();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    itr.next();
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    itr.next();
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    itr.next();
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    tree.add( string("NodeAA"), 3, tree.root(), true );
    itr = tree.root();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    itr.child();
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    tree.add( string("NodeAB"), 4, itr );
    itr.next();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;
    
    tree.add( string("NodeAC"), 5, itr );
    itr.next();
    cout << "Tree Size : " << tree.size() << endl;
    cout << "Iterator Pos : " << string( itr.get() ) << endl;

    Tree<string, int>::Iterator src = tree.get( 1, 2 );
    if ( src.ok() ){

        cout << "Node Founded : " << src.get() << endl;
    }
    src = tree.remove( src );
    cout << "Tree Size : " << tree.size() << endl;

    tree.remove_branch( tree.root().child() );
    cout << "Tree Size : " << tree.size() << endl;

    tree.clear();
    cout << "Tree Size : " << tree.size() << endl;

    cout << endl << "Tree Test OK" << endl;
    */
    
    float drive11[18] = { 0, 1, 0, 0, 0, 5.51f, 1, 6, 0.35f, 0, 0, 0, 0, 350, 1500, 0.5f, 3.4f , 2.2f };
    float drive12[18] = { 0, 1, 0, 0, 1, 198, 1, 22, 0.29f, 63, 1, 15, 0.1f, 200, 1500, 0.5f, 2.0f, 3.0f };
    float drive13[18] = { 0, 1, 0, 0, 0, 43.1f, 1, 20, 0.23f, 0, 0, 0, 0, 350, 750, 17.12f, 5.6f, 0.56f };
    MAIN_LIST_MODULE->add_module( MOD_DRIVE , 1 );
    MAIN_LIST_MODULE->get(1)->set_param(18, drive11);
    MAIN_LIST_MODULE->get(1)->add_bank(18, drive12);
    MAIN_LIST_MODULE->get(1)->add_bank(18, drive13);
    
    float drive21[18] = { 0, 1, 0, 0, 1, 6.32f, 1, 7, 0.6f, 7.02, 1, 2, 0.8f, 350, 1500, 0.5f, 3.4f , 2.2f };
    float drive22[18] = { 0, 1, 0, 0, 1, 100, 0, 0, 0, 177, 1, 30, 0.2f, 200, 1500, 0.7f, 1.41f, 2.5f };
    float drive23[18] = { 0, 1, 0, 0, 0, 60.1f, 0, 0, 0, 0, 0, 0, 0, 350, 750, 14.6f, 5.6f, 0.97f };
    MAIN_LIST_MODULE->add_module( MOD_DRIVE , 2 );
    MAIN_LIST_MODULE->get(2)->set_param(18, drive21);
    MAIN_LIST_MODULE->get(2)->add_bank(18, drive22);
    MAIN_LIST_MODULE->get(2)->add_bank(18, drive23);
    
    float chorus11[15] = { 0, 1, 0.0115f, 0.9f, 2, 10, 15, 15, 18, 23, 1, 1, 1, 1, 1 };
    float chorus12[15] = { 0, 1, 0.018f, 0.9f, 2, 10, 18, 15, 18, 23, 1, 1, 1, 1, 1  };
    float chorus13[15] = { 0, 1, 0.02f, 0.7f, 2, 10, 20, 15, 18, 23, 1, 1, 1, 1, 1  };
    MAIN_LIST_MODULE->add_module( MOD_CHORUS , 3 );
    MAIN_LIST_MODULE->get(3)->set_param(10, chorus11);
    MAIN_LIST_MODULE->get(3)->add_bank(10, chorus12);
    MAIN_LIST_MODULE->get(3)->add_bank(10, chorus13);
    
    float chorus21[15] = { 0, 1, -0.0115f, 0.9f, 2, 10, 18, 13, 18, 23, 1, 1, 1, 1, 1  };
    float chorus22[15] = { 0, 1, -0.018f, 0.9f, 2, 10, 13, 15, 18, 23, 1, 1, 1, 1, 1  };
    float chorus23[15] = { 0, 1, -0.02f, 0.7f, 2, 20, 25, 15, 18, 23, 1, 1, 1, 1, 1  };
    MAIN_LIST_MODULE->add_module( MOD_CHORUS , 4 );
    MAIN_LIST_MODULE->get(4)->set_param(10, chorus21);
    MAIN_LIST_MODULE->get(4)->add_bank(10, chorus22);
    MAIN_LIST_MODULE->get(4)->add_bank(10, chorus23);
    
    float lfo11[] = { 0, 1, 0, 3.5f, 0, 1, 0, 0, 0.5f, 0};
    float lfo12[] = { 0, 1, 0, 5.9f, 0, 1, 0, 0, 0.5f, 0};
    float lfo13[] = { 0, 1, 0, 0.8f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE->add_module( MOD_LFO , 5 );
    MAIN_LIST_MODULE->get(5)->set_param(10, lfo11);
    MAIN_LIST_MODULE->get(5)->add_bank(10, lfo12);
    MAIN_LIST_MODULE->get(5)->add_bank(10, lfo13);
    
    float lfo21[] = { 0, 1, 2, 2.0f, 0, 1, 0, 0, 0.5f, 0};
    float lfo22[] = { 0, 1, 0, 3.5f, 0, 1, 0, 0, 0.5f, 0};
    float lfo23[] = { 0, 1, 2, 0.6f, 0, 1, 0, 0, 0.5f, 0};
    MAIN_LIST_MODULE->add_module( MOD_LFO , 6 );
    MAIN_LIST_MODULE->get(6)->set_param(10, lfo21);
    MAIN_LIST_MODULE->get(6)->add_bank(10, lfo22);
    MAIN_LIST_MODULE->get(6)->add_bank(10, lfo23);
    
    float tone11[7] = { 0, 0.70f, 150, 1500, 0.5f, 2.1f, 4.1f };
    float tone12[7] = { 0, 0.15f, 150, 5000, 0.7f, 1.9f, 0.5f };
    float tone13[7] = { 0, 0.15f, 100, 350, 4, 2.79f, 0.74f };
    MAIN_LIST_MODULE->add_module( MOD_TONE , 10 );
    MAIN_LIST_MODULE->get(10)->set_param(7, tone11);
    MAIN_LIST_MODULE->get(10)->add_bank(7, tone12);
    MAIN_LIST_MODULE->get(10)->add_bank(7, tone13);
    
    float tone21[7] = { 0, 0.70f, 150, 1500, 0.5f, 2.1f, 4.1f };
    float tone22[7] = { 0, 0.15f, 150, 3000, 0.7f, 1.9f, 0.5f };
    float tone23[7] = { 0, 0.15f, 100, 350, 4, 2.19f, 1.3f };
    MAIN_LIST_MODULE->add_module( MOD_TONE , 11 );
    MAIN_LIST_MODULE->get(11)->set_param(7, tone21);
    MAIN_LIST_MODULE->get(11)->add_bank(7, tone22);
    MAIN_LIST_MODULE->get(11)->add_bank(7, tone23);
    
    float delay11[5] = { 0, 1, 641, 0.65f, 0.4f};
    float delay12[5] = { 0, 1, 641, 0.55f, 0.3f};
    float delay13[5] = { 1, 1, 641, 0.65f, 0};
    MAIN_LIST_MODULE->add_module( MOD_DELAY , 12 );
    MAIN_LIST_MODULE->get(12)->set_param(5, delay11);
    MAIN_LIST_MODULE->get(12)->add_bank(5, delay12);
    MAIN_LIST_MODULE->get(12)->add_bank(5, delay13);
    
    float delay21[5] = { 0, 1, 427, 0.65f, 0.4f};
    float delay22[5] = { 0, 1, 427, 0.55f, 0.3f};
    float delay23[5] = { 1, 1, 427, 0.65f, 0};
    MAIN_LIST_MODULE->add_module( MOD_DELAY , 13 );
    MAIN_LIST_MODULE->get(13)->set_param(5, delay21);
    MAIN_LIST_MODULE->get(13)->add_bank(5, delay22);
    MAIN_LIST_MODULE->get(13)->add_bank(5, delay23);
    
    
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
    
    
    
    //save_preset( string("DefaultPreset"), VERSION, MAIN_LIST_MODULE, MAIN_POTAR_TAB );
    
    for ( int i = 0 ; i <  MAIN_LIST_MODULE->list_.size() ; i++ ){
        
        MAIN_LIST_MODULE->list_[i]->set_bank(0);
    }
    
	//load_preset( string("DefaultPreset"), VERSION, &MAIN_LIST_MODULE, MAIN_POTAR_TAB );
    //save_preset( string("DefaultPresetCpy"), VERSION, MAIN_LIST_MODULE, MAIN_POTAR_TAB );
    
	
	/*
	*	---------------------Main Loop-------------------------------
	*/
	while(1){
	
		/*
		*	Read potar values and store values in MAIN_POTAR_VALUES
		*/
		//io_update_potar_tab(MAIN_POTAR_TAB, MAIN_LIST_MODULE);
        
        /*
        *   Update front pannel stuff
        */
        io_update_frontPanel( MAIN_LIST_MODULE );
		
        
        /*
        * Check if input is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE->begin_)->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Input");
            IOS::printm( string(n), IOS::DEFAULT_TEMP , IOS::TEMP);
            static_cast<EndModule*>(MAIN_LIST_MODULE->begin_)->reset_saturated();
        }
        
        /*
        * Check if output is saturated
        */
        if ( static_cast<EndModule*>(MAIN_LIST_MODULE->end_)->is_saturated() ){
            
            char n[64];
            sprintf( n, "Wrn Output");
            IOS::printm( string(n), IOS::DEFAULT_TEMP , IOS::TEMP );
            static_cast<EndModule*>(MAIN_LIST_MODULE->end_)->reset_saturated();
        }
		
		/*
		*	Pause main loop for 50ms ( 20Hz )
		*/
		usleep(50000);
	}
    
    exit(0);
}
