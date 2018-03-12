/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener Template
 * Listener are objects running a function that may block work flow
 * inside another thread
 **********************************************************************/
#ifndef DEF_LISTENER_H
#define DEF_LISTENER_H

#include <thread>
#include <chrono>

#include "../../Const.h"

class AbstractListener{
    
    protected :

        /**
         * Constructor register the new thread
         **/
        AbstractListener();

        /**
         * Destructor
         **/
        virtual ~AbstractListener();

        /**
         * Start listenning thread
         **/
        void start();

        /**
         * Listener's subtype specific function
         **/
        virtual int run()=0;

    private :

        /**
         * Function running inside the separate thread
         * Call arg's run function
         * Used because thread cannot take member function as callback
         **/
        static int threadRun(AbstractListener* arg);
};

#endif
