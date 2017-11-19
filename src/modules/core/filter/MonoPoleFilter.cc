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
#include "MonoPoleFilter.h"

SinglePoleFilter::SinglePoleFilter(float freq, float samplerate, FilterType type):
    AFilterBase(),
    m_pole(FilterPole(freq, samplerate)),
    m_type(type)
{

}

SinglePoleFilter::~SinglePoleFilter(){

}

sample_t SinglePoleFilter::compute(sample_t in, float g1, float g2){

    float out = m_pole.compute(in);

    if ( m_type == HIGHPASS ){
        
        out = m_sm3 - out;
    }
    else if ( m_type == MIXPASS ){

        out = out*g1 + (m_sm3 - out)*g2;
    }
    
    this->shuffleBuffer(in);
    return out;
}

void SinglePoleFilter::setFrequency(float f, float sr){

    m_pole.setFrequency( f, sr );
}

float SinglePoleFilter::getFrequency() const{

    return m_pole.getFrequency();
}
