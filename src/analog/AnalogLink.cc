/**********************************************************************
 * @file AnalogLink.h
 * @author TomatoPi
 * @version 2.0
 *
 **********************************************************************/
#include "AnalogLink.h"

AnalogLink::AnalogLink(SFXP::usize_t idx):_index(idx) {

    this->setTransfert(utils::Curve::CURVE_LIN, -1, 1, 0, 0);
}
AnalogLink::~AnalogLink() {

}

SFXP::usize_t AnalogLink::index() const {
    
    return _index;
}
/**
 * Set and get Potentiometers used parameters
 **/
void AnalogLink::setTransfert(utils::Curve c, float min, float max, bool isdb, bool isrlt)
{
    _curve = c;
    _tfunc = utils::convertCurve(c);
    _isdb  = isdb;
    _isrlt = isrlt;
    
    if (min < max) {

        _tmin = min;
        _tmax = max;
    }
    else {

        _tmin = max;
        _tmax = min;
    }
    
}
float AnalogLink::computeValue(float v) {

    // If value is an offset
    if (_isrlt) {

        // First remap the value to [-1, 1]
        v = (*_tfunc)(utils::map(v, 0, 1023, -1, 1));
        // Convert to an offset in [-tmin, +tmax]
        v = (v < 0)? v*_tmin : v*_tmax;
    }
    // Else value is absolute
    else {

        // Map the value to [tmin, tmax]
        v = (*_tfunc)(utils::map(v, 0, 1023, _tmin, _tmax));
    }

    // If boudaries are in db return a factor
    if (_isdb)  return utils::dbtorms(v);
    // Else return the value
    else        return v;
}
utils::Curve AnalogLink::curve() const {

    return _curve;
}
float AnalogLink::tmin() const {

    return _tmin;
}
float AnalogLink::tmax() const {

    return _tmax;
}
bool AnalogLink::isdb() const {

    return _isdb;
}
bool AnalogLink::isRelative() const {

    return _isrlt;
}
