/**********************************************************************
 * @file Potentiometer.h
 * @author TomatoPi
 * @version 2.0
 *
 * Basic Analog input object
 **********************************************************************/
#ifndef DEF_POTENTIOMETER_H
#define DEF_POTENTIOMETER_H

#include <string>
#include "types.h"

class Potentiometer {

    public :

        /**
         * Use a constant to define number of potentiometers
         **/
        //static const SFXP::usize_t PotarCount = 8;

        Potentiometer(SFXP::usize_t idx);
        ~Potentiometer();

        SFXP::usize_t index() const;
        void setValue(float v);
        float value() const;

        /**
         * Set and get Potentiometer's diplay parameters
         **/
        void setDisplay(float min, float max, std::string name);
        float dmin() const;
        float dmax() const;
        std::string dname() const;

    private :

        SFXP::usize_t _idx;
        float _value;
        
        float _dmin, _dmax;
        std::string _name;
};

#endif
