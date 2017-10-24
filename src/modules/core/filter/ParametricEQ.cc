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

ParametricEQ::ParametricEQ(uint8_t poleCount, float* poles, float* qf, float samplerate):
    AFilterBase(),
    m_band(new float[poleCount]),

    m_qf(new float[poleCount]),
    m_cf(new float[poleCount]),
    
    m_poleL(new FilterPole[poleCount]),
    m_poleH(new FilterPole[poleCount]),
    m_poleCount(poleCount)
{
    std::sort( poles, poles + poleCount );
    for ( uint8_t i = 0; i < poleCount; i++ ){

        this->updatePole( i, poles[i], qf[i], samplerate );
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
float ParametricEQ::compute(float in, uint8_t poleCount, float* gains){

    if ( poleCount > m_poleCount ) return in;
    
    // Compute Each Band
    float out = 0;
    for ( uint8_t i = 0; i < poleCount; i++ ){
        
        // Each band is a BandPass Filtering between High and Low Pole
        m_band[i] = m_poleH[i].compute(in) - m_poleL[i].compute(in);
        
        out += m_band[i] * gains[i];
    }
    
    // Shuffle Buffer
    this->shuffleBuffer( in );
    
    return out;
}
float ParametricEQ::compute(float in, uint8_t idx){

    if ( idx < m_poleCount ){

        m_band[idx] = m_poleH[idx].compute(in) - m_poleL[idx].compute(in);
        return m_band[idx];
    }
    return in;
}
float ParametricEQ::getBand(uint8_t idx){

    if ( idx < m_poleCount ){
        
        return m_band[idx];
    }
    return 0;
}

void ParametricEQ::setPole(uint8_t idx, float f, float q, float sr){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, f, q, sr );
    }
}
/**
* Change EQ Configuration
**/
void ParametricEQ::setFrequency(uint8_t idx, float f, float sr){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, f, m_qf[idx], sr );
    }
}
void ParametricEQ::setFrequency(uint8_t poleCount, float* poles, float sr){

    if ( poleCount == m_poleCount ){
        
        for ( uint8_t i = 0; i < poleCount; i++ ){

            this->updatePole( i, poles[i], m_qf[i], sr );
        }
    }
}
float ParametricEQ::getFrequency(uint8_t idx) const{

    if ( idx < m_poleCount ){
        
        return m_cf[idx];
    }
    return 0;
}

/**
* Change EQ Configuration
**/
void ParametricEQ::setQFactor(uint8_t idx, float qf, float sr){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, m_cf[idx], qf, sr);
    }
}
void ParametricEQ::setQFactor(uint8_t poleCount, float* qf, float sr){

    if ( poleCount == m_poleCount ){
        
        for ( uint8_t i = 0; i < poleCount; i++ ){

            this->updatePole( i, m_cf[i], qf[i], sr );
        }
    }
}
float ParametricEQ::getQFactor(uint8_t idx) const{

    if ( idx < m_poleCount ){
        
        return m_qf[idx];
    }
    return 0; // Good error value because Q factor cannot be 0
}

uint8_t ParametricEQ::getBandCount() const{

    return m_poleCount;
}

void ParametricEQ::updatePole(uint8_t idx, float cf, float qf, float sr){

    if ( idx < m_poleCount ){

        // Calculate BandWidth in Octave from Q factor
        m_cf[idx] = cf;
        m_qf[idx] = qf;

        std::cout << "Center(" << std::to_string(m_cf[idx]) << ") : ";
        std::cout << "QFactor(" << std::to_string(m_qf[idx]) << ")" << std::endl;

        // Bandwith in octave
        float bw = 2.01977305724f * asinh( 1.25f / (qf + AFilterBase::FilterPole::vsa) );
        std::cout << "BW(" << std::to_string(bw) << ")" << std::endl;

        // Scale Poles pairs, centered at cf ( Center Frequency )
        // by converting bandwidth to frequency ratio
        m_poleL[idx] = FilterPole(m_cf[idx] * pow( 2, -bw/2.0f ), sr);
        m_poleH[idx] = FilterPole(m_cf[idx] * pow( 2,  bw/2.0f ), sr);

        std::cout << "Filter Updated : lf(" << std::to_string(m_poleL[idx].getFrequency());
        std::cout << ") : hf(" << std::to_string(m_poleH[idx].getFrequency());
        std::cout << ")" << std::endl;
    }
}
