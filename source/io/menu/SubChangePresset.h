/**
 * File Containing ChangePresset submenu's functions and stuff
 */

#ifndef DEF_SUB_CHANGE_PRESSET_H
#define DEF_SUB_CHANGE_PRESSET_H

#include "../../consts.h"

#include "../../core/Tree/Tree.h"
#include "../../core/Modules.h"
#include "../../presset/Presset.h"

#include "../io.h"

#include "Declarations.h"

/**
 * Function called once at Menu initialisation for add this Submenu
 * It add new submenu as child of given Iterator
 * and return an iterator on Added submenu highest node
 */
MenuIterator menu_init_change_presset( MenuTree *menu, MenuIterator pos );

/**
 * Function called for enter inside change presset submenu
 */
void main_change_presset_enter( Module_Node_List* );

/**
 * Function called when inside change presset submenu
 */
MenuIterator main_change_presset_do( Move_flag, MenuIterator, Module_Node_List* &, IO_Potentiometer[SPI_POTAR_COUNT] );

#endif
