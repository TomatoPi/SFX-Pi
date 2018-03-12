/**********************************************************************
 * @file Commands.cc
 * @author TomatoPi
 * @version 1.1
 *
 * General Commands
 **********************************************************************/
#include "Commands.h"

/* ************************* Basic Commands ************************* */
Command::Command(std::string key, std::string help):
    m_key(key),
    m_help(help)
{
}

Command::~Command()=default;

std::string Command::getKey() const{

    return m_key;
}
std::string Command::getHelp() const{

    return m_help;
}

/* ******************** Constant Arg Count Command ****************** */
StrictArgCommand::StrictArgCommand(std::string key, std::string help, size_t argCount):
    Command(key, help),
    m_argCount(argCount)
{
}

void StrictArgCommand::tryPerform(std::vector<std::string> arg){

    if ( arg.size() == m_argCount ){

        this->perform(arg);
    }else{

        printf("Command Error : \"%s\" : Invalid Argument Count : \"help\" to show usage\n", m_key.c_str());
    }
}

/* ******************** Variable Arg Count Command ****************** */
VarArgCommand::VarArgCommand(std::string key, std::string help, size_t minArg, size_t maxArg):
    Command(key, help),
    m_minArg(minArg),
    m_maxArg(maxArg)
{
}

void VarArgCommand::tryPerform(std::vector<std::string> arg){

    if ( arg.size() >= m_minArg && arg.size() <= m_maxArg ){

        this->perform(arg);
    }else{

        printf("Command Error : \"%s\" : Invalid Argument Count : \"help\" to show usage\n", m_key.c_str());
    }
}
