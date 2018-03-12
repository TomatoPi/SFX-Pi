/**********************************************************************
 * @file Delay.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Simple Delay effect
 * Take normal signal and delay it, delayed signal is then mixed with
 * normal signal
 * This delay doesn't allow to insert effects inside the Feedback loop
 **********************************************************************/
#include "Delay.h"

extern "C" AbstractEffectUnit* func_builder(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin) {

    return new DelayEffect(typeCode, id, plugin);
}

extern "C" void func_destructor(AbstractEffectUnit* effect) {

    delete effect;
}

/* ***************************** Const ****************************** */
#define BYP 0   // Bypass
#define VOL 1   // Volume

#define D   2   // Delay's length
#define FB  3   // Delay's feedback
#define DW  4   // Delay's dry wet

/* ******************************* Delay **************************** */
DelayEffect::DelayEffect(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin):
    AbstractEffectUnit( id, typeCode, plugin->pool()),
    _buffer(nullptr),
    _samplerate(0)
{
    // Setup JACKUnit
    try{

        _jackU = new JACKUnit( SFXP::JACK_SERVER, plugin->name() );

        _jackU->registerPorts( plugin->portNameListList(), plugin->portCountList(), id );

        _jackU->registerCallback( DelayEffect::process, this );

        // Create the buffer
        _samplerate = _jackU->getSamplerate();
        _buffer = new Buffer_S(10, _samplerate);
        
        _jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
DelayEffect::~DelayEffect(){

    delete _buffer;
}

int DelayEffect::process(jack_nframes_t nframes, void* arg){

    DelayEffect* unit = (DelayEffect*)(arg);

    SFXP::sample_t *in, *out;
    in  = (SFXP::sample_t*)jack_port_get_buffer( unit->_jackU->getPort(SFXP::PortType::AudioIn, 0), nframes );
    out = (SFXP::sample_t*)jack_port_get_buffer( unit->_jackU->getPort(SFXP::PortType::AudioOut, 0), nframes );

    if ( !unit->_poolArray[BYP] ){

        float* p = unit->_poolArray;

        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Get Delayed signal back
            SFXP::sample_t del = unit->_buffer->read();

            // Output delayed signal with clear input
            out[i] = p[VOL] * utils::drywet( in[i], del, p[DW] );

            // Write input + feedback inside buffer
            unit->_buffer->write(in[i] + p[FB]*del);
        }
    }
    else{

        memcpy( out, in, nframes * sizeof(SFXP::sample_t) );
    }
    return 0;
}

/**
 * Function Called When a parameter is modified or bank changed
 * It will update the Param Array passed to callback function
 * Pure Virtual function, must be redefined by every effect type
 **/
void DelayEffect::update(SFXPEvent& event)
{
    if (event._type == SFXPEvent::Type::Event_EffectEditParam) {

        SFXP::usize_t idx = event._effect._idx;
        _poolArray[idx] = event._effect._value;

        if (idx == D) {
            
            _buffer->set_length(_poolArray[D], _samplerate);
        }
    }
    else if (event._type == SFXPEvent::Type::Event_EffectUpdateAll) {

        this->updateAll();
    }
}

/**
 * Function called when all effect parameters has changed
 **/
void DelayEffect::updateAll()
{

    memcpy(_poolArray, _currentBank->second, _poolSize * sizeof(float));

    _samplerate = _jackU->getSamplerate();
    _buffer->set_length(_poolArray[D], _samplerate);
}
