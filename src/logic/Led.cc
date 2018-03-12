/**********************************************************************
 * @file Led.cc
 * @author TomatoPi
 * @version 1.1
 *
 * LED file
 * At the moment led are just booleans store if led is active or not
 * More stuff will be probably aded in the future
 **********************************************************************/
#include "Led.h"

/**
 * LED constructor
 * Create a led with given id
 **/
Led::Led(SFXP::id2_t id):
    _id(id),
    _state(false)
{
}

/**
 * Change and get current Led State
 **/
bool Led::toggle(){

    return _state ^= 1;
}
bool Led::setState(bool s){

    return _state = s;
}
bool Led::getState() const{

    return _state;
}

SFXP::id2_t Led::getID() const{

    return _id;
}
