/**********************************************************************
 * @file CommandHandler.h
 * @author TomatoPi
 * @version 1.2
 *
 * Store all available commands and call them, previously called
 *  CommandParser
 **********************************************************************/
#ifndef DEF_COMMAND_HANDLER_H
#define DEF_COMMAND_HANDLER_H

#include <string>
#include <vector>
#include <map>

#include "core/AbstractHandler.h"
#include "core/Event.h"
#include "core/Parser.h"

#include "commands/CommandListener.h"
#include "commands/Commands.h"

#include "commands/list/CommandList.h"

#include "log.h"

class CommandHandler : public AbstractHandler {

    public :

        typedef std::map<std::string, Command*> CommandList;

        CommandHandler();
        virtual ~CommandHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent& event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run();

        class CmdHelp : public VarArgCommand{

            public :

                CmdHelp(CommandHandler* h);

            private :

                virtual void perform(std::vector<std::string> arg);
        };

        class CmdDebug : public StrictArgCommand{

            public :

                CmdDebug(CommandHandler* h);

            private :

                virtual void perform(std::vector<std::string> arg);
        };

        class CmdPrevCmd : public VarArgCommand{

            public :

                CmdPrevCmd(CommandHandler* h);

            private :

                virtual void perform(std::vector<std::string> arg);
        };

    private :
    
        /**
         * Function used to register a new command
         * given id will be used to identify a command while save it
         **/
        void registerCommand(Command *cmd);

        void buildCommandList();
        void performCommand(std::vector<std::string> args);

        CommandListener*_listener;
        CommandList     _commands;

        std::string _prevCmd;
};

#endif
