/**********************************************************************
 * @file IOEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct containing IO event informations
 **********************************************************************/
#ifndef DEF_IO_EVENT_H
#define DEF_IO_EVENT_H

#include <iostream>
#include "types.h"

/**
 * Struct containing IO event informations
 *  Logical event are related to digital IO :
 *  - Footswitches
 *  - Leds
 *  - Buttons
 *  Analog event are related to analog inputs :
 *  - Potentiometers
 *  - Expression pedals
 **/
struct IOEvent {

    static const int PUSHED = 1;
    static const int RELEASED = 0;
    static const int ON = 1;
    static const int OFF = 0;
    static const int RELATIVE = 1;
    static const int ABSOLUTE = 0;

    IOEvent(int status, float v, SFXP::id2_t id):
        _status(status), _value(v), _id(id){
    }
    IOEvent()=default;

    friend bool operator < (IOEvent const& l, IOEvent const& r) {

        return l._id < r._id || l._status < r._status || l._value < r._value;
    }

    friend std::ostream& operator << (std::ostream& os, IOEvent const& e)
    {
        return os << "[ID(" << e._id << ") Status(" << e._status
            << ") Value(" << e._value << ")]";
    }

    int _status;        /**< Status of the logical input */
    float _value;       /**< New value of the analog input */
    SFXP::id2_t _id;    /**< ID of the input */
};

#endif
