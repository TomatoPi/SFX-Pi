/**********************************************************************
 * @file CommandSequencer.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object that perform a serie of commands when receive
 * messages
 **********************************************************************/
#ifndef DEF_COMMAND_SEQUENCER_H
#define DEF_COMMAND_SEQUENCER_H

#include <vector>
#include <map>

#include "../core/Message.h"
#include "../../core/Typedefs.h"
#include "../../core/Parser.h"

#include "CommandParser.h"

class CommandSequencer{

    public :

        /**
         * Enum of avaible actions linkable to a Notification
         **/
        enum ActionT{
            
            DoNothing=0, /**< Null action */
            
            CycleNext=1, /**< Cylce to next Sequence and execute it */
            CyclePrev=2, /**< Cycle to prev Sequence and execute it */
            JumpTo=3,    /**< Jump to given Sequence and execute it */
            Execute=4    /**< Execute current Sequence */
        };
        typedef std::pair<ActionT,id2_t>        Action;
        typedef std::map<Notification,Action>   ActionMap;

        /* **** Type names uniformisation **** */
        typedef std::vector<std::string>            CommandSequence;
        typedef std::map<id2_t, CommandSequence>    CommandSequenceList;
        typedef CommandSequenceList::iterator       CommandListIterator;
        
        /**
         * Constructor4 
         **/
        CommandSequencer(id2_t id);

        /* **** Methods **** */

        /**
         * Assign an action to a given notification source and type
         **/
        void assign(id2_t source, Notification::NotificationType type, ActionT act, id2_t arg);
        void unassign(id2_t source, Notification::NotificationType type);

        /**
         * Push a notification to the command sequencer
         * If notification is linked with an action
         * Correct action will be performed
         **/
        void pushNotification(Notification notif);

        /**
         * Edit Command Sequencies
         **/
        void addCommand(id2_t seq, std::string cmd);
        void removeCommand(id2_t seq, usize_t idx);

        /**
         * Print Sequencer Content
         **/
        void print() const;

        /**
         * Get Sequencer's ID
         **/
        id2_t getID() const;

        /**
         * Get Stored Command Sequences
         **/
        CommandSequenceList getSequenceList() const;

        /**
         * Get Stored Links
         **/
        ActionMap getActionList() const;
        
    private :

        /**
         * Perform given action
         **/
        void performAction(Action act);

        id2_t m_id;

        CommandSequenceList m_list;
        CommandListIterator m_currentSeq;
        ActionMap           m_actions;
};

#endif
