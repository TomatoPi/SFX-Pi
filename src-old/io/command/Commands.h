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

class Command{

    public :

        Command(std::string key, std::string help);
        virtual ~Command();

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

        StrictArgCommand(std::string key, std::string help, size_t argCount);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        size_t m_argCount;
};

class VarArgCommand : public Command{

    public :

        VarArgCommand(std::string key, std::string help, size_t minArg, size_t maxArg);

        virtual void tryPerform(std::vector<std::string> arg);

    protected :

        virtual void perform(std::vector<std::string> arg)=0;

        size_t m_minArg;
        size_t m_maxArg; 
};

#endif
