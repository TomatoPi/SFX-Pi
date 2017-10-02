/**
 * File Containing Select Module submenu's functions and stuff
 */

/**
 * Function called once at Menu initialisation for add this Submenu
 * It add new menu branch next after given Iterator
 * and return iterator on Added highest node
 */
MenuIterator menu_init_select_module( MenuIterator pos );

/**********************************************************************
 *      Main node
 **********************************************************************/
 
/** 
 * Function called when enter this submenu
 */
void main_select_module_enter();

/**
 * Function called when inside this submenu
 */
void main_select_module_do( MENU::Move_flag );

/**
 * Menu tree object for this submenu
 */
MENU::Submenu NODE_MAIN_SELECT_MODULE = { main_select_module_enter, 
                                            main_select_module_do };
                                    
/**********************************************************************
 * Module's edition Branch
 **********************************************************************/

/**
 * Change Current Bank
 */
void selmod_change_bank_enter();
void selmod_change_bank_do();

MENU::Submenu NODE_SELMOD_SELECT_MODULE = { selmod_change_bank_enter, 
                                            selmod_change_bank_do };

/**
 * Add new Bank
 */
void selmod_add_bank_enter();
void selmod_add_bank_do();

MENU::Submenu NODE_SELMOD_SELECT_MODULE = { selmod_add_bank_enter, 
                                            selmod_add_bank_do };

/**
 * Edit Current Bank
 */
void selmod_edit_bank_enter();
void selmod_edit_bank_do();

MENU::Submenu NODE_SELMOD_SELECT_MODULE = { selmod_edit_bank_enter, 
                                            selmod_edit_bank_do };
