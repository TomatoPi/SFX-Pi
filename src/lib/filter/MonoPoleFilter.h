/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Signal Filtering stuff.
 * MonoPole Filter are LowPass Or High pass filters
 * 
 * https://en.wikipedia.org/wiki/Filter_(signal_processing)
 **********************************************************************/
#ifndef DEF_MONOPOLE_FILTER_H
#define DEF_MONOPOLE_FILTER_H

#include "lib/filter/FilterBase.h"

/**
 * Single Pole Filter.
 * Used For compute a 4th order ( -24dB/decade ) Filtering
 * low or high pass a given Signal
 **/
class SinglePoleFilter : public AFilterBase{

    public :

        typedef enum{
            HIGHPASS = 0, /**< Only Keep High Band of Signal */
            LOWPASS  = 1, /**< Only keep Loaw Band of Signal */
            MIXPASS  = 2  /**< Mix Low and High Bands with given gain */
        }FilterType;

        SinglePoleFilter(float freq, float samplerate, FilterType type
                        ,sfx::usize_t order=4);
        virtual ~SinglePoleFilter();

        sfx::sample_t compute(sfx::sample_t in, float g1, float g2);

        void setFrequency(float f, float sr);
        float getFrequency() const;

    private :

        FilterPole* m_pole;
        FilterType m_type;
};

#endif
