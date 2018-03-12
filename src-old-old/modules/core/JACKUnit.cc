/**********************************************************************
 * @file JACKUnit.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing JACK Side of Effect Units
 **********************************************************************/
#include "JACKUnit.h"
namespace{

    const hex_t FLAG_NULL         = 0x00;
    const hex_t FLAG_NO_CLIENT    = 0x01;
    const hex_t FLAG_NO_PORTS     = 0x02;
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
void JACKUnit::registerPorts(const std::string *names, usize_t ai, usize_t ao, usize_t mi, usize_t mo){

    usize_t portCount = ai + ao + mi + mo;
    m_ports = new jack_port_t*[portCount];
    
    for ( usize_t i = 0; i < portCount; i++ ){

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

    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Registered ( %lu ) ports\n",
            jack_get_client_name(m_client), portCount );
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

        throw std::string("Failed Activate Client, Unit Not Complete");
    }
    if ( jack_activate( m_client ) ){

        throw std::string("Failed Activate Client");
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Activated\n", jack_get_client_name(m_client) );
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

        throw std::string("Cannot Connect Ports : " + source + " to : " + target);
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Added Connection From ( %s ) To ( %s )\n",
            jack_get_client_name(m_client), source.c_str(), target.c_str() );
    }
}

void JACKUnit::disconnect(std::string source, std::string target){

    if ( m_status & FLAG_NO_PORTS || m_status & FLAG_NO_CLIENT ){

        throw std::string("NonCompleteClient");
    }
    if ( jack_disconnect( m_client, source.c_str(), target.c_str() ) ){

        throw std::string("Cannot Disconnect Ports");
    }
    if ( SFXP::GlobalIsDebugEnabled ){

        printf( "JACKClient ( %s ) : Removed Connection From ( %s ) To ( %s )\n",
            jack_get_client_name(m_client), source.c_str(), target.c_str() );
    }
}

/* ****************************************************************** */
/* ************************* Getters Setters ************************ */
/* ****************************************************************** */

jack_port_t** JACKUnit::getPorts(){

    return m_ports;
}

jack_client_t* JACKUnit::getClient(){

    return m_client;
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
