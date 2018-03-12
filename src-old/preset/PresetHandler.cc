/**********************************************************************
 * @file PresetHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * Object that will store and manage current preset
 **********************************************************************/
#include "PresetHandler.h"
using namespace std;

PresetHandler::PresetHandler():AbstractHandler("PresetHandler"),
_curentPreset(nullptr)
{
    log::log(_name) << "Created Handler" << endl;
    _status = HANDLER_OK;
}
PresetHandler::~PresetHandler() {

    log::log(_name) << "Delete Handler" << endl;

    delete _curentPreset;
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void PresetHandler::pushEvent(SFXPEvent& event) {

    switch (event._type) {

        case SFXPEvent::Type::Event_LoadPreset:

            this->eventLoadPreset(event);
            break;

        case SFXPEvent::Type::Event_SavePreset:

            this->eventSavePreset(event);
            break;

        default:
        
            log::wrn(_name) << "Unhandled Event : " << event._type << endl;
            break;
    }
}

/**
 * Function called when a load event is received
 **/
void PresetHandler::eventLoadPreset(SFXPEvent& event) {

    Preset* np = PresetParser::loadPreset(event._preset._file);
    
    if (np) {

        // Rise events to the handlers to update preset file
        SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_PresetChanged);
        e._preset = PresetEvent(event._preset._file, (void*)np);

        _eventHandler->pushEvent(e);
        _effectHandler->pushEvent(e);

        // When all handlers have been updated change current preset
        delete _curentPreset;
        _curentPreset = np;
    }
    else {

        log::err(_name) << "Failed Load Preset : " << event._preset._file << endl;
    }
}
/**
 * Function called when a save event is received
 **/
void PresetHandler::eventSavePreset(SFXPEvent& event) {

    if (PresetParser::savePreset(event._preset._file)) {

        log::err(_name) << "Failed Save Preset : " << event._preset._file << endl;
    }
    else {

        log::log(_name) << "Successfully Saved Preset : " << event._preset._file << endl;
    }
}
