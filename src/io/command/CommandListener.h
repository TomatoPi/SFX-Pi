/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener
 **********************************************************************/
#ifndef DEF_COMMAND_LISTENER_H
#define DEF_COMMAND_LISTENER_H

#include <thread>
#include <iostream>
#include <string>

#include "../../core/Singleton.h"
#include "../core/Listener.h"

/**
 * Singleton Object that always wait for an input from keyboard
 * When a line has been passed throught terminal, it can be accessed
 * with getBuffer function
 **/
class CommandListener : public AbstractListener,
    public Singleton<CommandListener>
{

    friend class Singleton<CommandListener>;

    public :

        std::string getBuffer();
        void clearBuffer();

    private :

        virtual int run();

        CommandListener();
        virtual ~CommandListener();
        
        std::string m_buffer;
};

#endif
