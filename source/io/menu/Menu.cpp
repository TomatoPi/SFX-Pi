#include "Menu.h"

using namespace PROG_CONST;

Submenu NODE_ROOT = { main_menu_exit_menu, main_menu_do };

void menu_init_main_menu( MenuTree *menu ){

    // Iterator that will be used along menu construction
    MenuIterator init = menu->root();
    
    // Set MainMenu node as root node
    init = menu->add( NODE_ROOT, MAIN_MENU_FLAG );
    
    // Add ChangePresset submenu
    init = menu_init_change_presset( menu, init );
    
    // Add SelectModule after ChangePresset
    init = menu_init_select_module( menu, init );

    // Add AddModule after SelectModule
    init = menu_init_add_module( menu, init );
    
}

void main_menu_exit_menu( Module_Node_List* graph ){

    cout << "Menu : Exit Main Menu" << endl;
}

MenuIterator main_menu_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    // Press any button for enter inside menu
    if ( action != MOVE_ESC ){

        itr.child();
        (*itr.get().enter_)( graph );
    }
    return itr;
}
