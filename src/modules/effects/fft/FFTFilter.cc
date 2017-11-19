/**********************************************************************
 * @file FFTFilter.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that perform filtering on signal in frequency domain
 * Parameters are weigths of each bins
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#include "FFTFilter.h"

#define OFFP    EUCST::C_PARAM_COUNT

#define FRQ     (OFFP + 0)
#define ORD     (OFFP + 1)
#define TYP     (OFFP + 2)    

namespace{

    const size_t BIN = 1 << (SFXP::JACK_BLOCK_SIZE-1);
}
/* ***************************** Consts ***************************** */
template<>  const std::string FFTFilterReg::NAME =  "FFT-Filter";

template<> AbstractEffectUnit* FFTFilterReg::BUILDER(id1_t id, id1_t type){

    return new FFTFilter( id, type );
}

template<> const std::string FFTFilterReg::PARNAMES[] = {
    "Frequency",
    "Order",
    "Type"
    };
template<> const size_t FFTFilterReg::PARCOUNT = 3;

template<> const std::string FFTFilterReg::PORNAMES[] = {
    "Input", "Output"};

template<> const size_t FFTFilterReg::AI = 1;
template<> const size_t FFTFilterReg::AO = 1;
template<> const size_t FFTFilterReg::MI = 0;
template<> const size_t FFTFilterReg::MO = 0;

template<> const size_t FFTFilterReg::PARSIZE = BIN;
/* ************************* FFTComplexifier ************************ */
/**
 * Constructor and Process Callback
 **/
FFTFilter::FFTFilter(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE)
{
    // Setup JACKUnit
    try{

        // Register Real to Complex Transformer
        m_jackU = new JACKUnit( EUCST::JACK_SERVER, NAME );

        m_jackU->registerPorts( PORNAMES );

        m_jackU->registerCallback( process, this );

        // Activate Units
        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
FFTFilter::~FFTFilter(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();
}

int FFTFilter::process(jack_nframes_t nframes, void* arg){

    FFTFilter* unit = (FFTFilter*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float* p = unit->m_paramArray;

        for ( jack_nframes_t i = 0; i < nframes/2; i++ ){

            // Scale real Part
            out[i] = p[i] * in[i];

            // Scale complex Part
            out[nframes/2 + i] = p[i] * in[nframes/2 + i];
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(sample_t) );
    }
    return 0;
}

void FFTFilter::update(){

    // Get Parameters
    FilterType type = static_cast<FilterType>(m_currentBank->second[TYP]);
    float      order  = m_currentBank->second[ORD];
    float      freq   = m_currentBank->second[FRQ];

    float sr2 = m_jackU->getSamplerate() / 2.0f;

    // If filter is highpass
    if ( type == HIGHPASS ){

        for ( size_t i = 0; i < m_paramSize; i++ ){

            // Calculate filter coef
            float x = ((float)(i)*sr2) / ((float)(BIN)*freq);

            // Calculate pass coef
            m_paramArray[i+OFFP] = x/sqrt(1+(x*x));

            // Compute order
            m_paramArray[i+OFFP] = pow(m_paramArray[i+OFFP], order);
        }
    }
    // If filter is bandpass
    else if ( type == BANDPASS ){

        for ( size_t i = 0; i < m_paramSize-OFFP; i++ ){

            // Calculate filter coef
            float x = ((float)(BIN)*freq) / ((float)(i)*sr2);
            
            // Calculate pass coef
            m_paramArray[i+OFFP] = 2.0f*(x/(1+(x*x)));

            // Compute order
            m_paramArray[i+OFFP] = pow(m_paramArray[i+OFFP], order);
        }
    }
    // Else filter is by default lowpass
    else{

        for ( size_t i = 0; i < m_paramSize-OFFP; i++ ){

            // Calculate filter coef
            float x = ((float)(i)*sr2) / ((float)(BIN)*freq);

            // Calculate pass coef
            m_paramArray[i+OFFP] = 1.0f/sqrt(1+(x*x));

            // Compute order
            m_paramArray[i+OFFP] = pow(m_paramArray[i+OFFP], order);
        }
    }
}
