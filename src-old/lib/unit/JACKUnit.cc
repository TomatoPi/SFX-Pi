/**********************************************************************
 * @file JACKUnit.cpp
 * @author TomatoPi
 * @version 1.1
 *
 * File Containing JACK Side of Effect Units
 **********************************************************************/
#include "JACKUnit.h"

using namespace SFXP;

namespace{

    const hex_t FLAG_NULL         = 0x00;
    const hex_t FLAG_NO_CLIENT    = 0x01;
    const hex_t FLAG_NO_PORTS     = 0x02;
}

/**
 * Function to get Jack's string equivalent of port type
 **/
const char* JACKUnit::portTypeString(PortType t){

    if (t == PortType::AudioIn || t == PortType::AudioOut) return JACK_DEFAULT_AUDIO_TYPE;
    return JACK_DEFAULT_MIDI_TYPE;
}

/* ****************************************************************** */
/* **************************** Constructor ************************* */
/* ****************************************************************** */
JACKUnit::JACKUnit(string server, string name):
    m_status(FLAG_NO_CLIENT | FLAG_NO_PORTS)
{

    jack_status_t status;

    m_client = jack_client_open( name.c_str(), JackNullOption, &status, server );
    if ( m_client == NULL ){

        throw string("Unable to connect JACK server");
    }
    if ( status & JackNameNotUnique && SFXP::GlobalIsDebugEnabled ){

        printf( "Unique Name ( %s ) Assigned\n", jack_get_client_name(m_client) );
    }
    m_status &= !FLAG_NO_CLIENT;

    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Created\n", jack_get_client_name(m_client) );
    }
}

JACKUnit::~JACKUnit(){
    
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "Kill JACKClient ( %s )", jack_get_client_name(m_client) );
    }

    delete m_ports;
    jack_client_close( m_client );

    if ( SFXP::GlobalIsDebugEnabled ){

        printf( " -- Done\n");
    }
}

/* ****************************************************************** */
/* *************************** Registration ************************* */
/* ****************************************************************** */
void JACKUnit::registerPorts(const std::string** names,const SFXP::usize_t* count){

    m_portCount = new usize_t[4];
    m_ports = new jack_port_t**[4];

    for ( usize_t k = 0; k < 4; k++ ){

        m_portCount[k] = count[k];
        m_ports[k] = new jack_port_t*[m_portCount[k]];
    
        for ( usize_t i = 0; i < m_portCount[k]; i++ ){

            if ( k == 0 ){

                m_ports[k][i] = jack_port_register( m_client, names[k][i].c_str(),
                            JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0 );
                if ( m_ports[k][i] == NULL ){
                    throw string("Failed Register Audio Input");
                }
            }
            else if ( k == 1 ){

                m_ports[k][i] = jack_port_register( m_client, names[k][i].c_str(),
                            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0 );
                if ( m_ports[k][i] == NULL ){
                    throw string("Failed Register Audio Output");
                }
            }
            else if ( k == 2 ){

                m_ports[k][i] = jack_port_register( m_client, names[k][i].c_str(),
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0 );
                if ( m_ports[k][i] == NULL ){
                    throw string("Failed Register Midi Input");
                }
            }
            else{

                m_ports[k][i] = jack_port_register( m_client, names[k][i].c_str(),
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0 );
                if ( m_ports[k][i] == NULL ){
                    throw string("Failed Register Midi Output");
                }
            }
        }
    }
    m_status &= !FLAG_NO_PORTS;

    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Registered ( %u ) ports\n",
            jack_get_client_name(m_client), count[0]+count[1]+count[2]+count[3] );
    }
}

void JACKUnit::registerCallback(int (*callback)(jack_nframes_t, void*), void *arg){

    if ( !(m_status & FLAG_NO_CLIENT) ){
        
        jack_set_process_callback( m_client, callback, (void*)arg );

        if ( SFXP::GlobalIsDebugEnabled ){

            printf( "JACKClient ( %s ) : Registered Process Callback\n",
                jack_get_client_name(m_client) );
        }
    }
}

void JACKUnit::activateClient(){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw string("Failed Activate Client, Unit Not Complete");
    }
    if ( jack_activate( m_client ) ){

        throw string("Failed Activate Client");
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Activated\n", jack_get_client_name(m_client) );
    }
}

void JACKUnit::deactivateClient(){

    jack_deactivate( m_client );
}

void JACKUnit::connect(string source, string target){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw string("NonCompleteClient");
    }
    if ( jack_connect( m_client, source.c_str(), target.c_str() ) ){

        throw string("Cannot Connect Ports : " + source + " to : " + target);
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Added Connection From ( %s ) To ( %s )\n",
            jack_get_client_name(m_client), source.c_str(), target.c_str() );
    }
}

void JACKUnit::disconnect(string source, string target){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw string("NonCompleteClient");
    }
    if ( jack_disconnect( m_client, source.c_str(), target.c_str() ) ){

        throw string("Cannot Disconnect Ports");
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Removed Connection From ( %s ) To ( %s )\n",
            jack_get_client_name(m_client), source.c_str(), target.c_str() );
    }
}

/* ****************************************************************** */
/* ************************* Getters Setters ************************ */
/* ****************************************************************** */

jack_port_t*** JACKUnit::getPortList(){

    return m_ports;
}

jack_port_t*  JACKUnit::getPort(PortType type, usize_t idx){

    return m_ports[static_cast<usize_t>(type)][idx];
}

usize_t JACKUnit::getPortCount(PortType type){

    return m_portCount[static_cast<usize_t>(type)];
}

jack_client_t* JACKUnit::getClient(){

    return m_client;
}

string JACKUnit::getName() const{

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        return string("NonCompleteClient");
    }
    return string( jack_get_client_name( m_client ) );
}

float JACKUnit::getSamplerate() const{

    return jack_get_sample_rate(m_client);
}
