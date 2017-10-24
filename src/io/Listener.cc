/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener
 **********************************************************************/
#include "Listener.h"

Listener::Listener():m_thread(run, this), m_buffer()
{
    std::cout << "Command Listener Started" << std::endl;
}

Listener::~Listener()
{
    std::cout << "Command Listener Closed" << std::endl;
}

std::string Listener::getBuffer(){

    return m_buffer;
}

void Listener::clearBuffer(){

    m_buffer.clear();
}

void Listener::run(Listener *arg){

    while (1){
        
        // Waiting for input
        std::string buffer;
        std::getline(std::cin, buffer);

        arg->m_buffer = buffer;
    }
}
