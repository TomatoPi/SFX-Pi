#include "Menu.h"

using namespace MENU;

void menu_init_main_menu(){
    
    // Prevent menu malfunctions
    MAIN_MENU.clear();
    
    // Set MainMenu node as root node
    MAIN_MENU.add( NODE_ROOT, MAIN_MENU );
    
    MenuIterator init;
    
    // Add ChangePresset submenu
    init = menu_init_change_presset( MAIN_MENU.root() );
    
    // Add SelectModule after ChangePresset
    init = menu_init_select_module( init );
    
}
