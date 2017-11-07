/**********************************************************************
 * @file CommandParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
#include "CommandParser.h"

std::map<std::string,Command*> CommandParser::m_list = std::map<std::string,Command*>();
        
void CommandParser::registerCommand(Command *cmd){

    std::string key = cmd->getKey();
    
    if ( m_list.find(key) == m_list.end() ){

        m_list[key] = cmd;
        printf("Command Manager : Registered : \"%s\"\n", key.c_str());
    }
    else{

        printf("Command Manager : Warning : \"%s\" Is Already Registered\n", key.c_str());
    }
}

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
        }
        else{

            printf("Command Manager : Error : \"%s\" : Command Not Found\n", key.c_str());
        }
    }
    else{
        printf("Command Manager : Error : No Command Passed\n");
    }
}

void CommandParser::clearRegister(){

    for ( auto& cur : m_list ){
        
        printf("Command Manager : Remove : \"%s\"\n", cur.first.c_str());
        delete cur.second;
    }
        
    printf("Command Manager : Clear List ... ");
    m_list.clear();
    printf("Done\n");
}

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
