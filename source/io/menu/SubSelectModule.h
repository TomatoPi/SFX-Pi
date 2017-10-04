/**
 * File Containing Select Module submenu's functions and stuff
 */
#ifndef DEF_SUB_SELECT_MODULE_H
#define DEF_SUB_SELECT_MODULE_H


#include "../../core/Tree/Tree.h"
#include "../../core/Modules.h"
#include "Declarations.h"

/**
 * Function called once at Menu initialisation for add this Submenu
 * It add new menu branch next after given Iterator
 * and return iterator on Added highest node
 */
MenuIterator menu_init_select_module( MenuTree *menu, MenuIterator pos );

/**********************************************************************
 *      Main node
 **********************************************************************/
 
/** 
 * Function called when enter this submenu
 */
void main_select_module_enter( Module_Node_List* );
MenuIterator main_select_module_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );
                                    
/**********************************************************************
 * Module's edition Branch
 **********************************************************************/

/**
 * Change Current Bank
 */
void selmod_change_bank_enter( Module_Node_List* );
MenuIterator selmod_change_bank_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );

/**
 * Add new Bank
 */
void selmod_add_bank_enter( Module_Node_List* );
MenuIterator selmod_add_bank_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );

/**
 * Edit Current Bank
 */
void selmod_edit_bank_enter( Module_Node_List* );
MenuIterator selmod_edit_bank_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );

/**********************************************************************
 * Param Edition Branch
 **********************************************************************/

/**
 * Select Current param
 */
void selmod_editb_selpar_enter( Module_Node_List* );
MenuIterator selmod_editb_selpar_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );

/**
 * Modify Choosen Param
 */
void selmod_editb_editpar_enter( Module_Node_List* );
MenuIterator selmod_editb_editpar_do( Move_flag, MenuIterator, Module_Node_List*, IO_Potentiometer[SPI_POTAR_COUNT] );

#endif
