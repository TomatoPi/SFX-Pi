/**********************************************************************
 * @file Reverb.h
 * @author TomatoPi
 * @version 1.0
 *
 * First attempt of reverb effect
 * Based on multiple small delay lengths summ, then summ is filtered
 * to remove clicks caused by audible delay between each lengths
 **********************************************************************/
#include "Reverb.h"
/* ***************************** Const ****************************** */
#define OFFP    EUCST::C_PARAM_COUNT

#define FR      (OFFP + 0)
#define LR      (OFFP + 1)
#define RP      (OFFP + 2)
#define FB      (OFFP + 3)
#define DW      (OFFP + 4)

#define DEL     (DW + 1)

/* ***************************** Const ****************************** */
template<> const std::string ReverbReg::NAME = "Reverb";

template<> AbstractEffectUnit* ReverbReg::BUILDER(id1_t id, id1_t type){

    return new ReverbEffect( id, type );
}

template<> const std::string ReverbReg::PARNAMES[] = {
    "First-Reflection",
    "Last-Reflection",
    "Repartition",
    "Feedback",
    "Dry-Wet"
    };
template<> const size_t ReverbReg::PARCOUNT = 5;

template<> const std::string ReverbReg::PORNAMES[] = {
    "Input",
    "Output","Reverb"
    };

template<> const size_t ReverbReg::AI = 1;
template<> const size_t ReverbReg::AO = 2;
template<> const size_t ReverbReg::MI = 0;
template<> const size_t ReverbReg::MO = 0;

template<> const size_t ReverbReg::PARSIZE = ReverbReg::PARCOUNT
                                            + ReverbEffect::LINE_COUNT;
//
/* ******************************* Delay **************************** */
ReverbEffect::ReverbEffect(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_buffer(new Buffer<MAX_TIME>())
{
    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit( EUCST::JACK_SERVER, NAME );

        m_jackU->registerPorts( PORNAMES, AI, AO );

        m_jackU->registerCallback( process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
ReverbEffect::~ReverbEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    delete m_buffer;
}

int ReverbEffect::process(jack_nframes_t nframes, void* arg){

    ReverbEffect* unit = (ReverbEffect*)(arg);

    sample_t *in, *out, *rev;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );
    rev = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[2], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float* p = unit->m_paramArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Collect Delay Lines
            sample_t del = 0;
            for ( uint8_t k = 0; k < LINE_COUNT; k++ ){
                
                del += unit->m_buffer->get(p[DEL+k]);
            }

            // Output reverb signal with clear input on main output
            out[i] = p[EUCST::IDXVOL] * utils::drywet( in[i], del, p[DW] );

            // Outputpure reverb signal on reverb out
            rev[i] = del;

            // Write input + feedback inside buffer
            unit->m_buffer->write(in[i] + p[FB]*del);
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(sample_t) );
    }
    return 0;
}
    
void ReverbEffect::update(){

    // First Copy basic parameters
    memcpy( m_paramArray, m_currentBank->second, m_bankSize * sizeof( float ) );

    // Constrain delays values between alowed ones
    m_paramArray[FR] = utils::constrain(m_paramArray[FR], MIN_TIME, MAX_TIME);
    m_paramArray[LR] = utils::constrain(m_paramArray[LR], MIN_TIME, MAX_TIME);

    // Constrain repartition value between 0 excluded and 30
    m_paramArray[RP] = utils::constrain(m_paramArray[RP], 0.0001f, 30);
    float w = m_paramArray[RP];

    // Fill delays bins
    for ( size_t i = 0; i < LINE_COUNT; i++){

        m_paramArray[DEL+i] = utils::map(
            log10(utils::map(i,0,LINE_COUNT,1,w))/log10(w),
            0,
            1,
            m_paramArray[FR],
            m_paramArray[LR]
            );
            
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Delay %lu = %3.5f\n",i,m_paramArray[DEL+i]);
        }
    }
}
