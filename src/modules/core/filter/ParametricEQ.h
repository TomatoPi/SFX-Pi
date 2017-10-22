/**********************************************************************
 * @file ParametricEQ.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Signal Filtering stuff.
 * ParametricEQ provide a pseudo-parametric Equalizer for signal processing
 * Bands overlapping is enabled for this EQ type
 * Each band is caracterised by it center frequency and Q factor
 * This EQ is pseudo parametric because band slope is fixed at -24dB/decade
 * and Q factor only modify positions of band's -3dB point
 * 
 * https://en.wikipedia.org/wiki/Equalization_(audio)
 **********************************************************************/
#ifndef DEF_PARAMETRIC_EQ_H
#define DEF_PARAMETRIC_EQ_H

#include <algorithm>
#include <iostream>

#include "FilterBase.h"

class ParametricEQ : public AFilterBase{

    public :
    
        ParametricEQ(uint8_t poleCount, float* poles, float* qf, float samplerate);
        virtual ~ParametricEQ();

        /**
         * Compute given input and output mix of all bands
         * Use getBand function for get a specific band value
         **/
        float compute(float in, uint8_t bandCount, float* gains);
        float getBand(uint8_t idx);

        /**
         * Change EQ Configuration
         **/
        void setFrequency(uint8_t idx, float f, float sr);
        void setFrequency(uint8_t poleCount, float* poles, float sr);
        float getFrequency(uint8_t idx) const;
        
        /**
         * Change EQ Configuration
         **/
        void setQFactor(uint8_t idx, float qf, float sr);
        void setQFactor(uint8_t poleCount, float* qf, float sr);
        float getQFactor(uint8_t idx) const;

        uint8_t getBandCount() const;
        
    private :

        /**
         * Scale Given Band's poles from it center Frenquency and Q factor
         **/
        void updatePole(uint8_t idx, float cf, float qf);

        /** Store non Scalled Bands **/
        float  *m_band;
        uint8_t m_bandCount;

        /** EQ Poles **/
        float      *m_qf;
        float      *m_cf;
        
        FilterPole *m_poleL;
        FilterPole *m_poleH;
        uint8_t     m_poleCount;
};

#endif
