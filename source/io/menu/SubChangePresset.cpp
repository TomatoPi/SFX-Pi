#include "SubChangePresset.h"

/**********************************************************************
 * Menu tree object for this submenu
 **********************************************************************/
Submenu NODE_MAIN_CHANGE_PRESSET = { main_change_presset_enter, 
                                            main_change_presset_do };

/**********************************************************************
 * Functions
 **********************************************************************/
MenuIterator menu_init_change_presset( MenuTree *menu, MenuIterator pos ){
    
    // Add this submenu to main menu, as child of given position
    pos = menu->add( NODE_MAIN_CHANGE_PRESSET, MAIN_CHANGE_PRESSET
                    , pos ,true );

    return pos;
}

void main_change_presset_enter( Module_Node_List* graph ){

}

MenuIterator main_change_presset_do( Move_flag action, MenuIterator itr, Module_Node_List* data ){

    return itr;
}
