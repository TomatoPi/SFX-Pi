/**********************************************************************
 * @file Commands.h
 * @author TomatoPi
 * @version 1.1
 *
 * General Commands
 **********************************************************************/
#ifndef DEF_COMMANDS_H
#define DEF_COMMANDS_H

#include <vector>
#include <string>

#include "core/Event.h"
#include "log.h"

class CommandHandler;

class Command{

    public :

        Command(CommandHandler* cmdh, std::string key, std::string help);
        virtual ~Command();

        virtual void tryPerform(std::vector<std::string> arg)=0;

        std::string getKey() const;
        std::string getHelp() const;

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        CommandHandler* _owner;

        std::string _key;
        std::string _help;
};

class StrictArgCommand : public Command{

    public :

        StrictArgCommand(CommandHandler* cmdh,
                        std::string key,
                        std::string help,
                        size_t argCount);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        size_t _argCount;
};

class VarArgCommand : public Command{

    public :

        VarArgCommand(CommandHandler* cmdh,
                        std::string key,
                        std::string help,
                        size_t minArg,
                        size_t maxArg);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        size_t _minArg;
        size_t _maxArg; 
};

#endif
