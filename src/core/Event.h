/**********************************************************************
 * @file Event.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing all events structs for the software internal
 *  comunication protocol
 **********************************************************************/
#ifndef DEF_EVENT_H
#define DEF_EVENT_H

#include <iostream>
#include "types.h"

#include "core/event/EditionEvent.h"
#include "core/event/EffectEvent.h"
#include "core/event/EVHEvent.h"
#include "core/event/GraphEvent.h"
#include "core/event/GuiEvent.h"
#include "core/event/IOEvent.h"
#include "core/event/PresetEvent.h"

/**
 * @brief EventTypeString, array containing names of events
 **/
static const std::string EventTypeString[] = {
    "Event_InitAll"
    ,"Event_ErrorEvent"
    
    ,"Event_FootswitchChanged"
    ,"Event_UpdateOutput"
    ,"Event_AnalogChanged"

    ,"Event_LoadPreset"
    ,"Event_SavePreset"
    ,"Event_NewPreset"
    ,"Event_PresetChanged"

    ,"Event_EffectAddBank"
    ,"Event_EffectRemoveBank"
    ,"Event_EffectChangeBank"
    ,"Event_EffectEditParam"
    ,"Event_EffectUpdateAll"
    ,"Event_EffectShow"
    ,"Event_EffectShowPool"

    ,"Event_GraphConnect"
    ,"Event_GraphDisconnect"

    ,"Event_AddEffect"
    ,"Event_RemoveEffect"
    ,"Event_ClearGraph"
    ,"Event_ShowGraph"
    ,"Event_ListAvailable"

    ,"Event_EVHPrint"
    ,"Event_EVHAddEVS"
    ,"Event_EVHRemoveEVS"
    ,"Event_EVHPrintEVS"
    ,"Event_EVHAssign"
    ,"Event_EVHUnassign"
    ,"Event_EVHAddEvent"
    ,"Event_EVHRemoveEvent"

    ,"Event_GuiInternal"
    ,"Event_GuiInputBool"
    ,"Event_GuiInputString"
    ,"Event_GuiInputInt"
    ,"Event_GuiEditTypeChanged"
    ,"Event_EditableAdded"
    ,"Event_GuiQuit"
};

static const std::string EventTargetString[] = {
     "Target_Logic"
    ,"Target_Effect"
    ,"Target_Event"
    ,"Target_Command"
    ,"Target_Preset"
    ,"Target_Analog"
    ,"Target_Gui"
    ,"Target_All"
    ,"Target_Null"
};

/**
 * Struct representing all events used for internal Communication
 **/
struct SFXPEvent {

    /**
     * @brief Type, list of all available events
     **/
    enum Type {
         Event_InitAll              //= 30
        ,Event_ErrorEvent           //= 31
        
        ,Event_FootswitchChanged    //= 0
        ,Event_UpdateOutput         //= 1
        ,Event_AnalogChanged        //= 2

        ,Event_LoadPreset           //= 3
        ,Event_SavePreset           //= 4
        ,Event_NewPreset
        ,Event_PresetChanged        //= 5

        ,Event_EffectAddBank        //= 6
        ,Event_EffectRemoveBank     //= 7
        ,Event_EffectChangeBank     //= 8
        ,Event_EffectEditParam      //= 9
        ,Event_EffectUpdateAll      //= 10
        ,Event_EffectShow           //= 11
        ,Event_EffectShowPool       //= 12

        ,Event_GraphConnect         //= 13
        ,Event_GraphDisconnect      //= 14

        ,Event_AddEffect            //= 15
        ,Event_RemoveEffect         //= 16
        ,Event_ClearGraph           //= 17
        ,Event_ShowGraph            //= 18
        ,Event_ListAvailable        //= 19

        ,Event_EVHPrint             //= 20
        ,Event_EVHAddEVS            //= 21
        ,Event_EVHRemoveEVS         //= 22
        ,Event_EVHPrintEVS          //= 23
        ,Event_EVHAssign            //= 24
        ,Event_EVHUnassign          //= 25
        ,Event_EVHAddEvent          //= 26
        ,Event_EVHRemoveEvent       //= 27

