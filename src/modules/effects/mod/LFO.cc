/**********************************************************************
 * @file LFO.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Low Frequency Oscillator effect
 * Effect used to generate modulation signals for effects like chorus
 * or ring modulator ( tremolo )
 **********************************************************************/
#include "LFO.h"
/* ***************************** Const ****************************** */
#define OFFP    EUCST::C_PARAM_COUNT

#define WAV     (OFFP + 0)
#define FRQ     (OFFP + 1)
#define PHA     (OFFP + 2)
#define SGN     (OFFP + 3)

#define P1      (OFFP + 4)
#define P2      (OFFP + 5)
#define PHD     (OFFP + 6)
#define PHF     (OFFP + 7)

#define PRAMP   (OFFP + 8)
#define PRBS    (OFFP + 9)

/* ***************************** Const ****************************** */
template<> const std::string LFOReg::NAME = "LFO";

template<> AbstractEffectUnit* LFOReg::BUILDER(id1_t id, id1_t type){

    return new LFOEffect( id, type );
}

template<> const std::string LFOReg::PARNAMES[] = {
    "Waveform",
    "Frequency",
    "Phase",
    "Sign",
    "Parameter-1",
    "Parameter-2",
    "Phase-Distortion",
    "Phase-Fix"
    };
template<> const size_t LFOReg::PARCOUNT = 8;

template<> const std::string LFOReg::PORNAMES[] = {"Sync","Output"};

template<> const size_t LFOReg::AI = 1;
template<> const size_t LFOReg::AO = 1;
template<> const size_t LFOReg::MI = 0;
template<> const size_t LFOReg::MO = 0;

template<> const size_t LFOReg::PARSIZE = LFOReg::PARCOUNT +2;

/* ******************************** LFO ***************************** */
LFOEffect::LFOEffect(id1_t id, id1_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_wave(castWaveform(WSIN))
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
LFOEffect::~LFOEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();
}
        
int LFOEffect::process(jack_nframes_t nframes, void* arg){

    LFOEffect* unit = (LFOEffect*)(arg);

    sample_t *sync, *out;
    sync  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        // Get params back
        float* p = unit->m_paramArray;
        
        float d = p[PHD];
        float o = p[PHF];

        float s = (p[SGN]<0)?-1:1;

        // Non zero value on sync port will cause lfo to reset
        if ( !!sync[0] ){

            printf("Sync LFO ...\n");
            p[PRBS] = p[FRQ] / unit->m_jackU->getSamplerate();
        }

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Calculate ramp
            p[PRAMP] += p[PRBS];
            p[PRAMP] = fmod(p[PRAMP] - o, 1.0f);

            // Compute Phase Distortion
            float pp = p[PRAMP];
            if ( d != 0.5f ){
                pp = (pp>d)? (pp+1.0f - 2.0f*d)/(2.0f*(1.0f-d)) : pp/(2.0f*d);
                pp += o;
                pp = fmod(pp, 1.0f);
            }

            // Compute waveform on output
            out[i] = p[EUCST::IDXVOL] * (*unit->m_wave)(pp, s, p[P1], p[P2]);
        }
    }
    else{

        memset( out, 0, nframes * sizeof(sample_t) );
    }
    return 0;
}

void LFOEffect::update(){

    memcpy( m_paramArray, m_currentBank->second, m_bankSize * sizeof( float ) );

    // Update waveform function
    m_wave = castWaveform(static_cast<WAVEFORM>(m_paramArray[WAV]));

    // Sync ramp with lfo phase
    m_paramArray[PRAMP] = m_paramArray[PHA];

    // Update ramp grow factor
    m_paramArray[PRBS] = m_paramArray[FRQ] / m_jackU->getSamplerate();
}

LFOEffect::waveform LFOEffect::castWaveform(WAVEFORM wave){

    if ( wave == WSIN ) return w_sin;
    if ( wave == WSQR ) return w_sqr;
    if ( wave == WTRI ) return w_tri;
    if ( wave == WSAW ) return w_saw;
    if ( wave == WVAR ) return w_var;
    if ( wave == WNPH ) return w_nph;
    if ( wave == WWHI ) return w_whi;
    if ( wave == WCST ) return w_cst;

    return w_sin;
}
