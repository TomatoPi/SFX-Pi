/**********************************************************************
 * @file EventHandler.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Object that manage EventSequencers
 **********************************************************************/
#include "EventHandler.h"

using namespace std;

EventHandler::EventHandler():AbstractHandler("EventHandler"),
_preset(nullptr)
{
    _status = HANDLER_OK;
    SFXPlog::log(_name) << "Builded Handler" << endl;
}
EventHandler::~EventHandler() {

    this->clearEvents();
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void EventHandler::pushEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_PresetChanged) {

        this->eventPresetChanged(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (!_preset) {

            SFXPlog::err(_name) << "No Preset Given to the Handler" << endl;
        }
        else {
                
            SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
        }
    }
    else if (_preset) {

        switch(event->_type) {

            case SFXPEvent::Type::Event_AnalogChanged :
            case SFXPEvent::Type::Event_FootswitchChanged :
            
                this->eventIOChanged(event);
                break;

            case SFXPEvent::Type::Event_EVHPrint :

                this->printHandler();
                break;

            case SFXPEvent::Type::Event_EVHAddEVS :

                this->eventAddSeq(event);
                break;

            case SFXPEvent::Type::Event_EVHRemoveEVS :

                this->eventRemoveSeq(event);
                break;

            case SFXPEvent::Type::Event_EVHPrintEVS :

                this->eventPrintSeq(event);
                break;

            case SFXPEvent::Type::Event_EVHAssign :

                this->eventAssign(event);
                break;

            case SFXPEvent::Type::Event_EVHUnassign :

                this->eventUnassign(event);
                break;

            case SFXPEvent::Type::Event_EVHAddEvent :

                this->eventAddEvent(event);
                break;

            case SFXPEvent::Type::Event_EVHRemoveEvent :

                this->eventRemoveEvent(event);
                break;

            default :
                SFXPlog::err(_name) << "Unhandled Event : " << (*event) << endl;
        }
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void EventHandler::run() {

}

void EventHandler::clearEvents() {

    if (_preset) {

        for (auto& s : _preset->_sequencers) delete s.second;
        _preset->_sequencers.clear();
    }
}

void EventHandler::printHandler() {

    if (_preset) {

        SFXPlog::log(_name) << "Current Active Sequencers :" << endl;
        for (auto& s : _preset->_sequencers) {

            cout << "   - " << s.first << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}

void EventHandler::eventIOChanged(SFXPEvent* event) {

    if (_preset) {

        for (auto& s : _preset->_sequencers) {

            EventSequencer::EventSequence seq = s.second->pushEvent(event);

            for (auto& e : seq) this->sendEvent(e);
        }
    }
}
void EventHandler::eventPresetChanged(SFXPEvent* event) {

    if (Preset* np = (Preset*)event->_preset._preset) {

        this->clearEvents();
        _preset = np;
    }
}

void EventHandler::eventAddSeq(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) == _preset->_sequencers.end())
        {

            _preset->_sequencers[id] = new EventSequencer(id);
            SFXPlog::log(_name) << "Added Sequencer : " << id << endl;
        }
        else {
            
            SFXPlog::err(_name) << "ID Already Used : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventRemoveSeq(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {

            delete _preset->_sequencers[id];
            _preset->_sequencers.erase(id);
            
            SFXPlog::log(_name) << "Removed Sequencer : " << id << endl;
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventPrintSeq(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {

            _preset->_sequencers[id]->printSequencer();
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventAssign(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {
            _preset->_sequencers[id]->assign(
                    IOEvent(event->_evh._status, 0, event->_evh._trigger)
                    ,static_cast<EventSequencer::Action::Type>(event->_evh._action)
                    ,event->_evh._target);
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventUnassign(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {
            _preset->_sequencers[id]->unassign(
                    IOEvent(event->_evh._status, 0, event->_evh._trigger));
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventAddEvent(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {
            if (event->_evh._event)
            {
                _preset->_sequencers[id]->addEvent(
                        event->_evh._target, event->_evh._event);
            }
            else {
                
                SFXPlog::err(_name) << "No Event Passed" << endl;
            }
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
void EventHandler::eventRemoveEvent(SFXPEvent* event) {

    if (_preset) {

        SFXP::id2_t id = event->_evh._id;

        if (_preset->_sequencers.find(id) != _preset->_sequencers.end())
        {
            _preset->_sequencers[id]->removeEvent(
                        event->_evh._target, event->_evh._idx);
        }
        else {
            
            SFXPlog::err(_name) << "ID Doesn't Exist : " << id << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}
