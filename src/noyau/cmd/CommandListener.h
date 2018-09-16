/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.2
 *
 * Command Listener
 * Create a new thread and wait for console inputs
 **********************************************************************/
#ifndef DEF_COMMAND_LISTENER_H
#define DEF_COMMAND_LISTENER_H

#include <thread>
#include <iostream>
#include <string>

#include "noyau/global.h"
#include "noyau/log.h"

/**
 * Object that always wait for an input from keyboard
 * When a line has been passed throught terminal, it can be accessed
 * with getBuffer function
 **/
class CommandListener
{

    public :

        /**
         * Creates the listener and start it
         **/
        CommandListener();
        ~CommandListener();

        std::string getBuffer();
        void clearBuffer();

    private :

        static int threadRun(CommandListener* listener);
        int run();

        /**
         * Buffer that will store last entered line
         **/
        std::string _buffer;

        /**
         * The thread were blocking function will run
         **/
        std::thread _thread;
};

#endif
