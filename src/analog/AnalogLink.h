/**********************************************************************
 * @file AnalogLink.h
 * @author TomatoPi
 * @version 2.0
 *
 **********************************************************************/
#ifndef DEF_ANALOG_LINK_H
#define DEF_ANALOG_LINK_H

#include <math.h>
#include "types.h"
#include "core/Utils.h"

class AnalogLink {

    public :

        AnalogLink(SFXP::usize_t idx);
        ~AnalogLink();
        
        SFXP::usize_t index() const;

        /**
         * Set and get Potentiometers used parameters
         **/
        void setTransfert(utils::Curve c, float min, float max, bool isdb, bool isrlt);
        float computeValue(float v);

        utils::Curve curve() const;
        float tmin() const;
        float tmax() const;
        
        bool isdb() const;
        bool isRelative() const;

    private :

        SFXP::usize_t _index;

        utils::Curve       _curve;
        utils::tranfsert_f _tfunc;

        float _tmin, _tmax;
        bool  _isdb;
        bool  _isrlt;
};

#endif
