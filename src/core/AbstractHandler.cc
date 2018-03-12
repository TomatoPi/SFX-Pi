/**********************************************************************
 * @file AbstractHandler.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing base object for program's handlers
 **********************************************************************/
#include "AbstractHandler.h"

AbstractHandler::AbstractHandler(std::string name):
_name(name),
_status(UNBUILDED),
_logicHandler(nullptr),
_eventHandler(nullptr),
_effectHandler(nullptr),
_commandHandler(nullptr),
_presetHandler(nullptr),
_analogHandler(nullptr)
{
}
AbstractHandler::~AbstractHandler() {

}

/**
 * Function to know if the handler contruction is successfull
 * @return non-zero error code if not
 **/
int AbstractHandler::status() {

    return _status;
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
