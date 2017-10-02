#include "SubChangePresset.cpp"

using namespace MENU;

MenuIterator menu_init_change_presset( MenuIterator pos ){
    
    // Add this submenu to main menu, as child of given position
    MAIN_MENU.add( NODE_MAIN_CHANGE_PRESSET, MAIN_CHANGE_PRESSET
                    , pos ,true );
                    
    // Return Iterator on added node
    
    return MAIN_MENU.get( 2, MAIN_MENU, MAIN_CHANGE_PRESSET );
}

void main_change_presset_enter();

void main_change_presset_do( MENU::Move_flag );
