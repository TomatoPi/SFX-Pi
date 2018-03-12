/**********************************************************************
 * @file CommandParser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Manager of all commands related stuff
 **********************************************************************/
#ifndef DEF_COMMAND_MANAGER_H
#define DEF_COMMAND_MANAGER_H

#include "Const.h"
#include "lib/io/Notification.h"

#include "io/command/CommandListener.h"
#include "io/command/CommandFactory.h"
#include "lib/io/command/CommandSequencer.h"

#include "utils/Parser.h"

class CommandManager{

    public :

        static int create();
        static int kill();

        static void pushNotification(Notification notif);

        static int addSequencer(SFXP::id2_t id);
        static int addSequencer(CommandSequencer* seq);
        static void removeSequencer(SFXP::id2_t id);
        static CommandSequencer* getSequencer(SFXP::id2_t id);
        static std::vector<SFXP::id2_t> getSequencerList();

        static void printManager();

        /**
         * Clear Command Manager Configuration
         **/
        static void clearManager();
        
    private :

        typedef std::map<SFXP::id2_t,CommandSequencer*> SequencerList;

        static SequencerList m_seqList;
};

#endif
