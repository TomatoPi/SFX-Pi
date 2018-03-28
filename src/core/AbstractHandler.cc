/**********************************************************************
 * @file AbstractHandler.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing base object for program's handlers
 **********************************************************************/
#include "AbstractHandler.h"
#include "log.h"

AbstractHandler::AbstractHandler(std::string name):
_name(name),
_status(UNBUILDED),
_logicHandler(nullptr),
_eventHandler(nullptr),
_effectHandler(nullptr),
_commandHandler(nullptr),
_presetHandler(nullptr),
_analogHandler(nullptr),
_guiHandler(nullptr)
{
}
AbstractHandler::~AbstractHandler() {

}

/**
 * Function to know if the handler contruction is successfull
 * @return non-zero error code if not
 **/
int AbstractHandler::errored() {

    return 0;
}

/**
 * @brief Function to get the current status of an Handler
 * @return status code
 */
int AbstractHandler::status() {

    return _status;
}

/**
 * Function used to push an event to the targeted handler
 * The event will be imediatly processed via pushEvent method
 **/
void AbstractHandler::sendEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_ErrorEvent) {

        SFXPlog::wrn(_name) << "ErrorEvent Received ..." << '\n';
        return;
    }

    AbstractHandler* target(nullptr);
    switch (event->_target) {

        case SFXPEvent::Target::TAnalog :

            target = _analogHandler;
            break;

        case SFXPEvent::Target::TCommand :

            target = _commandHandler;
            break;

        case SFXPEvent::Target::TEffect :

            target = _effectHandler;
            break;

        case SFXPEvent::Target::TEvent :

            target = _eventHandler;
            break;

        case SFXPEvent::Target::TGui :

            target = _guiHandler;
            break;

        case SFXPEvent::Target::TLogic :

            target = _logicHandler;
            break;

        case SFXPEvent::Target::TPreset :

            target = _presetHandler;
            break;
    }
    if (target) {

        target->pushEvent(event);
    }
    else {

        SFXPlog::err(_name) << "An Handler Is Missing : " << (*event) << '\n';
    }
}
        
/**
 * Functions to connect the Handlers together to permit direct
 *  event pushing and to avoid slow queueing system
 **/
void AbstractHandler::attachLogicHandler(AbstractHandler* l){

    _logicHandler = l;
}
void AbstractHandler::attachEventHandler(AbstractHandler* e){

    _eventHandler = e;
}
void AbstractHandler::attachEffectHandler(AbstractHandler* e){

    _effectHandler = e;
}
void AbstractHandler::attachCommandHandler(AbstractHandler* c){

    _commandHandler = c;
}
void AbstractHandler::attachPresetHandler(AbstractHandler* p){

    _presetHandler = p;
}
void AbstractHandler::attachAnalogHandler(AbstractHandler* a){

    _analogHandler = a;
}
void AbstractHandler::attachGuiHandler(AbstractHandler* g){

    _guiHandler = g;
}

AbstractHandler* AbstractHandler::logicHandler() {

    return _logicHandler;
}
AbstractHandler* AbstractHandler::eventHandler() {

    return _eventHandler;
}
AbstractHandler* AbstractHandler::effectHandler() {

    return _effectHandler;
}
AbstractHandler* AbstractHandler::commandHandler() {

    return _commandHandler;
}
AbstractHandler* AbstractHandler::presetHandler() {

    return _presetHandler;
}
AbstractHandler* AbstractHandler::analogHandler() {

    return _analogHandler;
}

AbstractHandler* AbstractHandler::guiHandler() {

    return _guiHandler;
}
