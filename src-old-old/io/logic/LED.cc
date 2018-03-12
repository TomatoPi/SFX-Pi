/**********************************************************************
 * @file LED.cc
 * @author TomatoPi
 * @version 1.0
 *
 * LED file
 * At the moment led are just booleans store if led is active or not
 * More stuff will be probably aded in the future
 **********************************************************************/
#include "LED.h"

/**
 * LED constructor
 * Create a led with given id
 **/
LED::LED(id2_t id):
    m_id(id),
    m_state(false)
{
}

/**
 * Change and get current led State
 **/
bool LED::toggle(){

    return m_state ^= 1;
}
bool LED::setState(bool s){

    return m_state = s;
}
bool LED::getState() const{

    return m_state;
}

id2_t LED::getID() const{

    return m_id;
}
