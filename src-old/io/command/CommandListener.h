/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.1
 *
 * Command Listener
 * Create a new thread and wait for console commands
 **********************************************************************/
#ifndef DEF_COMMAND_LISTENER_H
#define DEF_COMMAND_LISTENER_H

#include <thread>
#include <iostream>
#include <string>

#include "Const.h"

/**
 * Object that always wait for an input from keyboard
 * When a line has been passed throught terminal, it can be accessed
 * with getBuffer function
 **/
class CommandListener
{

    public :

        static void create();
        static void kill();

        static std::string getBuffer();
        static void clearBuffer();

    private :

        static int threadRun(CommandListener* listener);
        int run();

        CommandListener();
        ~CommandListener();

        /**
         * Buffer that will store last entered line
         **/
        std::string m_buffer;

        /**
         * The thread were blocking function will run
         **/
        std::thread m_thread;

        static CommandListener *m_instance;
};

#endif
