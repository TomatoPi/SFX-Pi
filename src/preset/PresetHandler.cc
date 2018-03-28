/**********************************************************************
 * @file PresetHandler.cc
 * @author TomatoPi
 * @version 1.1
 *
 * File providing program structure for handling presets
 **********************************************************************/
#include "PresetHandler.h"

using namespace SFXP;

PresetHandler::PresetHandler():AbstractHandler("PresetHandler"),
_preset(new Preset())
{
    _status = HANDLER_OK;
    SFXPlog::log(_name) << "Builded Handler" << endl;
}
PresetHandler::~PresetHandler() {

    delete _preset;
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void PresetHandler::pushEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_SavePreset) {

        this->eventSavePreset(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_LoadPreset) {

        this->eventLoadPreset(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_NewPreset) {

        this->eventNewPreset(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (!_preset) _preset = new Preset();

        SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_PresetChanged);
        e._preset = PresetEvent(event->_preset._file, (void*)_preset);

        if (_eventHandler) _eventHandler->pushEvent(&e);
        if (_effectHandler) _effectHandler->pushEvent(&e);
        if (_analogHandler) _analogHandler->pushEvent(&e);
        if (_guiHandler) _guiHandler->pushEvent(&e);

        SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
    }
    else {

        SFXPlog::wrn(_name) << "Unhandled Event : " << (*event) << endl;
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void PresetHandler::run() {

}

void PresetHandler::eventSavePreset(SFXPEvent* event) {

    if (PresetParser::savePreset(event->_preset._file, _preset))
        SFXPlog::err(_name) << "Failed save Preset : " << event->_preset._file << endl;
    else
        SFXPlog::log(_name) << "Saved Preset : " << event->_preset._file << endl;
}
void PresetHandler::eventLoadPreset(SFXPEvent* event) {

    Preset* np = PresetParser::loadPreset(event->_preset._file);

    if (np) {

        SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_PresetChanged);
        e._preset = PresetEvent(event->_preset._file, (void*)np);

        if (_eventHandler) _eventHandler->pushEvent(&e);
        if (_effectHandler) _effectHandler->pushEvent(&e);
        if (_analogHandler) _analogHandler->pushEvent(&e);
        if (_guiHandler) _guiHandler->pushEvent(&e);

        delete _preset;
        _preset = np;
        
        SFXPlog::log(_name) << "Loaded Preset : " << event->_preset._file << endl;
    }
    else
        SFXPlog::err(_name) << "Failed Load Preset : " << event->_preset._file << endl;
}

void PresetHandler::eventNewPreset (SFXPEvent* event) {

    // If a preset is loaded, backup it
    if (_preset) {

        time_t rawtime;
        time(&rawtime);

        SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_SavePreset);
        e._preset._preset = (void*)(_preset);
        e._preset._file =    std::string("Backup: ")
                            +std::string(ctime(&rawtime));
        this->eventSavePreset(&e);
    }

    // Then create and send a new preset to the program
    Preset* np = new Preset();

    SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_PresetChanged);
    e._preset = PresetEvent(event->_preset._file, (void*)np);

    if (_eventHandler) _eventHandler->pushEvent(&e);
    if (_effectHandler) _effectHandler->pushEvent(&e);
    if (_analogHandler) _analogHandler->pushEvent(&e);
    if (_guiHandler) _guiHandler->pushEvent(&e);

    if (_preset) delete _preset;
    _preset = np;

    SFXPlog::log(_name) << "Created a new Preset" << '\n';
}
