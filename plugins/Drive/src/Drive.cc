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

extern "C" AbstractEffectUnit* func_builder(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin) {

    return new DriveEffect(typeCode, id, plugin);
}

extern "C" void func_destructor(AbstractEffectUnit* effect) {

    delete effect;
}

/* ***************************** Consts ***************************** */
#define BYP 0
#define VOL 1
#define ASM 2

#define GP  3
#define TP  4
#define SOP 5
#define SHP 6

#define GN  7
#define TN  8
#define SON 9
#define SHN 10

#define ILC 11
#define IHC 12
#define ILG 13
#define IMG 14
#define IHG 15

#define OLC 16
#define OHC 17
#define OLG 18
#define OMG 19
#define OHG 20

bool test = true;

/* *************************** DriveEffect ************************** */
DriveEffect::DriveEffect(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin):
    AbstractEffectUnit( id, typeCode, plugin->pool()),
    m_toneIn(NULL),
    m_toneOut(NULL),
    m_clipP(clipHard),
    m_clipN(clipHard)
{
    
    // Verify that plugin is correctly loaded
    if (plugin->status()) throw std::string("Plugin Incomplete" + std::to_string(plugin->status()));

    // Setup Equalizer, Param used here are not important
    float f[] = {100, 1000};
    m_toneIn  = new GraphicEQ( 2, f, 48000);
    m_toneOut = new GraphicEQ( 2, f, 48000);

    // Setup JACKUnit
    try{

        _jackU = new JACKUnit( SFXP::JACK_SERVER, plugin->name() );

        _jackU->registerPorts( plugin->portNameListList(), plugin->portCountList(), id );

        _jackU->registerCallback( DriveEffect::process, this );
        
        _jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}

DriveEffect::~DriveEffect(){

    // Delete EQ
    delete m_toneIn;
    delete m_toneOut;
}

int DriveEffect::process(jack_nframes_t nframes, void* arg){

    DriveEffect* unit = (DriveEffect*)(arg);

    SFXP::sample_t *in, *out;
    in  = (SFXP::sample_t*)jack_port_get_buffer( unit->_jackU->getPort(SFXP::PortType::AudioIn, 0), nframes );
    out = (SFXP::sample_t*)jack_port_get_buffer( unit->_jackU->getPort(SFXP::PortType::AudioOut, 0), nframes );

    if ( !unit->_poolArray[BYP] ){
        
        float* p = unit->_poolArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Apply tone on input
            out[i] = unit->m_toneIn->compute( in[i], 3, p + ILG );

            // Clip Signal
            if ( out[i] < 0 && p[ASM] ){

                out[i] = (*unit->m_clipN)( out[i]*p[GN], p[SON], p[SHN]);
            }
            else{

                out[i] = (*unit->m_clipP)( out[i]*p[GP], p[SOP], p[SHP]);
            }

            // Apply tone on output
            out[i] = p[VOL] * unit->m_toneOut->compute( out[i], 3, p + OLG );
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(SFXP::sample_t) );
    }
    return 0;
}

void DriveEffect::update(SFXPEvent& event){

    if (event._type == SFXPEvent::Type::Event_EffectEditParam) {
        
        SFXP::usize_t idx = event._effect._idx;
        _poolArray[idx] = event._effect._value;

        if (idx == ILC || idx == IHC) {

            m_toneIn->setFrequency ( idx-ILC, _poolArray[idx], _jackU->getSamplerate() );
        }
        else if (idx == OLC || idx == OHC) {

            m_toneOut->setFrequency ( idx-OLC, _poolArray[idx], _jackU->getSamplerate() );
        }
        else if (idx == TP) {

            m_clipP = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)_poolArray[TP]) );
        }
        else if (idx == TN) {

            m_clipN = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)_poolArray[TN]) );
        }
        
        test = true;
    }
    else if (event._type == SFXPEvent::Type::Event_EffectUpdateAll) {

        this->updateAll();
    }
}

/**
 * Function called when all effect parameters has changed
 **/
void DriveEffect::updateAll() {

    memcpy( _poolArray, _currentBank->second, _poolSize * sizeof( float ) );

    m_toneIn->setFrequency ( 2, _poolArray+ILC, _jackU->getSamplerate() );
    m_toneOut->setFrequency( 2, _poolArray+OLC, _jackU->getSamplerate() );
    
    m_clipP = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)_poolArray[TP]) );
    m_clipN = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)_poolArray[TN]) );
}
