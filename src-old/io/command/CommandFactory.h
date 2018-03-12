/**********************************************************************
 * @file CommandFactory.h
 * @author TomatoPi
 * @version 1.1
 *
 * Store all available commands and call them
 **********************************************************************/
#ifndef DEF_COMMAND_PARSER_H
#define DEF_COMMAND_PARSER_H

#include <map>
#include <vector>
#include <string>

#include "Const.h"
#include "io/command/Commands.h"

class CommandFactory{

    public :

        /**
         * Function used to register a new command
         * given id will be used to identify a command while save it
         **/
        static void registerCommand(Command *cmd);

        /**
         * Function that takes a vector of words corresponding to a command
         * first word must be command name
         * other ones list of parameters
         **/
        static void performCommand(std::vector<std::string> args);

        /**
         * Get number of commands registered
         **/
        static SFXP::usize_t registrySize();
        
        /**
         * Clean Command Registry
         **/
        static void clearRegister();

        class CmdHelp : public VarArgCommand{

            public :

                CmdHelp();

            private :

                virtual void perform(std::vector<std::string> arg);
        };

        class CmdDebug : public StrictArgCommand{

            public :

                CmdDebug();

            private :

                virtual void perform(std::vector<std::string> arg);
        };

        class CmdPrevCmd : public VarArgCommand{

            public :

                CmdPrevCmd();

            private :

                virtual void perform(std::vector<std::string> arg);
        };

    private :

        static std::map<std::string,Command*> m_list;

        static std::string m_prevCmd;
};

#endif
