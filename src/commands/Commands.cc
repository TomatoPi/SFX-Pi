/**********************************************************************
 * @file Commands.cc
 * @author TomatoPi
 * @version 1.1
 *
 * General Commands
 **********************************************************************/
#include "Commands.h"

/* ************************* Basic Commands ************************* */
Command::Command(CommandHandler* cmdh, std::string key, std::string help):
    _owner(cmdh),
    _key(key),
    _help(help)
{
}

Command::~Command()=default;

std::string Command::getKey() const{

    return _key;
}
std::string Command::getHelp() const{

    return _help;
}

/* ******************** Constant Arg Count Command ****************** */
StrictArgCommand::StrictArgCommand(CommandHandler* cmdh,
                                std::string key, std::string help,
                                size_t argCount):
    Command(cmdh, key, help),
    _argCount(argCount)
{
}

void StrictArgCommand::tryPerform(std::vector<std::string> arg){

    if ( arg.size() == _argCount ){

        this->perform(arg);
    }else{

        SFXPlog::err("Command") << "\"" << _key <<
            "\" : Invalid Argument Count : \"help\" to show usage\n";
    }
}

/* ******************** Variable Arg Count Command ****************** */
VarArgCommand::VarArgCommand(CommandHandler* cmdh,
                            std::string key, std::string help,
                            size_t minArg, size_t maxArg):
    Command(cmdh, key, help),
    _minArg(minArg),
    _maxArg(maxArg)
{
}

void VarArgCommand::tryPerform(std::vector<std::string> arg){

    if ( arg.size() >= _minArg && arg.size() <= _maxArg ){

        this->perform(arg);
    }else{

        SFXPlog::err("Command") << "\"" << _key <<
            "\" : Invalid Argument Count : \"help\" to show usage\n";
    }
}
