/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.0
 *
 * Listener Template
 * Listener are objects running a function that may block work flow
 * inside another thread
 **********************************************************************/
#include "Listener.h"

/**
 * Constructor register the new thread
 **/
AbstractListener::AbstractListener():m_thread()
{
}

AbstractListener::~AbstractListener()
{
}
void AbstractListener::start(){

    m_thread = std::thread(threadRun, this);
}
/**
 * Function running inside the separate thread
 * Call arg's run function
 * Used because thread cannot take member function as callback
 **/
int AbstractListener::threadRun(AbstractListener* arg){

    return arg->run();
}