        ,Event_GuiInternal          //= 28
        ,Event_GuiInputBool
        ,Event_GuiInputString
        ,Event_GuiInputInt
        ,Event_GuiEditTypeChanged
        ,Event_EditableAdded
        ,Event_GuiQuit              //= 29
    };

    /**
     * @brief Target, enum of available targets for an event
     **/
    enum Target {
         TLogic  = 0
        ,TEffect = 1
        ,TEvent  = 2
        ,TCommand= 3
        ,TPreset = 4
        ,TAnalog = 5
        ,TGui    = 6
        ,TAll    = 7
        ,TNull   = 8
    };

    static Target getDefaultTarget(Type t) {

        switch (t) {
            
            case Event_ErrorEvent:
                return TNull;
                
            case Event_InitAll:
                return TAll;
            
            case Event_FootswitchChanged :
            case Event_AnalogChanged:
                return TEvent;
            case Event_UpdateOutput :
                return TLogic;

            case Event_LoadPreset :
            case Event_SavePreset  :
            case Event_NewPreset :
            case Event_PresetChanged :
                return TPreset;

            case Event_EffectAddBank :
            case Event_EffectRemoveBank:
            case Event_EffectChangeBank:
            case Event_EffectEditParam:
            case Event_EffectUpdateAll:
            case Event_EffectShow:
            case Event_EffectShowPool:

            case Event_GraphConnect:
            case Event_GraphDisconnect:

            case Event_AddEffect:
            case Event_RemoveEffect:
            case Event_ClearGraph :
            case Event_ShowGraph :
            case Event_ListAvailable :
                return TEffect;

            case Event_EVHPrint :
            case Event_EVHAddEVS :
            case Event_EVHRemoveEVS :
            case Event_EVHPrintEVS :
            case Event_EVHAssign :
            case Event_EVHUnassign :
                return TEvent;
                
            case Event_GuiInternal :
            case Event_GuiInputBool :
            case Event_GuiInputString :
            case Event_GuiInputInt :
            case Event_GuiEditTypeChanged :
            case Event_EditableAdded :
            case Event_GuiQuit :
                return TGui;

            default :
                return TNull;
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const SFXPEvent& e)
    {
        return os   << "['" << EventTypeString[static_cast<int>(e._type)]
                    << "':" << e._type
                    << ",'" << EventTargetString[static_cast<int>(e._target)]
                    << "':" << e._target << "]";
    }

    SFXPEvent(Type t, Target tar):_type(t), _target(tar)
        ,_nextOk(nullptr)
        ,_nextFail(nullptr)
        ,_alwaysOk(false)
        ,_successed(false)
    {
    }
    SFXPEvent(Type t):SFXPEvent(t, getDefaultTarget(t))
    {
    }
    SFXPEvent():SFXPEvent(Event_ErrorEvent)
    {
    }

    static void eraseEventChain(SFXPEvent* event, bool doDelete = false) {

        if (event) {

            eraseEventChain(event->_nextOk  , true);
            eraseEventChain(event->_nextFail, true);

            if (doDelete) delete event;
            else {
                event->_nextOk   = nullptr;
                event->_nextFail = nullptr;
            }
        }
    }

    /* Members about this Event */
    Type _type;             /**< Event's type */
    Target _target;         /**< Event's target */

    /* Event's content */
    IOEvent         _io;    /**< IOEvent datas */    
    PresetEvent     _preset;/**< PresetEvent datas */
    EffectEvent     _effect;/**< EffectEvent datas */
    GraphEvent      _graph; /**< GraphEvent datas */
    EditionEvent    _edit;  /**< EditionEvent datas */
    EVHEvent        _evh;   /**< EventHandlerEvent datas */
    GuiEvent        _gui;   /**< GuiEvent datas */

    /* Members about nextEvent */
    SFXPEvent*  _nextOk;    /**< Pointer to the next event in the queue */
    SFXPEvent*  _nextFail;  /**< Pointer to the event to process on failure */
    bool        _alwaysOk;  /**< True to always process _nextOk event */
    bool        _successed; /**< True if process of this event has successed */
};

#endif
