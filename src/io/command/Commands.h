/**********************************************************************
 * @file Commands.h
 * @author TomatoPi
 * @version 1.0
 *
 * General Commands
 **********************************************************************/
#ifndef DEF_COMMANDS_H
#define DEF_COMMANDS_H

#include <vector>
#include <string>

#include "../../core/Parser.h"

class Command{

    public :

        Command(std::string key, std::string help);

        virtual void tryPerform(std::vector<std::string> arg)=0;

        std::string getKey() const;
        std::string getHelp() const;

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        std::string m_key;
        std::string m_help;
};

class StrictArgCommand : public Command{

    public :

        StrictArgCommand(std::string key, std::string help, uint8_t argCount);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        uint8_t m_argCount;
};

class VarArgCommand : public Command{

    public :

        VarArgCommand(std::string key, std::string help, uint8_t minArg, uint8_t maxArg);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        uint8_t m_minArg;
        uint8_t m_maxArg; 
};

#endif
