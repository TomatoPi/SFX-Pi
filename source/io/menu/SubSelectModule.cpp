#include "SubSelectModule.h"

/**********************************************************************
 * Branch Nodes
 **********************************************************************/
Submenu NODE_MAIN_SELECT_MODULE = { main_select_module_enter, 
                                            main_select_module_do };
/**
 * Nodes For Module Edition Branch
 */
Submenu NODE_SELMOD_CHANGE_BANK = { selmod_change_bank_enter, 
                                            selmod_change_bank_do };

Submenu NODE_SELMOD_ADD_BANK = { selmod_add_bank_enter, 
                                            selmod_add_bank_do };

Submenu NODE_SELMOD_EDIT_BANK = { selmod_edit_bank_enter, 
                                            selmod_edit_bank_do };
/**
 * Nodes For Param Edition Branch
 */
Submenu NODE_SELMOD_EDITB_SELPAR = { selmod_editb_selpar_enter, 
                                            selmod_editb_selpar_do };

Submenu NODE_SELMOD_EDITB_EDITPAR = { selmod_editb_editpar_enter, 
                                            selmod_editb_editpar_do };
                                            
/**********************************************************************
 * Stuff for this submenu
 **********************************************************************/
namespace{

	// Files names for backup during graph's edition
	const string BACK_SELECT_MODULE = "tmp/tmp_sel_mod";
	const string BACK_EDIT_BANK     = "tmp/tmp_edit_bank";
	const string BACK_EDIT_PARAM    = "tmp/tmp_edit_param";
    
    int current_id = -1;
    Module_iterator current_module;
    
}
void func_exit_select_module();
/**********************************************************************
 * Functions
 **********************************************************************/
MenuIterator menu_init_select_module( MenuTree *menu, MenuIterator pos ){

    // Add Select Module's branch root node
    pos = menu->add( NODE_MAIN_SELECT_MODULE, MAIN_SELECT_MODULE, pos );

    MenuIterator init = pos;

    /*
     * Module's edition branch
     */
    pos = menu->add( NODE_SELMOD_CHANGE_BANK, SELMOD_CHANGE_BANK,
                            pos , true);

    pos = menu->add( NODE_SELMOD_ADD_BANK, SELMOD_ADD_BANK,
                            pos );

    pos = menu->add( NODE_SELMOD_EDIT_BANK, SELMOD_EDIT_BANK,
                            pos );

    /*
     * Param edition branch
     */
    pos = menu->add( NODE_SELMOD_EDITB_SELPAR, SELMOD_EDITB_SELPAR,
                            pos, true );

    pos = menu->add( NODE_SELMOD_EDITB_EDITPAR, SELMOD_EDITB_EDITPAR,
                            pos, true );

    return init;
}

void func_exit_select_module(){
        
    current_id = -1;
}
/**********************************************************************
 *      Main node
 **********************************************************************/

void main_select_module_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Select Module" << endl;
    current_id = -1;
    current_module = graph->end();
}

MenuIterator main_select_module_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_UP ){
        
        itr.prev();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_DOWN ){
        
        itr.next();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_NEXT ){
        
        // Increment current module
    }
    else if ( action == MOVE_PREV ){
        
        //Decrement Current module
    }
    else if ( action == MOVE_ENTER ){

        // If current module is selected enter, else do nothing
        itr.child();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ESC ){

        // Clear curent positions inside menu;
        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}


/**********************************************************************
 * Module's edition Branch
 **********************************************************************/

/**
 * Change Current Bank
 */
void selmod_change_bank_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Select Bank" << endl;
}

MenuIterator selmod_change_bank_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_UP ){
        
        itr.prev();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_DOWN ){
        
        itr.next();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}

/**
 * Add new Bank
 */
void selmod_add_bank_enter( Module_Node_List* graph ){

     cout << "Menu : Enter Add Bank" << endl;
}

MenuIterator selmod_add_bank_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_UP ){
        
        itr.prev();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_DOWN ){
        
        itr.next();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}

/**
 * Edit Current Bank
 */
void selmod_edit_bank_enter( Module_Node_List* graph ){

     cout << "Menu : Enter Edit Bank" << endl;
}

MenuIterator selmod_edit_bank_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

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

/**********************************************************************
 * Param Edition Branch
 **********************************************************************/

/**
 * Select Current param
 */
void selmod_editb_selpar_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Select Param" << endl;
}

MenuIterator selmod_editb_selpar_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_ENTER ){

        itr.child();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}

/**
 * Modify Choosen Param
 */
void selmod_editb_editpar_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Edit Param" << endl;
}

MenuIterator selmod_editb_editpar_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}
