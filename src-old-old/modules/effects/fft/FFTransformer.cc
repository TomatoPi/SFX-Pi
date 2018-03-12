/**********************************************************************
 * @file FFTransformer.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that transform time domain signal to frequency domain
 * This effect contains two JACKUnits , the first one is the real to
 * complex tranformer, the second one is the complex to real transformer
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#include "FFTransformer.h"
/* ***************************** Consts ***************************** */
template<>  const std::string FFTCReg::NAME =  "FFT-Complexifier";

template<> AbstractEffectUnit* FFTCReg::BUILDER(id1_t id, id1_t type){

    return new FFTComplexifier( id, type );
}

template<> const std::string FFTCReg::PARNAMES[] = {};
template<> const usize_t FFTCReg::PARCOUNT = 0;

template<> const std::string FFTCReg::PORNAMES[] = {
    "Input-Real", "Output-Complex"};

template<> const usize_t FFTCReg::AI = 1;
template<> const usize_t FFTCReg::AO = 1;
template<> const usize_t FFTCReg::MI = 0;
template<> const usize_t FFTCReg::MO = 0;

template<> const usize_t FFTCReg::PARSIZE = FFTCReg::PARCOUNT;

/* ***************************** Consts ***************************** */
template<>  const std::string FFTRReg::NAME =  "FFT-Realizer";

template<> AbstractEffectUnit* FFTRReg::BUILDER(id1_t id, id1_t type){

    return new FFTRealizer( id, type );
}

template<> const std::string FFTRReg::PARNAMES[] = {};
template<> const usize_t FFTRReg::PARCOUNT = 0;

template<> const std::string FFTRReg::PORNAMES[] = {
    "Input-Complex", "Output-Real"};

template<> const usize_t FFTRReg::AI = 1;
template<> const usize_t FFTRReg::AO = 1;
template<> const usize_t FFTRReg::MI = 0;
template<> const usize_t FFTRReg::MO = 0;

template<> const usize_t FFTRReg::PARSIZE = FFTRReg::PARCOUNT;

/* ************************* FFTComplexifier ************************ */
FFTComplexifier::FFTComplexifier(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_ffft(new ffft::FFTRealFixLen<SFXP::JACK_BLOCK_SIZE>())
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
FFTComplexifier::~FFTComplexifier(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    delete m_ffft;
}

int FFTComplexifier::process(jack_nframes_t nframes, void* arg){

    FFTComplexifier* unit = (FFTComplexifier*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        unit->m_ffft->do_fft(out, in);
    }
    return 0;
}
    
void FFTComplexifier::update(){

    memcpy( m_paramArray, m_currentBank->second, m_paramSize * sizeof( float ) );
}

/* *************************** FFTRealizer ************************** */
FFTRealizer::FFTRealizer(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_ffft(new ffft::FFTRealFixLen<SFXP::JACK_BLOCK_SIZE>())
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
FFTRealizer::~FFTRealizer(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    delete m_ffft;
}
int FFTRealizer::process(jack_nframes_t nframes, void* arg){

    FFTRealizer* unit = (FFTRealizer*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        unit->m_ffft->do_ifft(in, out);
        unit->m_ffft->rescale(out);
    }
    return 0;
}
    
void FFTRealizer::update(){

    memcpy( m_paramArray, m_currentBank->second, m_paramSize * sizeof( float ) );
}
