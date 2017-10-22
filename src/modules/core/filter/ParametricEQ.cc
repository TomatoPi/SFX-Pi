/**********************************************************************
 * @file ParametricEQ.cc
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
#include "ParametricEQ.h"

const float ln22 = 2.88539008f; // 2/ln(2)

ParametricEQ::ParametricEQ(uint8_t poleCount, float* poles, float* qf, float samplerate):
    AFilterBase(),
    m_band(new float[poleCount+1]),
    m_bandCount(poleCount+1),

    m_qf(new float[poleCount]),
    m_cf(new float[poleCount]),
    
    m_poleL(new FilterPole[poleCount]),
    m_poleH(new FilterPole[poleCount]),
    m_poleCount(poleCount)

{
    std::sort( poles, poles + poleCount );
    for ( uint8_t i = 0; i < poleCount; i++ ){

        this->updatePole( i, poles[i], qf[i] );
    }
}
ParametricEQ::~ParametricEQ(){

    delete m_band;

    delete m_qf;
    delete m_cf;

    delete m_poleL;
    delete m_poleH;
}

/**
* Compute given input and output mix of all bands
* Use getBand function for get a specific band value
**/
float ParametricEQ::compute(float in, uint8_t bandCount, float* gains){

}
float ParametricEQ::getBand(uint8_t idx){

}

/**
* Change EQ Configuration
**/
void ParametricEQ::setFrequency(uint8_t idx, float f, float sr){

}
void ParametricEQ::setFrequency(uint8_t poleCount, float* poles, float sr){

}
float ParametricEQ::getFrequency(uint8_t idx) const{

}

/**
* Change EQ Configuration
**/
void ParametricEQ::setQFactor(uint8_t idx, float qf, float sr){

}
void ParametricEQ::setQFactor(uint8_t poleCount, float* qf, float sr){

}
float ParametricEQ::getQFactor(uint8_t idx) const{

}

uint8_t ParametricEQ::getBandCount() const{

    return m_bandCount;
}

void ParametricEQ::updatePole(uint8_t idx, float cf, float qf){

    if ( idx < m_poleCount ){

        // Calculate BandWidth in Octave from Q factor
        m_cf[idx] = cf;
        m_qf[idx] = qf;

        float bw = ln22 * asinh( 0.5f / (qf + AFilterBase::FilterPole::vsa) );

        // Scale Poles pairs, centered at cf ( Center Frequency )
        m_poleL[idx] = FilterPole(m_cf[idx] * pow( 2, -bw/2.0f ), samplerate);
        m_poleH[idx] = FilterPole(m_cf[idx] * pow( 2,  bw/2.0f ), samplerate);
    }
}
