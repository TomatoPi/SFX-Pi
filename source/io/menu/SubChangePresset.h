/**
 * File Containing ChangePresset submenu's functions and stuff
 */

/**
 * Function called once at Menu initialisation for add this Submenu
 * It add new submenu as child of given Iterator
 * and return an iterator on Added submenu highest node
 */
MenuIterator menu_init_change_presset( MenuIterator pos );

/**
 * Function called for enter inside change presset submenu
 */
void main_change_presset_enter();

/**
 * Function called when inside change presset submenu
 */
void main_change_presset_do( MENU::Move_flag );

/**
 * Menu tree object for this submenu
 */
MENU::Submenu NODE_MAIN_CHANGE_PRESSET = { main_change_presset_enter, 
                                            main_change_presset_do };

