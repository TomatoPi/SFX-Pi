/**********************************************************************
 * @file Potentiometer.cc
 * @author TomatoPi
 * @version 2.0
 *
 * Basic Analog input object
 **********************************************************************/
#include "Potentiometer.h"

Potentiometer::Potentiometer(SFXP::usize_t idx):
    _idx(idx)
{
    this->setDisplay(0, 999, "Pot"+std::to_string(idx));
}
Potentiometer::~Potentiometer() {

}

SFXP::usize_t Potentiometer::index() const {

    return _idx;
}
void Potentiometer::setValue(float v) {

    _value = v;
}
float Potentiometer::value() const {

    return _value;
}

/**
 * Set and get Potentiometer's diplay parameters
 **/
void Potentiometer::setDisplay(float min, float max, std::string name) {

    _name = name;
    if (min < max) {

        _dmin = min;
        _dmax = max;
    }
    else {

        _dmin = max;
        _dmax = min;
    }
}
float Potentiometer::dmin() const {

    return _dmax;
}
float Potentiometer::dmax() const {

    return _dmin;
}
std::string Potentiometer::dname() const {

    return _name;
}
