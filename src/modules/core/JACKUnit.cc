/**********************************************************************
 * @file JACKUnit.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing JACK Side of Effect Units
 **********************************************************************/
#include "JACKUnit.h"
namespace{

    const uint8_t FLAG_NULL         = 0x00;
    const uint8_t FLAG_NO_CLIENT    = 0x01;
    const uint8_t FLAG_NO_PORTS     = 0x02;
}

/* ****************************************************************** */
/* **************************** Constructor ************************* */
/* ****************************************************************** */
JACKUnit::JACKUnit(std::string server, std::string name):
    m_status(FLAG_NO_CLIENT | FLAG_NO_PORTS)
{

    jack_options_t options = JackNullOption;
    jack_status_t status;

    m_client = jack_client_open( name.c_str(), options, &status, server );
    if ( m_client == NULL ){

        throw std::string("Unable to connect JACK server");
    }
    if ( status & JackNameNotUnique ){

        std::cout << "Unique name " << jack_get_client_name( m_client );
        std::cout << " assigned" << std::endl;
    }
    m_status &= !FLAG_NO_CLIENT;
}

JACKUnit::~JACKUnit(){

    delete m_ports;
    jack_client_close( m_client );
}

/* ****************************************************************** */
/* *************************** Registration ************************* */
/* ****************************************************************** */
void JACKUnit::registerPorts(const std::string *names, uint8_t ai, uint8_t ao, uint8_t mi, uint8_t mo){

    uint8_t portCount = ai + ao + mi + mo;
    m_ports = new jack_port_t*[portCount];
    
    for ( uint8_t i = 0; i < portCount; i++ ){

        if ( i < ai ){

            m_ports[i] = jack_port_register( m_client, names[i].c_str(),
                        JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0 );
            if ( m_ports[i] == NULL ){
                throw std::string("Failed Register Audio Input");
            }
        }
        else if ( i < ai + ao ){

            m_ports[i] = jack_port_register( m_client, names[i].c_str(),
                        JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0 );
            if ( m_ports[i] == NULL ){
                throw std::string("Failed Register Audio Output");
            }
        }
        else if ( i < ai + ao + mi ){

            m_ports[i] = jack_port_register( m_client, names[i].c_str(),
                        JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0 );
            if ( m_ports[i] == NULL ){
                throw std::string("Failed Register Midi Input");
            }
        }
        else{

            m_ports[i] = jack_port_register( m_client, names[i].c_str(),
                        JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0 );
            if ( m_ports[i] == NULL ){
                throw std::string("Failed Register Midi Output");
            }
        }
    }
    m_status &= !FLAG_NO_PORTS;
}

void JACKUnit::registerCallback(int (*callback)(jack_nframes_t, void*), void *arg){

    if ( !(m_status & FLAG_NO_CLIENT) ){
        
        jack_set_process_callback( m_client, callback, (void*)arg );
    }
}

void JACKUnit::activateClient(){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw std::string("Failed Activate Client, Unit Not Complete");
    }
    if ( jack_activate( m_client ) ){

        throw std::string("Failed Activate Client");
    }
}

void JACKUnit::deactivateClient(){

    jack_deactivate( m_client );
}

void JACKUnit::connect(std::string source, std::string target){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw std::string("NonCompleteClient");
    }
    if ( jack_connect( m_client, source.c_str(), target.c_str() ) ){

        throw std::string("Cannot Connect Ports");
    }
}

void JACKUnit::disconnect(std::string source, std::string target){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw std::string("NonCompleteClient");
    }
    if ( jack_disconnect( m_client, source.c_str(), target.c_str() ) ){

        throw std::string("Cannot Disconnect Ports");
    }
}

/* ****************************************************************** */
/* ************************* Getters Setters ************************ */
/* ****************************************************************** */

jack_port_t** JACKUnit::getPorts(){

    return m_ports;
}

std::string JACKUnit::getName() const{

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        return std::string("NonCompleteClient");
    }
    return std::string( jack_get_client_name( m_client ) );
}

float JACKUnit::getSamplerate() const{

    return jack_get_sample_rate( m_client);
}
