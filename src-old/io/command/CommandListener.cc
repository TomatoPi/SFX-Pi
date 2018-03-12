/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Command Listener
 * Create a new thread and wait for console commands
 **********************************************************************/
#include "CommandListener.h"

CommandListener *CommandListener::m_instance = NULL;

void CommandListener::create(){

    m_instance = new CommandListener();
}

void CommandListener::kill(){

    delete m_instance;
}

CommandListener::CommandListener() : m_buffer(), m_thread()
{
    m_thread = std::thread(CommandListener::threadRun, this);
    std::cout << "CommandListener : Start Listener" << std::endl;
}

CommandListener::~CommandListener()
{
    std::cout << "CommandListener : Wait For Command Thread to Stop ... ";
    // Wait thread to Stop before destroy Listener
    m_thread.join();
    std::cout << "CommandListener : Closed" << std::endl;
}

std::string CommandListener::getBuffer() {

    return m_instance->m_buffer;
}

void CommandListener::clearBuffer(){

    m_instance->m_buffer.clear();
    m_instance->m_buffer = "";
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
        m_buffer = buffer;
    }
    return 0;
}
