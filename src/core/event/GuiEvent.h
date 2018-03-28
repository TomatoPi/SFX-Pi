/**********************************************************************
 * @file GuiEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct containing GuiHandler Events informations
 **********************************************************************/
#ifndef DEF_GUI_EVENT_H
#define DEF_GUI_EVENT_H

#include "types.h"

/**
 * Struct containing GuiHandler Events informations
 **/
struct GuiEvent {

    static const int PARAMSET   = 1;
    static const int CONNECTION = 2;
    static const int SEQUENCER  = 3;
    static const int ALINK      = 4;

    GuiEvent()=default;

    bool _boolDialog;       /**< Return value of a bool dialog */
    std::string _strDialog; /**< Return value of a string dialog */
    int _intDialog;         /**< Return value of an int dialog */

    int _editableType;  /**< int representing type of ediatble object contained */
    void* _editable;    /**< Pointer to the editable */
};

#endif
