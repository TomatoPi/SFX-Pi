#include "SubAddModule.h"

/**********************************************************************
 *  Branch Nodes
 **********************************************************************/

Submenu NODE_MAIN_ADD_MODULE = { main_add_module_enter, 
                                            main_add_module_do };
                                            
Submenu NODE_ADDMOD_CHOOSE_MODTYPE = { addmod_choose_type_enter, 
                                            addmod_choose_type_do };

/**********************************************************************
 *  Init
 **********************************************************************/

MenuIterator menu_init_add_module( MenuTree *menu, MenuIterator pos ){

    pos = menu->add( NODE_MAIN_ADD_MODULE, MAIN_ADD_MODULE, pos );

    MenuIterator init = pos;

    pos = menu->add( NODE_ADDMOD_CHOOSE_MODTYPE, ADDMOD_CHOOSE_MODTYPE,
                            pos, true );
    
    return init;
}

/**********************************************************************
 * Nodes
 **********************************************************************/
void main_add_module_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Add Module" << endl;
}

MenuIterator main_add_module_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_UP ){
        
        itr.prev();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_DOWN ){
        
        itr.next();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ENTER ){

        itr.child();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}

/**********************************************************************/
void addmod_choose_type_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Choose Type" << endl;
}

MenuIterator addmod_choose_type_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}
