/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener
 **********************************************************************/
#include "CommandListener.h"

CommandListener::CommandListener():AbstractListener(), m_buffer()
{
    this->start();
    std::cout << "Command Listener Started" << std::endl << std::endl;
}

CommandListener::~CommandListener()
{
    std::cout << "Wait For Command Thread to Stop ... ";
    // Wait thread to Stop before destroy Listener
    m_thread.join();
    std::cout << "Command Listener Closed" << std::endl << std::endl;
}

std::string CommandListener::getBuffer(){

    return m_buffer;
}

void CommandListener::clearBuffer(){

    m_buffer.clear();
}

int CommandListener::run(){

    while (!SFXP::GlobalNeedToExit){
        
        // Wait for input
        std::string buffer;
        std::getline(std::cin, buffer);

        // If received exit command tell Programm to start exit procedure
        if ( buffer == "exit" ){
            SFXP::GlobalNeedToExit = true;
        }
        // Assign m_buffer value after getline to ensure the entire line
        // to be copied
        m_buffer = buffer;
    }
    return 0;
}
