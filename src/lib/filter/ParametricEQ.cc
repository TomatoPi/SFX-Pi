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

using namespace sfx;

ParametricEQ::ParametricEQ(usize_t poleCount, float* poles, float* qf, float samplerate
                        ,sfx::usize_t* orders):
    AFilterBase(0),
    m_band(new float[poleCount]),

    m_qf    (new float  [poleCount]),
    m_cf    (new float  [poleCount]),
    m_orders(new usize_t[poleCount]),
    
    m_poleL(new FilterPole*[poleCount]),
    m_poleH(new FilterPole*[poleCount]),
    m_poleCount(poleCount)
{
    // std::sort( poles, poles + poleCount );
    memcpy(m_orders, orders, poleCount * sizeof(usize_t));
    
    for ( usize_t i = 0; i < poleCount; i++ ){

        m_poleL[i] = new FilterPole();
        m_poleH[i] = new FilterPole();
        this->updatePole( i, poles[i], qf[i], samplerate, orders[i]);
    }
}
ParametricEQ::~ParametricEQ(){

    delete[] m_band;

    delete[] m_qf;
    delete[] m_cf;

    for ( usize_t i = 0; i < m_poleCount; i++ ){

        delete m_poleL[i];
        delete m_poleH[i];
    }
    delete[] m_poleL;
    delete[] m_poleH;
}

/**
* Compute given input and output mix of all bands
* Use getBand function for get a specific band value
**/
sample_t ParametricEQ::compute(sample_t in, usize_t poleCount, float* gains){

    if ( poleCount > m_poleCount ) return in;
    
    // Compute Each Band
    sample_t out = 0;
    for ( usize_t i = 0; i < poleCount; i++ ){
        
        // Each band is a BandPass Filtering between High and Low Pole
        m_band[i] = m_poleH[i]->compute(in) - m_poleL[i]->compute(in);
        
        out += m_band[i] * gains[i];
    }
    
    // Shuffle Buffer
    this->shuffleBuffer( in );
    
    return out;
}
sample_t ParametricEQ::compute(sample_t in, usize_t idx){

    if ( idx < m_poleCount ){

        m_band[idx] = m_poleH[idx]->compute(in) - m_poleL[idx]->compute(in);
        return m_band[idx];
    }
    return in;
}
sample_t ParametricEQ::getBand(usize_t idx){

    if ( idx < m_poleCount ){
        
        return m_band[idx];
    }
    return 0;
}

void ParametricEQ::setPole(usize_t idx, float f, float q, float sr, sfx::usize_t order){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, f, q, sr, order );
    }
}
/**
* Change EQ Configuration
**/
void ParametricEQ::setFrequency(usize_t idx, float f, float sr){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, f, m_qf[idx], sr, m_orders[idx]);
    }
}
void ParametricEQ::setFrequency(usize_t poleCount, float* poles, float sr){

    if ( poleCount == m_poleCount ){
        
        for ( usize_t i = 0; i < poleCount; i++ ){

            this->updatePole( i, poles[i], m_qf[i], sr, m_orders[i] );
        }
    }
}
float ParametricEQ::getFrequency(usize_t idx) const{

    if ( idx < m_poleCount ){
        
        return m_cf[idx];
    }
    return 0;
}

/**
* Change EQ Configuration
**/
void ParametricEQ::setQFactor(usize_t idx, float qf, float sr){

    if ( idx < m_poleCount ){
        
        this->updatePole( idx, m_cf[idx], qf, sr, m_orders[idx]);
    }
}
void ParametricEQ::setQFactor(usize_t poleCount, float* qf, float sr){

    if ( poleCount == m_poleCount ){
        
        for ( usize_t i = 0; i < poleCount; i++ ){

            this->updatePole( i, m_cf[i], qf[i], sr, m_orders[i] );
        }
    }
}
float ParametricEQ::getQFactor(usize_t idx) const{

    if ( idx < m_poleCount ){
        
        return m_qf[idx];
    }
    return 0; // Good error value because Q factor cannot be 0
}

usize_t ParametricEQ::getBandCount() const{

    return m_poleCount;
}

void ParametricEQ::updatePole(usize_t idx, float cf, float qf, float sr, sfx::usize_t order){

    if ( idx < m_poleCount ){

        // Calculate BandWidth in Octave from Q factor
        m_cf[idx] = cf;
        m_qf[idx] = qf;

        #ifdef __DEBUG__
        if ( sfx::GlobalIsDebugEnabled ){
            printf("Center( %3.5f ) : QFactor( %3.5f ) : ",
                m_cf[idx],
                m_qf[idx]
                );
        }
        #endif

        // Bandwith in octave
        float bw = 2.01977305724f * asinh( 1.25f / (qf + AFilterBase::FilterPole::vsa) );
        
        #ifdef __DEBUG__
        if ( sfx::GlobalIsDebugEnabled ){
            printf("BW( %3.5f )\n", bw);
        }
        #endif

        // Scale Poles pairs, centered at cf ( Center Frequency )
        // by converting bandwidth to frequency ratio
        delete m_poleL[idx];
        delete m_poleH[idx];
        
        m_poleL[idx] = new FilterPole(m_cf[idx] * pow( 2, -bw/2.0f ), sr, order);
        m_poleH[idx] = new FilterPole(m_cf[idx] * pow( 2,  bw/2.0f ), sr, order);

        #ifdef __DEBUG__
        if ( sfx::GlobalIsDebugEnabled ){
                
            printf("Filter Updated : lf( %4.5f ) : hf( %4.5f )\n",
                m_poleL[idx]->getFrequency(),
                m_poleH[idx]->getFrequency()
                );
        }
        #endif
    }
}
