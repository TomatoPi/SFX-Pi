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

#include "lib/filter/FilterBase.h"

class ParametricEQ : public AFilterBase{

    public :
    
        ParametricEQ(SFXP::usize_t poleCount, float* poles, float* qf, float samplerate);
        virtual ~ParametricEQ();

        /**
         * Compute given input and output mix of all bands
         * Use getBand function for get a specific band value
         **/
        SFXP::sample_t compute(SFXP::sample_t in, SFXP::usize_t poleCount, float* gains);
        SFXP::sample_t compute(SFXP::sample_t in, SFXP::usize_t idx);
        SFXP::sample_t getBand(SFXP::usize_t idx);

        void setPole(SFXP::usize_t idx, float f, float q, float sr);

        /**
         * Change EQ Configuration
         **/
        void setFrequency(SFXP::usize_t idx, float f, float sr);
        void setFrequency(SFXP::usize_t poleCount, float* poles, float sr);
        float getFrequency(SFXP::usize_t idx) const;
        
        /**
         * Change EQ Configuration
         **/
        void setQFactor(SFXP::usize_t idx, float qf, float sr);
        void setQFactor(SFXP::usize_t poleCount, float* qf, float sr);
        float getQFactor(SFXP::usize_t idx) const;

        SFXP::usize_t getBandCount() const;
        
    private :

        /**
         * Scale Given Band's poles from it center Frenquency and Q factor
         **/
        void updatePole(SFXP::usize_t idx, float cf, float qf, float sr);

        /** Store non Scalled Bands **/
        float*  m_band;

        /** EQ Poles **/
        float*  m_qf;
        float*  m_cf;
        
        FilterPole*     m_poleL;
        FilterPole*     m_poleH;
        SFXP::usize_t   m_poleCount;
};

#endif
