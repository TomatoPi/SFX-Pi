#ifndef DEF_DECLARATION_H
#define DEF_DECLARATION_H

#include "../../core/Tree/Tree.h"
#include "../../core/Modules.h"

#include "../Potar.h"
#include "../Button.h"

/**********************************************************************
 * Enum of all submenus flags
 * Used as nodes id inside menu's tree
 **********************************************************************/
typedef enum{
    
    /** Outside of main Menu */
    MAIN_MENU_FLAG,

        /** Submenu for change current presset */
        MAIN_CHANGE_PRESSET,
        
        /** Submenu for select a module to Modify */
        MAIN_SELECT_MODULE,

            /** Change current bank */
            SELMOD_CHANGE_BANK,
            /** Add a new bank to the module */
            SELMOD_ADD_BANK,
            /** Edit Selected bank */
            SELMOD_EDIT_BANK,
            
                /** Select param to modify */
                SELMOD_EDITB_SELPAR,
                    /** Modify choosen param */
                    SELMOD_EDITB_EDITPAR,

        /** Add a module to the Graph */
        MAIN_ADD_MODULE,
            /** Choose module's type */
            ADDMOD_CHOOSE_MODTYPE,

        /** Change Connection Graph */
        MAIN_CHANGE_CONNECTION,

            /** Add new Connection */
            EDITCO_ADD_CONNECTION,
                /** Choose connection's source */
                ADDCO_CHOOSE_CSOURCE,
                    /** Choose connection's target */
                    ADDCO_CHOOSE_CTARGET,

        /** Edit Potentiometer */
        MAIN_EDIT_POT,

            /** Edit Pot display values */
            EDDITPOT_DISPLAY,

                EDITPOT_DISP_NAME,
                EDITPOT_DISP_MIN,
                EDITPOT_DISP_MAX,

            /** Add an accessor to selected pot */
            EDITPOT_ADD_ACCESSOR,
            
                EDITPOT_CHOOSE_TARGET_M,
                EDITPOT_CHOOSE_TARGET_P,
                EDITPOT_CHOOSE_MIN,
                EDITPOT_CHOOSE_MAX,
                EDITPOT_CHOOSE_CURVE,
                EDITPOT_CHOOSE_ISDB,
                EDITPOT_CHOOSE_ISRLT,

            /** Change an accessor of selected pot */
            EDITPOT_EDIT_ACCESSOR,
            
                EDITPOT_EDIT_TARGET_M,
                EDITPOT_EDIT_TARGET_P,
                EDITPOT_EDIT_MIN,
                EDITPOT_EDIT_MAX,
                EDITPOT_EDIT_CURVE,
                EDITPOT_EDIT_ISDB,
                EDITPOT_EDIT_ISRLT
    
}Status;

/**********************************************************************
 * Enum of differents movements avaiable inside the menu
 * Each movement is ascociated with a specific button
 **********************************************************************/
typedef enum{
    
    MOVE_UP     =0,
    MOVE_DOWN   =1,
    
    MOVE_NEXT   =2,
    MOVE_PREV   =3,
    
    MOVE_ADD    =4,
    MOVE_DEL    =5,
    
    MOVE_ENTER  =6,
    
    MOVE_ESC    =7,
    MOVE_OK     =8

}Move_flag;

/**********************************************************************
 *                          Menu structure
 **********************************************************************/

struct Submenu;
typedef struct Submenu Submenu;

typedef Tree<Submenu, Status> MenuTree;
typedef MenuTree::Iterator MenuIterator;

/**
 * Submenu structure, used as node's datas iniside tree structure
 * It contains two functions, one called when move to this node
 * The other called whean a button is pressed inside it
 */
struct Submenu{

    /**
     * Function Called when move into this submenu
     */
    void (*enter_)(Module_Node_List*);
    /**
     * Function Called when a button is pressed inside this submenu
     */
    MenuIterator (*do_)( Move_flag, MenuIterator, Module_Node_List* &, IO_Potentiometer[SPI_POTAR_COUNT] );

};

/**
 * Menu's root node functions
 */
void main_menu_exit_menu( Module_Node_List* );
MenuIterator main_menu_do( Move_flag, MenuIterator, Module_Node_List* &, IO_Potentiometer[SPI_POTAR_COUNT] );

#endif
