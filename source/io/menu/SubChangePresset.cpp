#include "SubChangePresset.h"

using namespace PROG_CONST;
using namespace std;

/**********************************************************************
 * Stuff for this Submenu
 **********************************************************************/
Submenu NODE_MAIN_CHANGE_PRESSET = { main_change_presset_enter, 
                                            main_change_presset_do };
                                            
namespace{
    
    vector<string> presset_list;
    vector<string>::iterator current_presset;
    
    string loaded_preset_name = "";
}

/**
 * Function called when move outside this submenu
 */
void func_exit_change_presset();

/**
 * Function called for reload presset list
 * @return true if list has been reloaded
 */
bool func_reload_presset_list();

/**
 * Function called for load current presset
 */
void func_load_preset( Module_Node_List* &, IO_Potentiometer[SPI_POTAR_COUNT] );

/**********************************************************************
 * Functions
 **********************************************************************/
MenuIterator menu_init_change_presset( MenuTree *menu, MenuIterator pos ){
    
    // Add this submenu to main menu, as child of given position
    pos = menu->add( NODE_MAIN_CHANGE_PRESSET, MAIN_CHANGE_PRESSET
                    , pos ,true );
                    
    // Load presset list
    func_reload_presset_list();

    return pos;
}

void main_change_presset_enter( Module_Node_List* graph ){

    cout << "Menu : Enter Change Presset" << endl;
    // If presset list is empty try reload it
    if ( presset_list.size() == 0 ){
        if ( !func_reload_presset_list() ){
            IOS::printm( "ErrNoPreset", IOS::OVERIDE );
        }
    }
    
    if ( presset_list.size() != 0 ){
        
        IOS::printm( loaded_preset_name, IOS::OVERIDE );
        IOS::printm( "SelPresset", IOS::DEFAULT_TEMP, IOS::TEMP );
    }
}

MenuIterator main_change_presset_do( Move_flag action, MenuIterator itr, Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){

    /*
     * Compute given action
     */
    if ( action == MOVE_UP ){
        
        func_exit_change_presset();
        itr.prev();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_DOWN ){
        
        func_exit_change_presset();
        itr.next();
        (*itr.get().enter_)( graph );
    }
    else if ( action == MOVE_NEXT ){
        
        // If presset list is empty try reload it
        if ( presset_list.size() == 0 ){
            if ( !func_reload_presset_list() ){
                IOS::printm( "ErrNoPreset", IOS::DEFAULT_TEMP, IOS::TEMP );
            }
        }
        // If list is not empty navigate throught it
        if ( presset_list.size() != 0 ){
            
            // Iterate position inside list or go back to begin
            if ( current_presset == presset_list.end() ){
                
                current_presset = presset_list.begin();
            }
            else if ( ++current_presset == presset_list.end() ){
                
                current_presset = presset_list.begin();
            }

            IOS::printm( (*current_presset), IOS::OVERIDE );
        }
    }
    else if ( action == MOVE_PREV ){
        
        // If presset list is empty try reload it
        if ( presset_list.size() == 0 ){
            if ( !func_reload_presset_list() ){
                IOS::printm( "ErrNoPreset", IOS::DEFAULT_TEMP, IOS::TEMP );
            }
        }
        // If list is not empty navigate throught it
        if ( presset_list.size() != 0 ){
            
            // Iterate position inside list or go back to begin
            if ( current_presset == presset_list.begin() ){
                
                current_presset == presset_list.end() - 1;
            }
            else{
                
                --current_presset;
            }
            
            IOS::printm( (*current_presset), IOS::OVERIDE );
        }
    }
    else if ( action == MOVE_ADD ){

        if ( presset_list.size() != 0 ){
            
            if ( save_preset( (*current_presset),
                        graph, pot) )
            {
                IOS::printm( "ErrSavePrst", IOS::DEFAULT_TEMP, IOS::TEMP );
            }
            else{
                IOS::printm( "SaveSuccess", IOS::DEFAULT_TEMP, IOS::TEMP );
            }
        }
    }
    else if ( action == MOVE_DEL ){
        
    }
    else if ( action == MOVE_ENTER ){
        
        if ( func_reload_presset_list() ){
            
            IOS::printm( (*current_presset), IOS::OVERIDE );
            IOS::printm( "ListReloadd", IOS::DEFAULT_TEMP, IOS::TEMP );
        }
        else{
            IOS::printm( "ErrNoPreset", IOS::OVERIDE );
        }
    }
    else if ( action == MOVE_OK ){
        
        // If presset list is empty try reload it
        if ( presset_list.size() == 0 ){
            if ( !func_reload_presset_list() ){
                IOS::printm( "ErrNoPreset", IOS::DEFAULT_TEMP, IOS::TEMP );
            }
        }
        // If list is not empty navigate throught it
        if ( presset_list.size() != 0 ){
            
            // Then try load presset
            func_load_preset( graph, pot );
        }
    }
    else if ( action == MOVE_ESC ){

        itr.parent();
        (*itr.get().enter_)( graph );
    }
    return itr;
}

void func_exit_change_presset(){
    
}


bool func_reload_presset_list(){
    
    /*
     * Try reload presset list
     * Cancel operation if returned empty list or if directory is not valid
     */
    if ( list_files( PATH_PRESSET, presset_list ) ){
        
        cout << "Error Loading Presset list" << endl;
        current_presset = presset_list.begin();
        
        return false;
    }
    
    current_presset = presset_list.begin();
    
    return true;
}

void func_load_preset( Module_Node_List* & graph, IO_Potentiometer pot[SPI_POTAR_COUNT] ){
    
    if ( load_preset( (*current_presset),
                        graph, pot) )
    {
        IOS::printm( "ErrLoadPrst", IOS::DEFAULT_TEMP, IOS::TEMP );
    }
    else{
        IOS::printm( "LoadSuccess", IOS::DEFAULT_TEMP, IOS::TEMP );
        loaded_preset_name = (*current_presset);
    }
}
