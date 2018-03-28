/**********************************************************************
 * @file EventSequencer.h
 * @author TomatoPi
 * @version 2.0
 *
 * Object that store a list of events to push when it receive a specified
 *  trigger
 **********************************************************************/
#ifndef DEF_EVENT_SEQUENCER_H
#define DEF_EVENT_SEQUENCER_H

#include <string>
#include <vector>
#include <map>

#include "core/Event.h"
#include "log.h"

class EventSequencer {

    public :

        /**
         * Object to store an action and the targeted EventSequence
         *  to perform 
         **/
        struct Action{

            /**
             * Enum of avaible actions linkable to a Notification
             **/
            enum Type {
                DoNothing=0, /**< Null action */
                
                CycleNext=1, /**< Cylce to next Sequence and execute it */
                CyclePrev=2, /**< Cycle to prev Sequence and execute it */
                JumpTo=3,    /**< Jump to given Sequence and execute it */
                Execute=4    /**< Execute current Sequence */
            };

            Action(Type t, SFXP::id2_t target):_type(t), _target(target)
            {
            }
            Action():Action(Type::DoNothing, SFXP::ErrId2)
            {
            }

            friend std::ostream& operator << (std::ostream& os, Action const& a)
            {
                return os << "[Type(" << a._type << ") Target(" << a._target << ")]";
            }

            Type _type;
            SFXP::id2_t _target;
        };
        /**
         * Store pair of trigger IOEvent and an action to perform
         **/
        typedef std::map<IOEvent, Action> ActionMap;
        /** Object to store a list of events to perform **/
        typedef std::vector<SFXPEvent*> EventSequence;
        /** Object to store EventSequencies by ID */
        typedef std::map<SFXP::id2_t, EventSequence> EventMap;
        typedef EventMap::iterator EventIterator;

        EventSequencer(SFXP::id2_t id);
        ~EventSequencer();

        /**
         * Function used to link an IOEvent to an action to perform
         **/
        void assign(IOEvent trigger, Action::Type type, SFXP::id2_t target);
        void unassign(IOEvent trigger);

        /**
         * Push an event to the event sequencer
         * If event is linked with an action
         * Correct action will be performed
         **/
        EventSequence pushEvent(SFXPEvent* event);

        /**
         * Edit Event Sequencies
         **/
        void addEvent(SFXP::id2_t seq, SFXPEvent* event);
        void removeEvent(SFXP::id2_t seq, SFXP::usize_t idx);

        /**
         * Get Sequencer's ID
         **/
        SFXP::id2_t id() const;

        /**
         * Get Stored Event Sequences
         **/
        EventMap getSequenceList() const;

        /**
         * Get Stored Links
         **/
        ActionMap getActionList() const;

        void printSequencer();

    private :

        /**
         * Perform given action
         **/
        EventSequence performAction(Action act);

        SFXP::id2_t     _id;

        ActionMap       _actions;

        EventMap        _eventList;
        EventIterator   _curentEvent;
};

#endif
