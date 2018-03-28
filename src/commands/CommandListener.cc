/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.2
 *
 * Command Listener
 * Create a new thread and wait for console commands
 **********************************************************************/
#include "CommandListener.h"

#define NAME ("CommandListerner")

CommandListener::CommandListener() : _buffer(), _thread()
{
    _thread = std::thread(CommandListener::threadRun, this);
    SFXPlog::log(NAME) << "Start Listener" << std::endl;
}

CommandListener::~CommandListener()
{
    SFXPlog::log(NAME) << "Wait For Command Thread to Stop ... " << '\n';
    // Wait thread to Stop before destroy Listener
    _thread.join();
    SFXPlog::log(NAME) << "Closed" << std::endl;
}

std::string CommandListener::getBuffer() {

    return _buffer;
}

void CommandListener::clearBuffer(){

    _buffer.clear();
    _buffer = "";
}

int CommandListener::threadRun(CommandListener* listener){

    return listener->run();
}
int CommandListener::run(){

    while (!SFXP::GlobalNeedToExit){
        
        // Wait for input
        std::string buffer = "";
        std::getline(std::cin, buffer);

        // If received exit command tell Programm to start exit procedure
        if ( buffer == "exit" ){
            SFXP::GlobalNeedToExit = true;
        }
        
        // Assign m_buffer value after getline to ensure the entire line
        // to be copied
        _buffer = buffer;
    }
    return 0;
}
