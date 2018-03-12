/**********************************************************************
 * @file Chorus.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Simple Chorus effect
 * Takes a signal and add it delayed versions
 **********************************************************************/
#include "Chorus.h"
/* ***************************** Const ****************************** */
#define OFFP    EUCST::C_PARAM_COUNT

#define DPH     (OFFP + 0)
#define FB      (OFFP + 1)
#define DW      (OFFP + 2)
#define SZ      (OFFP + 3)

#define D       (SZ + 1)
#define W       (D  + 5)
#define POS     (W  + 5)

/* ***************************** Const ****************************** */
template<> const std::string ChorusReg::NAME = "Chorus";

template<> AbstractEffectUnit* ChorusReg::BUILDER(id1_t id, id1_t type){

    return new ChorusEffect( id, type );
}

template<> const std::string ChorusReg::PARNAMES[] = {
    "Depth",
    "Feedback",
    "Dry-Wet",
    "Size",
    "Delay1",   "Delay2",   "Delay3",   "Delay4",   "Delay5",
    "Weigth1",  "Weigth2",  "Weigth3",  "Weigth4",  "Weigth5"
    };
template<> const usize_t ChorusReg::PARCOUNT = 14;

template<> const std::string ChorusReg::PORNAMES[] = {"Input","Mod","Output"};

template<> const usize_t ChorusReg::AI = 2;
template<> const usize_t ChorusReg::AO = 1;
template<> const usize_t ChorusReg::MI = 0;
template<> const usize_t ChorusReg::MO = 0;

template<> const usize_t ChorusReg::PARSIZE = ChorusReg::PARCOUNT + 5;

/* ****************************** Chorus **************************** */
ChorusEffect::ChorusEffect(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_buffer(new Buffer<BUFFER_SIZE>())
{
    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit( EUCST::JACK_SERVER, NAME );

        m_jackU->registerPorts( PORNAMES , AI, AO );

        m_jackU->registerCallback( process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
ChorusEffect::~ChorusEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    delete m_buffer;
}

int ChorusEffect::process(jack_nframes_t nframes, void* arg){

    ChorusEffect* unit = (ChorusEffect*)(arg);

    sample_t *in, *mod, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    mod = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[2], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float* p = unit->m_paramArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Get Delayed signal back
            sample_t del = 0;

            float depth = p[DPH] * mod[i];

            for ( uint8_t k = 0; k < p[SZ]; k++ ){

                // Change delay k value by depth amount
                p[POS+k] += depth;

                // Delay is now non integer so get linear interpolation
                // of wanted sample
                del += p[W+k] * unit->m_buffer->getLI( p[POS+k] );
            }

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
    
void ChorusEffect::update(){

    memcpy( m_paramArray, m_currentBank->second, m_bankSize * sizeof( float ) );

    memcpy( m_paramArray + POS, m_paramArray + D, 5 * sizeof( float ) );
}
