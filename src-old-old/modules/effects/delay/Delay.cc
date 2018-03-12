/**********************************************************************
 * @file Delay.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Simple Delay effect
 * Take normal signal and delay it, delayed signal is then mixed with
 * normal signal
 **********************************************************************/
#include "Delay.h"
/* ***************************** Const ****************************** */
#define OFFP    EUCST::C_PARAM_COUNT

#define D       (OFFP + 0)
#define FB      (OFFP + 1)
#define DW      (OFFP + 2)

/* ***************************** Const ****************************** */
template<> const std::string DelayReg::NAME = "Delay";

template<> AbstractEffectUnit* DelayReg::BUILDER(id1_t id, id1_t type){

    return new DelayEffect( id, type );
}

template<> const std::string DelayReg::PARNAMES[] = {
    "Delay",
    "Feedback",
    "Dry-Wet"
    };
template<> const usize_t DelayReg::PARCOUNT = 3;

template<> const std::string DelayReg::PORNAMES[] = {"Input","Output"};

template<> const usize_t DelayReg::AI = 1;
template<> const usize_t DelayReg::AO = 1;
template<> const usize_t DelayReg::MI = 0;
template<> const usize_t DelayReg::MO = 0;

template<> const usize_t DelayReg::PARSIZE = DelayReg::PARCOUNT;

/* ******************************* Delay **************************** */
DelayEffect::DelayEffect(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_buffer(new Buffer<MAX_DELAY>())
{
    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit( EUCST::JACK_SERVER, NAME );

        m_jackU->registerPorts( PORNAMES );

        m_jackU->registerCallback( process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
DelayEffect::~DelayEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    delete m_buffer;
}

int DelayEffect::process(jack_nframes_t nframes, void* arg){

    DelayEffect* unit = (DelayEffect*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float* p = unit->m_paramArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Get Delayed signal back
            sample_t del = unit->m_buffer->get(p[D]);

            // Output delayed signal with clear input
            out[i] = p[EUCST::IDXVOL] * utils::drywet( in[i], del, p[DW] );

            // Write input + feedback inside buffer
            unit->m_buffer->write(in[i] + p[FB]*del);
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(sample_t) );
    }
    return 0;
}

void DelayEffect::update(){

    memcpy( m_paramArray, m_currentBank->second, m_bankSize * sizeof( float ) );

    m_paramArray[D] = utils::constrain(
        utils::mstos(m_paramArray[D],m_jackU->getSamplerate()),
        0,
        MAX_DELAY);
}
