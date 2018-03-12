/**********************************************************************
 * @file CommandParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
#include "CommandParser.h"

std::map<std::string,Command*> CommandParser::m_list = std::map<std::string,Command*>();
std::map<id1_t,std::string> CommandParser::m_alias = std::map<id1_t,std::string>();

std::string CommandParser::m_prevCmd = "";

/**
 * Function used to register a new command
 * given id will be used to identify a command while save it
 **/
void CommandParser::registerCommand(id1_t id, Command *cmd){

    std::string key = cmd->getKey();
    
    if ( m_list.find(key) == m_list.end() && m_alias.find(id) == m_alias.end() ){

        m_list[key] = cmd;
        m_alias[id] = key;
        printf("Command Manager : Registered : ( %2u ) \"%s\"\n", id, key.c_str());
    }
    else{

        printf("Command Manager : Warning : \"%s\" Is Already Registered\n", key.c_str());
    }
}

/**
 * Function that takes a vector of words corresponding to a command
 * first word must be command name
 * other ones list of parameters
 **/
void CommandParser::performCommand(std::vector<std::string> args){

    if ( args.size() > 0 ){

        std::string key = args[0];

        if ( m_list.find(key) != m_list.end() ){

            try{
                args.erase(args.begin());
                m_list[key]->tryPerform(args);
            }
            catch( std::invalid_argument& e ){

                printf("Command Error : Invalid Argument : %s\n", e.what());
            }
            catch( std::out_of_range& e ){

                printf("Command Error : Invalid Argument : %s\n", e.what());
            }

            // If performed command is not previous command
            // Save it
            if ( key.compare("p") ){

                if ( SFXP::GlobalIsDebugEnabled ){
                    printf("Save performed Command : \"%s\"\n", key.c_str());
                }
                m_prevCmd = key;
            }
        }
        else{

            printf("Command Manager : Error : \"%s\" : Command Not Found\n", key.c_str());
        }
    }
    else{
        printf("Command Manager : Error : No Command Passed\n");
    }
}

/**
 * Function used to call a command by it id and a vector of
 * parameters
 **/
void CommandParser::performCommand(id1_t id, std::vector<std::string> args){

    if ( m_alias.find(id) != m_alias.end() ){

        args.insert(args.begin(), m_alias[id]);

        performCommand(args);
    }
    else{

        printf("Command Manager : Error : %2u : Alias not Found", id);
    }
}

/**
 * Get command name associated with given id
 **/
std::string CommandParser::getAlias(id1_t id){

    if ( m_alias.find(id) != m_alias.end() ){

        return m_alias[id];
    }
    return std::string();
}
/**
 * Get id associated with given command keyword
 * return 255 if given key not founded
 **/
id1_t CommandParser::getID(std::string key){

    for ( auto & k : m_alias ){

        if ( !k.second.compare(key) ){

            return k.first;
        }
    }
    return 255;
}

/**
 * Get a command id, a vector of parameters and build a command
 * Will return an empty string if given id is not valid
 **/
std::string CommandParser::buildCommand(id1_t id, std::vector<float> args){

    // Verify that provided id is valid
    std::string key = getAlias(id);

    if ( key.size() == 0 ){

        return std::string();
    }

    for ( auto & p : args ){

        key += " " + std::to_string(p);
    }

    return key;
}

/**
 * Clean Command Registry
 **/      
void CommandParser::clearRegister(){
        
    printf("Command Manager : Clear Register ... ");

    for ( auto& cur : m_list ){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Command Manager : Remove : \"%s\"\n", cur.first.c_str());
            
        delete cur.second;
    }
    m_list.clear();
    m_alias.clear();
    printf("Done\n");
}
/**
 * Help Command
 **/
CommandParser::CmdHelp::CmdHelp():
    VarArgCommand("help",
        "help {command} : show list of available commands or help about a specific command",
        0, 1)
{
}

void CommandParser::CmdHelp::perform(std::vector<std::string> arg){

    if ( arg.size() == 0 ){
        
        printf("Available Commands : \n");

        for ( auto& cur : m_list ){

            printf("   - \"%s\"\n", cur.first.c_str());
        }
        printf("End\n");
    }
    else{

        std::string key = arg[0];
        
        if ( m_list.find(key) != m_list.end() ){

            printf(" - \"%s\" : %s\n", key.c_str(), m_list[key]->getHelp().c_str());
        }
        else{

            printf("Command Error : \"%s\" : Command Not Found\n", key.c_str());
        }
    }
}

/**
 * Show alias list
 **/
CommandParser::CmdAlias::CmdAlias():
    StrictArgCommand("alias",
        "alias : show list of ids for commands, used by internal command casters",
        0)
{
}
void CommandParser::CmdAlias::perform(std::vector<std::string> arg){

    printf("Available Aliases : \n");

    for ( auto& cur : m_alias ){

        printf("   - %2u : \"%s\"\n", cur.first, cur.second.c_str());
    }
    printf("End\n");
}

/**
 * Enable debug mode
 **/
CommandParser::CmdDebug::CmdDebug():
    StrictArgCommand("debug",
        "debug [true/false] : enable full program\'s full logging",
        1)
{
}
void CommandParser::CmdDebug::perform(std::vector<std::string> arg){

    if ( !arg[0].compare("true") ){

        printf("Enable debug mode\n");
        SFXP::GlobalIsDebugEnabled = true;
    }
    else{

        printf("Disable debug mode\n");
        SFXP::GlobalIsDebugEnabled = false;
    }
}

/**
 * Recall previous called command
 **/
CommandParser::CmdPrevCmd::CmdPrevCmd():
    VarArgCommand("p",
        "p {args} : Alias for call previous performed command",
        0,10)
{
}
void CommandParser::CmdPrevCmd::perform(std::vector<std::string> arg){

    arg.insert(arg.begin(),m_prevCmd);
    
    performCommand(arg);
}
