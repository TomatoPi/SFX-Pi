/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.2
 *
 * Command Listener
 * Create a new thread and wait for console commands
 **********************************************************************/
#include "CommandListener.h"

CommandListener::CommandListener() : _buffer(), _thread()
{
    _thread = std::thread(CommandListener::threadRun, this);
    std::cout << "CommandListener : Start Listener" << std::endl;
}

CommandListener::~CommandListener()
{
    std::cout << "CommandListener : Wait For Command Thread to Stop ... ";
    // Wait thread to Stop before destroy Listener
    _thread.join();
    std::cout << "CommandListener : Closed" << std::endl;
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
