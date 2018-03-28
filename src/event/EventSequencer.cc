/**********************************************************************
 * @file EventSequencer.cc
 * @author TomatoPi
 * @version 2.0
 *
 * Object that store a list of events to push when it receive a specified
 *  trigger
 **********************************************************************/
#include "EventSequencer.h"

using namespace std;

#define NAME ("EventSeq("+to_string(_id)+")")

EventSequencer::EventSequencer(SFXP::id2_t id):
_id(id)
, _actions()
,_eventList()
,_curentEvent()
{    
}
EventSequencer::~EventSequencer() {

    for (auto& es : _eventList) {
        for (auto& e : es.second) {

            delete e;
        }
    }
}

/**
 * Function used to link an IOEvent to an action to perform
 **/
void EventSequencer::assign(IOEvent trigger, Action::Type type, SFXP::id2_t target) {

    // Verify that given event is not already linked
    if (_actions.find(trigger) == _actions.end()) {

        _actions[trigger] = Action(type, target);

        SFXPlog::log(NAME) << "Linked Event : " << trigger << " To Action : "
            << _actions[trigger] << endl;
    }
    else {

        SFXPlog::err(NAME) << "Event is Already Linked : " << trigger << endl;
    }
}
void EventSequencer::unassign(IOEvent trigger) {

    // If event is linked unassign it
    if (_actions.find(trigger) != _actions.end()) {

        _actions.erase(trigger);
        SFXPlog::log(NAME) << "UnLinked Event : " << trigger << endl;
    }
    else {

        SFXPlog::err(NAME) << "Event is Not Linked : " << trigger << endl;
    }
}

void EventSequencer::printSequencer() {

    SFXPlog::log(NAME) << " Content : " << endl;
    for (auto& s : _eventList) {

        if (s.first == _curentEvent->first)
            cout << "    >> ";
        else
            cout << "     - ";

        cout << "Sequency ( " << s.first << " ) : " << endl;

        for (auto& e : s.second) {

            cout << "        -> " << (*e) << endl;
        }
    }
}

/**
 * Push an event to the event sequencer
 * If event is linked with an action
 * Correct action will be performed
 **/
EventSequencer::EventSequence EventSequencer::pushEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_FootswitchChanged
        || event->_type == SFXPEvent::Type::Event_AnalogChanged)
    {
        // If event is linked to an action perform it
        if (_actions.find(event->_io) != _actions.end()) {

            return performAction(_actions[event->_io]);
        }
        else {

            SFXPlog::wrn(NAME) << "Unlinked Event Received : "
            << event->_io << endl;
        }
    }
    else {

        SFXPlog::err(NAME) << "Unhandled Event : " << (*event) << endl;
    }
    return EventSequence();
}

/**
 * Edit Event Sequencies
 **/
void EventSequencer::addEvent(SFXP::id2_t seq, SFXPEvent* event) {

    if (_eventList.size() == 0) {

        _eventList[seq].push_back(event);
        _curentEvent = _eventList.begin();
    }
    else {
            
        _eventList[seq].push_back(event);
    }
    SFXPlog::log(NAME) << "Added Event : Sequency(" << seq << "]"
        << " Event : " << (*event) << '\n';
}
void EventSequencer::removeEvent(SFXP::id2_t seq, SFXP::usize_t idx) {

    if ( _eventList.find(seq) != _eventList.end() ){

        EventSequence& sequency = _eventList[seq];
        if ( idx < sequency.size() ){

            sequency.erase(sequency.begin() + idx);
        }
    }
    else{

        SFXPlog::err(NAME) << "Event Not Found : " << seq << endl;
    }
}

/**
 * Get Sequencer's ID
 **/
SFXP::id2_t EventSequencer::id() const {

    return _id;
}

/**
 * Get Stored Event Sequences
 **/
EventSequencer::EventMap EventSequencer::getSequenceList() const {

    return _eventList;
}

/**
 * Get Stored Links
 **/
EventSequencer::ActionMap EventSequencer::getActionList() const {

    return _actions;
}

/**
 * Perform given action
 **/
EventSequencer::EventSequence EventSequencer::performAction(Action act) {

    Action::Type type = act._type;
    
    if ( type != Action::DoNothing && _eventList.size() != 0 ){

        /*
         * Change Current Sequence
         */
        if ( type == Action::CycleNext ){

            #ifdef __DEBUG__
            if ( SFXP::GlobalIsDebugEnabled )
                SFXPlog::debug(NAME) << "Cycle to Next Sequency\n";
            #endif

            // Cycle to Next Sequency
            ++_curentEvent;
            if ( _curentEvent == _eventList.end() ){

                _curentEvent = _eventList.begin();
            }
        }
        else if ( type == Action::CyclePrev ){
            
            #ifdef __DEBUG__
            if ( SFXP::GlobalIsDebugEnabled )
                SFXPlog::debug(NAME) << "Cycle to Prev Sequency\n";
            #endif

            // Cycle to Prev Sequency
            if ( _curentEvent == _eventList.begin() ){

                _curentEvent = _eventList.end();
            }
            --_curentEvent;
        }
        else if ( type == Action::JumpTo ){
            
            #ifdef __DEBUG__
            if ( SFXP::GlobalIsDebugEnabled )
                SFXPlog::debug(NAME) << "Jump To Sequency : " << act._target;
            #endif

            // Try to jump to given Sequency
            if ( _eventList.find(act._target) != _eventList.end() ){

                _curentEvent = _eventList.find(act._target);
            }
            else{

                SFXPlog::wrn(NAME) << "Action linked to unkown sequency : "
                    << static_cast<SFXP::id1_t>(type) << ":" << act._target << endl;
            }
        }
        else if ( type == Action::Execute ){
            
            #ifdef __DEBUG__
            if ( SFXP::GlobalIsDebugEnabled )
                SFXPlog::debug(NAME) << "Execute Sequency : " << act._target << '\n';
            #endif
            // Don't change Current Sequency
        }
        /*
         * Execute current Sequency
         */
        return _curentEvent->second;
    }
    return EventSequence();
}
