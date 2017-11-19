/**********************************************************************
 * @file FFTDelay.h
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that delay frequency domain signal with a different
 * delay for each bins
 * Resulting signal harmonics appear in a different order that original
 * one
 * At the moment individual delays are random, using two parameters
 * Delay and spread, determinating repartition around given delay
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#include "FFTDelay.h"

#define OFFP    EUCST::C_PARAM_COUNT

#define DEL     (OFFP + 0)
#define SPR     (OFFP + 1)
#define FBB     (OFFP + 2)
#define DWB     (OFFP + 3)

#define FBP     (OFFP + FFTDelay::BIN/2)
#define DWP     (FBP + 1)

/* ***************************** Consts ***************************** */
template<>  const std::string FFTDelayReg::NAME =  "FFT-Delay";

template<> AbstractEffectUnit* FFTDelayReg::BUILDER(id1_t id, id1_t type){

    return new FFTDelay( id, type );
}

template<> const std::string FFTDelayReg::PARNAMES[] = {
    "Delay",
    "Spread",
    "Feedback",
    "Dry-Wet"
    };
template<> const size_t FFTDelayReg::PARCOUNT = 4;

template<> const std::string FFTDelayReg::PORNAMES[] = {
    "Input", "Output"};

template<> const size_t FFTDelayReg::AI = 1;
template<> const size_t FFTDelayReg::AO = 1;
template<> const size_t FFTDelayReg::MI = 0;
template<> const size_t FFTDelayReg::MO = 0;

template<> const size_t FFTDelayReg::PARSIZE = FFTDelay::BIN/2 + 2;
/* ***************************** FFTDelay *************************** */
FFTDelay::FFTDelay(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_buffer(new BlockBuffer<sample_t,BIN,BIN_COUNT>())
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
FFTDelay::~FFTDelay(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();
    
    delete m_buffer;
}

int FFTDelay::process(jack_nframes_t nframes, void* arg){

    FFTDelay* unit = (FFTDelay*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float* p = unit->m_paramArray;
        BlockBuffer<sample_t,BIN,BIN_COUNT> *buff = unit->m_buffer;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Get delayed bin
            sample_t del = buff->get(p[OFFP+i])[i];

            // Dry wet between delayed bin and new one
            out[i] = utils::drywet( in[i], del, p[DWP] );

            // Feedback delayed bin inside input
            in[i] += del * p[FBP];
        }

        // Write new block inside buffer
        buff->write(in, nframes);
    }
    else{

        memcpy( out, in, nframes * sizeof(sample_t) );
    }
    return 0;
}
    
void FFTDelay::update(){

    // Copy feedback and drywet
    m_paramArray[FBP] = m_currentBank->second[FBB];
    m_paramArray[DWP] = m_currentBank->second[DWB];

    // Get Parameters
    float sr = m_jackU->getSamplerate();
    float delay  = utils::mstos(m_currentBank->second[DEL], sr)/BIN;
    float spread = utils::mstos(m_currentBank->second[SPR], sr)/BIN;

    
    if ( SFXP::GlobalIsDebugEnabled ){
        printf("Fill Bin :\n");
    }

    m_paramArray[OFFP + 0] = utils::constrain( delay , 0, BIN_COUNT);
    
    // Fill bin with random delays values between 0 and MAX_DELAY
    for ( size_t i = 1; i < BIN/2; i++ ){

        m_paramArray[OFFP + i] =
            utils::constrain( m_paramArray[OFFP+i-1] + spread*utils::frand(-1,1), 0, BIN_COUNT);

            
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("%lu(bins) = %f(ms)\n",
                (size_t)m_paramArray[OFFP + i],
                utils::stoms(m_paramArray[OFFP + i]*BIN, sr)
                );
        }
    }
}
