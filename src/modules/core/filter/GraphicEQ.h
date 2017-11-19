/**********************************************************************
 * @file GraphicEQ.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Signal Filtering stuff.
 * GraphicEQ Filter provided by this file is analog to an Equalizer
 * Bands are overlapping at -3dB point ( aproximately pole frequencies )
 * @see ParametricEQ for EQ with ovelapped bands
 * 
 * https://en.wikipedia.org/wiki/Equalization_(audio)
 **********************************************************************/
#ifndef DEF_GRAPHIC_EQ_H
#define DEF_GRAPHIC_EQ_H

#include <algorithm>
#include <iostream>

#include "FilterBase.h"

/**
 * Basic Equalizer
 * @warning an EQ with N bands will Have N-1 Poles
 **/
class GraphicEQ : public AFilterBase{

    public :

        GraphicEQ(size_t poleCount, float* poles, float samplerate);
        virtual ~GraphicEQ();

        /**
         * Compute given input and output mix of all bands
         * Use getBand function for get a specific band value
         **/
        float compute(float in, size_t bandCount, float* gains);
        float getBand(size_t idx);

        /**
         * Change EQ Configuration
         * Cannot be used to change bands count
         **/
        void setFrequency(size_t idx, float f, float sr);
        void setFrequency(size_t poleCount, float* poles, float sr);
        float getFrequency(size_t idx) const;

        size_t getBandCount() const;

    protected :
    
        /** store non-scalled band outputs **/
        float  *m_band;
        size_t  m_bandCount;

        /** Store EQ Poles **/
        FilterPole *m_pole;
        size_t      m_poleCount;
};

#endif
