/**********************************************************************
 * @file Led.h
 * @author TomatoPi
 * @version 1.1
 *
 * LED file
 * At the moment led are just booleans store if led is active or not
 * More stuff will be probably aded in the future
 **********************************************************************/
#ifndef DEF_LED_H
#define DEF_LED_H

#include "types.h"

class Led{

    public :

        /**
         * LED constructor
         * Create a led with given id
         **/
        Led(SFXP::id2_t id);

        /**
         * Change and get current led State
         **/
        bool toggle();
        bool setState(bool s);
        bool getState() const;

        SFXP::id2_t getID() const;

    private :

        SFXP::id2_t _id;
        bool        _state;
};

#endif
