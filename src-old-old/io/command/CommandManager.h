/**********************************************************************
 * @file CommandParser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Manager of all commands related stuff
 **********************************************************************/
#ifndef DEF_COMMAND_MANAGER_H
#define DEF_COMMAND_MANAGER_H

#include "../../Const.h"
#include "../core/Message.h"

#include "CommandListener.h"
#include "CommandParser.h"
#include "CommandSequencer.h"

class CommandManager{

    public :

        static int create();
        static int kill();

        static void pushNotification(Notification notif);

        static int addSequencer(id2_t id);
        static int addSequencer(CommandSequencer* seq);
        static void removeSequencer(id2_t id);
        static CommandSequencer* getSequencer(id2_t id);
        static std::vector<id2_t> getSequencerList();

        static void printManager();

        /**
         * Clear Command Manager Configuration
         **/
        static void clearManager();
        
    private :

        typedef std::map<id2_t,CommandSequencer*> SequencerList;

        static SequencerList m_seqList;
};

#endif
