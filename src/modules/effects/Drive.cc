/**********************************************************************
 * @file Drive.h
 * @author TomatoPi
 * @version 1.0
 *
 * Basic Distortion Effect.
 * Clip given Signal using hard clipping or dual-tanh Soft Clipping
 * Signal is passed througth a 3Bands EQ before and after clipping
 * Different Settings can be set for positives and negatives half-wave
 **********************************************************************/
#include "Drive.h"
/* ***************************** Consts ***************************** */
#define OFFP    EUCST::C_PARAM_COUNT
#define ASM     (OFFP + 0)

#define GP      (OFFP + 1)
#define TP      (OFFP + 2)
#define SOP     (OFFP + 3)
#define SHP     (OFFP + 4)

#define GN      (OFFP + 5)
#define TN      (OFFP + 6)
#define SON     (OFFP + 7)
#define SHN     (OFFP + 8)

#define ILC     (OFFP + 9)
#define IHC     (OFFP + 10)
#define ILG     (OFFP + 11)
#define IMG     (OFFP + 12)
#define IHG     (OFFP + 13)

#define OLC     (OFFP + 14)
#define OHC     (OFFP + 15)
#define OLG     (OFFP + 16)
#define OMG     (OFFP + 17)
#define OHG     (OFFP + 18)

const std::string DriveReg::NAME = "Drive";

AbstractEffectUnit* DriveReg::BUILDER(uint8_t id, uint8_t type){

    return new DriveEffect( id, type );
}

const std::string DriveReg::PARNAMES[] = {"Asymetrical",
    "Gain-p", "Type-p", "Soft-p", "Shape-p",
    "Gain-n", "Type-n", "Soft-n", "Shape-n",
    "In-Lowcut", "In-Highcut", "In-Lowgain", "In-Midgain", "In-Highgain",
    "Out-Lowcut", "Out-Highcut", "Out-Lowgain", "Out-Midgain", "Out-Highgain"
    };
const uint8_t DriveReg::PARCOUNT = 19;

const std::string DriveReg::PORNAMES[] = {"Input", "Output"};
const uint8_t DriveReg::AI = 1;
const uint8_t DriveReg::AO = 1;
const uint8_t DriveReg::MI = 0;
const uint8_t DriveReg::MO = 0;

const uint8_t DriveReg::PARSIZE = DriveReg::PARCOUNT;

/* *************************** DriveEffect ************************** */
DriveEffect::DriveEffect(uint8_t id, uint8_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_toneIn(NULL),
    m_toneOut(NULL),
    m_samplerate(48000),
    m_clipP(clipHard),
    m_clipN(clipHard)
{

    // Setup Equalizer, Param used here are not important
    float f[] = {100, 1000};
    m_toneIn  = new GraphicEQ( 2, f, 48000);
    m_toneOut = new GraphicEQ( 2, f, 48000);

    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit( EUCST::JACK_SERVER, DriveReg::NAME );

        m_jackU->registerPorts( DriveReg::PORNAMES );

        m_jackU->registerCallback( DriveEffect::process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}

DriveEffect::~DriveEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    // Delete EQ
    delete m_toneIn;
    delete m_toneOut;
}

int DriveEffect::process(jack_nframes_t nframes, void* arg){

    DriveEffect* unit = (DriveEffect*)(arg);

    sample_t *in, *out;
    in  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    if ( !unit->m_paramArray[EUCST::C_IDX_BYPASS] ){

        float* p = unit->m_paramArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Apply tone on input
            out[i] = unit->m_toneIn->compute( in[i], 3, p + ILG );

            // Clip Signal
            if ( out[i] > 0 ){

                out[i] = (*unit->m_clipP)( out[i]*p[GP], p[SOP], p[SHP]);
            }
            else{

                out[i] = (*unit->m_clipN)( out[i]*p[GN], p[SON], p[SHN]);
            }

            // Apply tone on output
            out[i] = unit->m_toneOut->compute( out[i], 3, p + OLG );
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(sample_t) );
    }
    return 0;
}

void DriveEffect::update(){

    memcpy( m_paramArray, m_currentBank->second, m_paramSize * sizeof( float ) );

    m_toneIn->setFrequency ( 2, m_paramArray+ILC, m_jackU->getSamplerate() );
    m_toneOut->setFrequency( 2, m_paramArray+OLC, m_jackU->getSamplerate() );

    m_clipP = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)m_paramArray[TP]) );
    m_clipN = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)m_paramArray[TN]) );
}
