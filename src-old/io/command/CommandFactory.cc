/**********************************************************************
 * @file CommandParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
#include "CommandFactory.h"

std::map<std::string,Command*> CommandFactory::m_list = std::map<std::string,Command*>();

std::string CommandFactory::m_prevCmd = "";

/**
 * Function used to register a new command
 * given id will be used to identify a command while save it
 **/
void CommandFactory::registerCommand(Command *cmd){

    std::string key = cmd->getKey();
    
    if ( m_list.find(key) == m_list.end() ){

        m_list[key] = cmd;
        if (SFXP::GlobalIsDebugEnabled)
            printf("Command Manager : Registered : \"%s\"\n", key.c_str());
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
void CommandFactory::performCommand(std::vector<std::string> args){

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
 * Get number of commands registered
 **/
SFXP::usize_t CommandFactory::registrySize(){

    return m_list.size();
}
        
/**
 * Clean Command Registry
 **/      
void CommandFactory::clearRegister(){
        
    printf("Command Manager : Clear Register ... ");

    for ( auto& cur : m_list ){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Command Manager : Remove : \"%s\"\n", cur.first.c_str());
            
        delete cur.second;
    }
    m_list.clear();
    printf("Done\n");
}
/**
 * Help Command
 **/
CommandFactory::CmdHelp::CmdHelp():
    VarArgCommand("help",
        "help {command} : show list of available commands or help about a specific command",
        0, 1)
{
}
void CommandFactory::CmdHelp::perform(std::vector<std::string> arg){

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
 * Enable debug mode
 **/
CommandFactory::CmdDebug::CmdDebug():
    StrictArgCommand("debug",
        "debug [true/false] : enable full program\'s full logging",
        1)
{
}
void CommandFactory::CmdDebug::perform(std::vector<std::string> arg){

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
CommandFactory::CmdPrevCmd::CmdPrevCmd():
    VarArgCommand("p",
        "p {args} : Alias for call previous performed command",
        0,10)
{
}
void CommandFactory::CmdPrevCmd::perform(std::vector<std::string> arg){

    arg.insert(arg.begin(),m_prevCmd);
    
    performCommand(arg);
}
