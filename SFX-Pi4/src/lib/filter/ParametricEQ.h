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
    
        ParametricEQ(sfx::usize_t poleCount, float* poles, float* qf, float samplerate
                    ,sfx::usize_t* orders);
        virtual ~ParametricEQ();

        /**
         * Compute given input and output mix of all bands
         * Use getBand function for get a specific band value
         **/
        sfx::sample_t compute(sfx::sample_t in, sfx::usize_t poleCount, float* gains);
        sfx::sample_t compute(sfx::sample_t in, sfx::usize_t idx);
        sfx::sample_t getBand(sfx::usize_t idx);

        void setPole(sfx::usize_t idx, float f, float q, float sr, sfx::usize_t order);

        /**
         * Change EQ Configuration
         **/
        void setFrequency(sfx::usize_t idx, float f, float sr);
        void setFrequency(sfx::usize_t poleCount, float* poles, float sr);
        float getFrequency(sfx::usize_t idx) const;
        
        /**
         * Change EQ Configuration
         **/
        void setQFactor(sfx::usize_t idx, float qf, float sr);
        void setQFactor(sfx::usize_t poleCount, float* qf, float sr);
        float getQFactor(sfx::usize_t idx) const;

        sfx::usize_t getBandCount() const;
        
    private :

        /**
         * Scale Given Band's poles from it center Frenquency and Q factor
         **/
        void updatePole(sfx::usize_t idx, float cf, float qf, float sr, sfx::usize_t order);

        /** Store non Scalled Bands **/
        float*  m_band;

        /** EQ Poles **/
        float*          m_qf;
        float*          m_cf;
        sfx::usize_t*  m_orders;
        
        FilterPole**    m_poleL;
        FilterPole**    m_poleH;
        sfx::usize_t   m_poleCount;
};

#endif
