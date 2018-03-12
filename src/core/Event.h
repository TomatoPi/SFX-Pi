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

#include <string>
#include <iostream>
#include "types.h"

/**
 * Struct containing IO event informations
 *  Logical event are related to digital IO :
 *  - Footswitches
 *  - Leds
 *  - Buttons
 *  Analog event are related to analog inputs :
 *  - Potentiometers
 *  - Expression pedals
 **/
struct IOEvent {

    static const int PUSHED = 1;
    static const int RELEASED = 0;
    static const int ON = 1;
    static const int OFF = 0;
    static const int RELATIVE = 1;
    static const int ABSOLUTE = 0;

    IOEvent(int status, float v, SFXP::id2_t id):
        _status(status), _value(v), _id(id){
    }
    IOEvent()=default;

    friend bool operator < (IOEvent const& l, IOEvent const& r) {

        return l._id < r._id || l._status < r._status || l._value < r._value;
    }

    friend std::ostream& operator << (std::ostream& os, IOEvent const& e)
    {
        return os << "[ID(" << e._id << ") Status(" << e._status
            << ") Value(" << e._value << ")]";
    }

    int _status;        /**< Status of the logical input */
    float _value;       /**< New value of the analog input */
    SFXP::id2_t _id;    /**< ID of the input */
};

/**
 * Struct contaning Preset event informations
 *  Preset events are related to preset changing
 **/
struct PresetEvent {

    PresetEvent(std::string f, void* p):_file(f), _preset(p) {
    }
    PresetEvent()=default;

    std::string _file;  /**< Name of the file to load */
    void* _preset;      /**< Pointer to the new preset object */
};

/**
 * Struct contaning Effect event informations
 *  Effects events are rised to edit active effects :
 *  - Change / Add / Remove a Bank
 *  - Edit a parameter
 **/
struct EffectEvent {

    EffectEvent(SFXP::id1_t id, SFXP::usize_t idx, float v):
        _id(id), _idx(idx), _value(v) {
        }
    EffectEvent()=default;

    SFXP::id1_t _id;    /**< ID of the ParamBank targeted by the change */
    SFXP::usize_t _idx; /**< Index of the param to change */
    float _value;       /**< The new value for the param */
};

/**
 * Struct contaning Graph event informations
 *  Graph events are related to modification of the process graph
 *  - Add or Remove a Connection
 **/
struct GraphEvent {

    GraphEvent(SFXP::id1_t si, SFXP::usize_t sp, SFXP::id1_t ti, SFXP::usize_t tp, bool m) :
        _source(si), _sport(sp), _target(ti), _tport(tp), _midi(m) {
        }
    GraphEvent()=default;

    SFXP::id1_t _source;    /**< Id of the source  for the connection */
    SFXP::usize_t _sport;   /**< Index of source port for the connection */
    
    SFXP::id1_t _target;    /**< ID of the target for the connection */
    SFXP::usize_t _tport;   /**< Index of target port for the connection */

    bool _midi; /**< True if this linnk is a midi link */
};

/**
 * Struct contaning Edition event informations
 *  Edition event are related to add or remove an effect from the graph
 **/
struct EditionEvent {

    EditionEvent(SFXP::id1_t id, SFXP::tc_t t):
        _id(id), _type(t) {
    }
    EditionEvent()=default;

    SFXP::id1_t _id;    /**< ID of effect to add */
    SFXP::tc_t _type;   /**< Type of effect to add */
};

/**
 * Struct containing EventHandler event informations
 *  Eventhandler events are related to edit EventSequencers from the preset
 **/
struct EVHEvent {

    EVHEvent(SFXP::id2_t id, SFXP::id2_t tr, int s, int a, SFXP::id2_t ta):
        _id(id), _trigger(tr),
        _status(s), _action(a),
        _target(ta)
    {
    }
    EVHEvent()=default;

    SFXP::id2_t _id;        /**< ID of the sequencer related */
    SFXP::id2_t _trigger;   /**< ID of the trigger (An External Input) */

    int _status;            /**< Wanted Trigger's status */
    int _action;            /**< Action to perform */

    SFXP::id2_t _target;    /**< ID of the Sequence to edit */
};

static const std::string EventTypeString[28] = {
     "Event_FootswitchChanged"
    ,"Event_UpdateOutput"
    ,"Event_AnalogChanged"

    ,"Event_LoadPreset"
    ,"Event_SavePreset"
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

    ,"Event_InitAll"
    ,"Event_ErrorEvent"
};

/**
 * Struct representing all events used for internal Communication
 **/
struct SFXPEvent {

    enum Type {
         Event_FootswitchChanged    = 0
        ,Event_UpdateOutput         = 1
        ,Event_AnalogChanged        = 2

        ,Event_LoadPreset           = 3
        ,Event_SavePreset           = 4
        ,Event_PresetChanged        = 5

        ,Event_EffectAddBank        = 6
        ,Event_EffectRemoveBank     = 7
        ,Event_EffectChangeBank     = 8
        ,Event_EffectEditParam      = 9
        ,Event_EffectUpdateAll      = 10
        ,Event_EffectShow           = 11
        ,Event_EffectShowPool       = 12

        ,Event_GraphConnect         = 13
        ,Event_GraphDisconnect      = 14

        ,Event_AddEffect            = 15
        ,Event_RemoveEffect         = 16
        ,Event_ClearGraph           = 17
        ,Event_ShowGraph            = 18
        ,Event_ListAvailable        = 19

        ,Event_EVHPrint             = 20
        ,Event_EVHAddEVS            = 21
        ,Event_EVHRemoveEVS         = 22
        ,Event_EVHPrintEVS          = 23
        ,Event_EVHAssign            = 24
        ,Event_EVHUnassign          = 25

        ,Event_InitAll              = 26
        ,Event_ErrorEvent           = 27
    };
    
    enum Target {
        TLogic  =0,
        TEffect =1,
        TEvent  =2,
        TCommand=3,
        TPreset =4,
        TAnalog =5
    };

    static Target getDefaultTarget(Type t) {

        switch (t) {
            
            case Event_FootswitchChanged :
            case Event_AnalogChanged:
                return TEvent;
            case Event_UpdateOutput :
                return TLogic;

            case Event_LoadPreset :
            case Event_SavePreset  :
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

            case Event_ErrorEvent:
            case Event_InitAll:
            default :
                return TCommand;
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const SFXPEvent& e)
    {
        return os << EventTypeString[static_cast<int>(e._type)] << ":" << e._type;
    }

    SFXPEvent(Type t, Target tar):_type(t), _target(tar) {
    }
    SFXPEvent(Type t):SFXPEvent(t, getDefaultTarget(t)) {
    }

    Type _type;
    Target _target;

    IOEvent     _io;
    PresetEvent _preset;
    EffectEvent _effect;
    GraphEvent  _graph;
    EditionEvent _edit;
    EVHEvent    _evh;
};

#endif
