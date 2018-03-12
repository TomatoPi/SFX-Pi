/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Signal Filtering stuff.
 * MultiPole Filter provided by this file is Similar to a MultiBand
 * Parametric Equalizer
 * 
 * https://en.wikipedia.org/wiki/Equalization_(audio)
 **********************************************************************/
#include "MultibandEQ.h"

MultibandEQ::MultibandEQ(uint8_t poleCount, float* poles, float samplerate):
    AFilterBase(),
    m_band(new float[poleCount+1]),
    m_bandCount(poleCount+1),
    m_pole(new FilterPole[poleCount]),
    m_poleCount(poleCount)
{
    std::sort( poles, poles + poleCount );
    for ( uint8_t i = 0; i < poleCount; i++ ){

        m_pole[i] = FilterPole(poles[i], samplerate);
    }
}

MultibandEQ::~MultibandEQ(){

    delete m_band;
    delete m_pole;
}

float MultibandEQ::compute(float in, uint8_t bandCount, float* gains){

    // If given band count is invalid output non filtered signal
    if ( bandCount != m_bandCount ) return in;
    
    // Compute Each Band From lowest Freq to Highest
    float sband = 0;
    float out = 0;
    for ( uint8_t i = 0; i < m_poleCount; i++ ){

        m_band[i] = m_pole[i].compute(in) - sband;
        sband += m_band[i];
        
        out += m_band[i] * gains[i];
    }
    // Calculate Last band
    m_band[m_poleCount] = m_sm3 - sband;

    //Suffle buffer
    this->shuffleBuffer( in );

    return out;
}
float MultibandEQ::getBand(uint8_t idx){

    if ( idx < m_bandCount ){
        
        return m_band[idx];
    }
    return 0;
}

void MultibandEQ::setFrequency(uint8_t idx, float f, float sr){

    if ( idx < m_poleCount ){
        
        m_pole[idx].setFrequency(f, sr);
    }
}
void MultibandEQ::setFrequency(uint8_t poleCount, float* poles, float sr){

    if ( poleCount != m_poleCount ){

        delete m_band;
        delete m_pole;

        m_band      = new float[poleCount+1];
        m_bandCount = poleCount+1;
        
        m_pole      = new FilterPole[poleCount];
        m_poleCount = poleCount;
    }

    for ( uint8_t i = 0; i < poleCount; i++ ){

        m_pole[i].setFrequency(poles[i], sr);
    }
}
float MultibandEQ::getFrequency(uint8_t idx) const{

    if ( idx < m_poleCount ){

        return m_pole[idx].getFrequency();
    }
    return 0;
}

uint8_t MultibandEQ::getBandCount() const{

    return m_bandCount;
}
