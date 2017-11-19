/**********************************************************************
 * @file LED.h
 * @author TomatoPi
 * @version 1.0
 *
 * LED file
 * At the moment led are just booleans store if led is active or not
 * More stuff will be probably aded in the future
 **********************************************************************/
#ifndef DEF_LED_H
#define DEF_LED_H

#include <iostream>

#include "../../core/Typedefs.h"

class LED{

    public :

        /**
         * LED constructor
         * Create a led with given id
         **/
        LED(id2_t id);

        /**
         * Change and get current led State
         **/
        bool toggle();
        bool setState(bool s);
        bool getState() const;

        id2_t getID() const;

    private :

        id2_t   m_id;
        bool    m_state;
};

#endif
