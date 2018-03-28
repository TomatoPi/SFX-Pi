/**********************************************************************
 * @file PresetEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct contaning Preset event informations
 **********************************************************************/
#ifndef DEF_PRESET_EVENT_H
#define DEF_PRESET_EVENT_H

#include "types.h"

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

#endif
