/**********************************************************************
 * @file CommandHandler.cc
 * @author TomatoPi
 * @version 1.2
 *
 * Store all available commands and call them, previously called
 *  CommandParser
 **********************************************************************/
#include "CommandHandler.h"

using namespace std;

CommandHandler::CommandHandler(): AbstractHandler("CommandHandler"),
_listener(new CommandListener()),
_commands(CommandList()),
_prevCmd("")
{
    this->buildCommandList();
    _status = HANDLER_OK;
}
CommandHandler::~CommandHandler() {

    delete _listener;

    for (auto& c : _commands) delete c.second;
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void CommandHandler::pushEvent(SFXPEvent& event) {

    if (event._type == SFXPEvent::Type::Event_InitAll) {

        SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
    }
    else {

        SFXPlog::wrn(_name) << "Command Handler doesn't Handle Events" << endl;
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void CommandHandler::run() {

    // Look for commands to perform
    std::string cmd = _listener->getBuffer();
    
    if ( cmd.size() != 0 ) {
        
        std::vector<std::string> args = Parser::parseSimpleString( cmd, " ", '"' );
        if (args.size() != 0 ){

            if (_commands.find(args[0]) != _commands.end()) {
                
                this->performCommand(args);

                if (args[0] != "p") {

                    _prevCmd = args[0];
                }
            }
            else {

                SFXPlog::err(_name) << "Unkown Command : " << args[0] << endl;
            }
        }
        _listener->clearBuffer();
    }
}

/**
 * Function used to register a new command
 * given id will be used to identify a command while save it
 **/
void CommandHandler::registerCommand(Command *cmd){

    std::string key = cmd->getKey();
    
    if ( _commands.find(key) == _commands.end() ){

        _commands[key] = cmd;

        /*
        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            SFXPlog::debug(_name) << "Registered : \"" << key << "\"\n";
        #endif
        * */
    }
    else{

        SFXPlog::err(_name) << "\"" << key <<"\" Is Already Registered\n";
    }
}

void CommandHandler::buildCommandList() {

    // General Commands
    registerCommand(new CmdHelp(this)   );
    registerCommand(new CmdPrevCmd(this));
    registerCommand(new CmdDebug(this)  );
    
    // Process Graph edition
    registerCommand(new CmdAddEffect(this)       );
    registerCommand(new CmdRemoveEffect(this)    );
    registerCommand(new CmdAddConnection(this)   );
    registerCommand(new CmdRemoveConnection(this));
    
    // Process graph View
    registerCommand(new CmdShowGraph(this)       );
    registerCommand(new CmdClearGraph(this)      );
    registerCommand(new CmdListAvailable(this)   );
    
    // Effects Edition and View
    registerCommand(new CmdShowEffect(this)  );
    registerCommand(new CmdAddBank(this)     );
    registerCommand(new CmdRemoveBank(this)  );
    registerCommand(new CmdEditParam(this)   );
    registerCommand(new CmdShowParam(this)   );
    registerCommand(new CmdShowPool(this)    );
    registerCommand(new CmdChangeBank(this)  );
    
    // Presset Managment
    registerCommand(new CmdListPreset(this)  );
    registerCommand(new CmdSavePreset(this)  );
    registerCommand(new CmdLoadPreset(this)  );
    
    // Command Manager
    registerCommand(new CmdPrintCMDManager(this) );
    registerCommand(new CmdAddSequencer(this)    );
    registerCommand(new CmdDelSequencer(this)    );
    
    registerCommand(new CmdPrintSequencer(this)  );
    registerCommand(new CmdAddSeqLink(this)      );
    registerCommand(new CmdDelSeqLink(this)      );
    registerCommand(new CmdAddSeqCmd(this)       );
    registerCommand(new CmdDelSeqCmd(this)       );
}

void CommandHandler::performCommand(std::vector<std::string> args) {

    try{
        std::string key = args[0];
        args.erase(args.begin());
        
        _commands[key]->tryPerform(args);
    }
    catch( std::invalid_argument& e ){

        SFXPlog::err(_name) << "Invalid Argument : " << e.what() << endl;
    }
    catch( std::out_of_range& e ){

        SFXPlog::err(_name) << "Invalid Argument : " << e.what() << endl;
    }
}

/**
 * Help Command
 **/
CommandHandler::CmdHelp::CmdHelp(CommandHandler* h):
    VarArgCommand(h, "help",
        "help {command} : show list of available commands or help about a specific command",
        0, 1)
{
}
void CommandHandler::CmdHelp::perform(std::vector<std::string> arg){

    if ( arg.size() == 0 ){
        
        SFXPlog::log(_owner->_name) << "Available Commands :" << endl;

        for ( auto& cur : _owner->_commands ){

           cout << "   - \"" << cur.first << "\"" << endl;
        }
    }
    else{

        std::string key = arg[0];
        
        if ( _owner->_commands.find(key) != _owner->_commands.end() ){

            cout << " - \"" << key << "\" : " <<
                _owner->_commands[key]->getHelp() << endl;
        }
        else{

            SFXPlog::err(_owner->_name) << "\"" << key <<
                "\" : Command Not Found" << endl;
        }
    }
}

/**
 * Enable debug mode
 **/
CommandHandler::CmdDebug::CmdDebug(CommandHandler* h):
    StrictArgCommand(h, "debug",
        "debug [true/false] : enable full program\'s full logging",
        1)
{
}
void CommandHandler::CmdDebug::perform(std::vector<std::string> arg){

    #ifdef __DEBUG__
    if ( !arg[0].compare("true") ){

        SFXPlog::log(_owner->_name) << "Enable debug mode\n";
        SFXP::GlobalIsDebugEnabled = true;
    }
    else{

        SFXPlog::log(_owner->_name) << "Disable debug mode\n";
        SFXP::GlobalIsDebugEnabled = false;
    }
    #else
    SFXPlog::err(_owner->_name) << "RELEASE-BUILD : Debug Unavailable" << endl;
    #endif
}

/**
 * Recall previous called command
 **/
CommandHandler::CmdPrevCmd::CmdPrevCmd(CommandHandler* h):
    VarArgCommand(h, "p",
        "p {args} : Alias for call previous performed command",
        0,10)
{
}
void CommandHandler::CmdPrevCmd::perform(std::vector<std::string> arg){

    arg.insert(arg.begin(),_owner->_prevCmd);
    
    _owner->performCommand(arg);
}
