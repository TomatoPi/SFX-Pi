/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener
 **********************************************************************/
#ifndef DEF_LISTENER_H
#define DEF_LISTENER_H

#include <thread>
#include <iostream>
#include <string>

#include "../core/Singleton.h"

class Listener: public Singleton<Listener>{

    friend class Singleton<Listener>;

    public :

        std::string getBuffer();
        void clearBuffer();

    private :

        static void run(Listener *arg);

        Listener();
        ~Listener();

        std::thread m_thread;
        std::string m_buffer;
};

#endif
