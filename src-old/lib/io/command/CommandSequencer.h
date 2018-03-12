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

#include "lib/io/Notification.h"
#include "Const.h"

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
        typedef std::pair<ActionT,SFXP::id2_t>        Action;
        typedef std::map<Notification,Action>   ActionMap;

        /* **** Type names uniformisation **** */
        typedef std::vector<std::string>                CommandSequence;
        typedef std::map<SFXP::id2_t, CommandSequence>  CommandSequenceList;
        typedef CommandSequenceList::iterator           CommandListIterator;
        
        /**
         * Constructor
         **/
        CommandSequencer(SFXP::id2_t id);

        /* **** Methods **** */

        /**
         * Assign an action to a given notification source and type
         **/
        void assign(SFXP::id2_t source, Notification::NotificationType type, ActionT act, SFXP::id2_t arg);
        void unassign(SFXP::id2_t source, Notification::NotificationType type);

        /**
         * Push a notification to the command sequencer
         * If notification is linked with an action
         * Correct action will be performed
         * return list of commands to excecute, empty list if nothing required
         **/
        CommandSequence pushNotification(Notification notif);

        /**
         * Edit Command Sequencies
         **/
        void addCommand(SFXP::id2_t seq, std::string cmd);
        void removeCommand(SFXP::id2_t seq, SFXP::usize_t idx);

        /**
         * Print Sequencer Content
         **/
        void print() const;

        /**
         * Get Sequencer's ID
         **/
        SFXP::id2_t getID() const;

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
        CommandSequence performAction(Action act);

        SFXP::id2_t m_id;

        CommandSequenceList m_list;
        CommandListIterator m_currentSeq;
        ActionMap           m_actions;
};

#endif
