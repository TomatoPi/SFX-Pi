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

        static void registerCommand(id1_t id, Command *cmd);

        static void performCommand(std::vector<std::string> args);

        static void performCommand(id1_t id, std::vector<std::string> args);

        static std::string getAlias(id1_t id);

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
