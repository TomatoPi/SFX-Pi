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

#include "Const.h"

/**
 * Struct containing Logical event informations
 *  Logical event are related to digital IO :
 *  - Footswitches
 *  - Leds
 *  - Buttons
 **/
struct LogicEvent {

    static const int PUSHED = 1;
    static const int RELEASED = 0;
    static const int ON = 1;
    static const int OFF = 0;

    LogicEvent(int status, SFXP::id2_t id):_status(status), _id(id){
    }

    int _status;
    SFXP::id2_t _id;
};

/**
 * Struct containing Analog event informations
 *  Analog event are related to analog inputs :
 *  - Potentiometers
 *  - Expression pedals
 **/
struct AnalogEvent {

    AnalogEvent(float v, SFXP::id2_t id):_value(v), _id(id) {
    }

    float _value;
    SFXP::id2_t _id;
};

/**
 * Struct contaning Preset event informations
 *  Preset events are related to preset changing
 **/
struct PresetEvent {

    PresetEvent(std::string f, void* p):_file(f), _preset(p) {
    }

    std::string _file;
    void* _preset;
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

    SFXP::id1_t _id;
    SFXP::usize_t _idx;
    float _value;
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

    SFXP::id1_t _source;
    SFXP::usize_t _sport;
    
    SFXP::id1_t _target;
    SFXP::usize_t _tport;

    bool _midi;
};

/**
 * Struct contaning Edition event informations
 *  Edition event are related to add or remove an effect from the graph
 **/
struct EditionEvent {

    EditionEvent(SFXP::id1_t id, SFXP::tc_t t): _id(id), _type(t) {
    }

    SFXP::id1_t _id;
    SFXP::tc_t _type;
};

/**
 * Struct representing all events used for internal Communication
 **/
struct SFXPEvent {

    enum Type {
        Event_FootswitchChanged,
        Event_UpdateOutput,

        Event_AnalogChanged,

        Event_LoadPreset,
        Event_SavePreset,
        Event_PresetChanged,

        Event_EffectAddBank,
        Event_EffectRemoveBank,
        Event_EffectChangeBank,
        Event_EffectEditParam,

        Event_GraphConnect,
        Event_GraphDisconnect,

        Event_AddEffect,
        Event_RemoveEffect
    };

    SFXPEvent(Type t):_type(t){
    }

    Type _type;

    LogicEvent _logic;
    AnalogEvent _analog;
    PresetEvent _preset;
    EffectEvent _effect;
    GraphEvent _graph;
    EditionEvent _edit;
};

#endif
