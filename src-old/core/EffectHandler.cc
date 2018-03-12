/**********************************************************************
 * @file EffectHandler.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Previously called ProcessGraph, this file provide the main's program
 *  structure, that manage effects
 **********************************************************************/
#include "EffectHandler.h"

EffectHandler::EffectHandler():AbstractHandler("EffectHandler"){

    log::log(_name)

    try{

        jack_status_t status;

        m_client = jack_client_open( "SpaceFX-Software", JackNullOption, &status, SFXP::JACK_SERVER );
        if ( m_client == NULL ){

            throw string("Unable to connect JACK server");
        }
        if ( status & JackNameNotUnique && SFXP::GlobalIsDebugEnabled ){

            log::log(_name) << "Unique Name " << jack_get_client_name(m_client) << " Assigned\n";
        }

        log::log(_name) << "Registered Graph Client : " << jack_get_client_name(m_client) << endl;

        _status = HANDLER_OK;
    }
    catch (string const& e){

        log::err(_name) << e << endl;
        _status = UNBUILDED;
    }
}
EffectHandler::~EffectHandler(){

    // Close Graph's client
    jack_client_close(m_client);
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void EffectHandler::pushEvent(SFXPEvent& event){

    switch (event._type) {

        case SFXPEvent::Type::Event_PresetChanged:

            eventPresetChanged(event);
            break;

        case SFXPEvent::Type::Event_EffectAddBank:

            eventAddBank(event);
            break;

        case SFXPEvent::Type::Event_EffectRemoveBank:

            eventRemoveBank(event);
            break;

        case SFXPEvent::Type::Event_EffectChangeBank:

            eventChangeBank(event);
            break;

        case SFXPEvent::Type::Event_EffectEditParam:

            eventEditParam(event);
            break;

        case SFXPEvent::Type::Event_GraphConnect:

            eventConnect(event);
            break;

        case SFXPEvent::Type::Event_GraphDisconnect:

            eventDisconnect(event);
            break;

        case SFXPEvent::Type::Event_AddEffect:

            eventAddEffect(event);
            break;

        case SFXPEvent::Type::Event_RemoveEffect:

            eventRemoveEffect(event);
            break;

        default:

            log::wrn(_name) << "Unhandled Event : " << event._type << endl;
    }
}

void eventPresetChanged(SFXPEvent& event) {

}

void eventAddBank(SFXPEvent& event) {

}
void eventRemoveBank(SFXPEvent& event) {

}
void eventChangeBank(SFXPEvent& event) {

}
void eventEditParam(SFXPEvent& event) {

}

void eventConnect(SFXPEvent& event) {

}
void eventDisconnect(SFXPEvent& event) {

}

void eventAddEffect(SFXPEvent& event) {

}
void eventRemoveEffect(SFXPEvent& event) {

}
