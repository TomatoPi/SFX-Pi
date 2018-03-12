/**********************************************************************
 * @file CommandParser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
#ifndef DEF_COMMAND_PARSER_H
#define DEF_COMMAND_PARSER_H

#include <map>
#include <vector>
#include <string>

#include "../../Const.h"
#include "Commands.h"

class CommandParser{

    public :

        /**
         * Function used to register a new command
         * given id will be used to identify a command while save it
         **/
        static void registerCommand(id1_t id, Command *cmd);

        /**
         * Function that takes a vector of words corresponding to a command
         * first word must be command name
         * other ones list of parameters
         **/
        static void performCommand(std::vector<std::string> args);

        /**
         * Function used to call a command by it id and a vector of
         * parameters
         **/
        static void performCommand(id1_t id, std::vector<std::string> args);

        /**
         * Get command name associated with given id
         **/
        static std::string getAlias(id1_t id);
        /**
         * Get id associated with given command keyword
         * return 255 if given key not founded
         **/
        static id1_t getID(std::string key);

        /**
         * Get a command id, a vector of parameters and build a command
         * Will return an empty string if given id is not valid
         **/
        static std::string buildCommand(id1_t id, std::vector<float> args);

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

        class CmdAlias : public StrictArgCommand{

            public :

                CmdAlias();

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

        static std::map<id1_t,std::string> m_alias;

        static std::string m_prevCmd;
};

#endif
