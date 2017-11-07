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

#include "../../core/Parser.h"

#include "Commands.h"

class CommandParser{

    public :

        static void registerCommand(Command *cmd);

        static void performCommand(std::vector<std::string> args);

        static void clearRegister();

        class CmdHelp : public VarArgCommand{

            public :

                CmdHelp();

            private :

                virtual void perform(std::vector<std::string> arg);
        };

    private :

        static std::map<std::string,Command*> m_list;
};

#endif
