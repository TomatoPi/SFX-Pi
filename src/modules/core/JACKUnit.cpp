/**********************************************************************
 * 
 * 
 **********************************************************************/
#include "JACKUnit.h"

using namespace std;
using namespace PCST;

JACKUnit::~JACKUnit(){
    
}

bool JACKUnit::registerClient(std::string p_name){
    
    if ( status_ & JACKUnit::STATUS_NO_CLIENT ){
        
        jack_options_t options = JackNullOption;
        jack_status_t status;

        jack_client_t *client;

        char* name = (char*)p_name.c_str();

        // Creating jack client
        client = jack_client_open ( name, options, &status, JACK_SERVER_NAME.c_str() );
        if ( client == NULL ) {

            return true;
        }

        // Upgrade name if given name is not unique
        if ( status & JackNameNotUnique ) {

            name = jack_get_client_name(client);
            if ( FULL_LOG ) cout << "Unique name " << name << " assigned" << endl;
        }

        if ( FULL_LOG ) cout << "Client Registration is OK" << endl;
        client_ = client;
        name_ = string(jack_get_client_name(client));
        
        status &= !STATUS_NO_CLIENT;
        
        return false;
    }
    return true;
}

bool JACKUnit::activateClient(){
    
    if ( status_ & JACKUnit::STATUS_NO_CLIENT 
        || JACKUnit::status_ & STATUS_NO_PORT ) return true;
    
    
}

bool JACKUnit::killClient(){
    
    if ( status_ & JACKUnit::STATUS_NO_CLIENT ) return true;
}

bool registerPort(std::string* names, size_t ai, size_t ao, size_t mi, size_t mo){
    
    if ( status_ & JACKUnit::STATUS_NO_CLIENT ) return true;
    
    portC_ = ai + ao + mi + mo;
        
        port_ = new jack_port_t*[portC_];
        
        for ( size_t i = 0; i < portC_ ; i++ ) {

            int t = 3;
            if (i < ai + ao + mi) {

                t = 2;
            }
            if (i < ai + ao) {

                t = 1;
            }
            if (i < ai) {

                t = 0;
            }

            switch (t) {

                case 0 :
                    //cout << "register audio in : " << n << " idx " << i << endl;
                    port_[i]        = jack_port_register( client_, names[i], JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
                    if ( audio_in_[i] == NULL ) throw string("Failed register audio input port");
                    break;
                case 1 :
                    //cout << "register audio out : " << n << " idx " << i-ai << endl;
                    port_[i]    = jack_port_register( client_, names[i], JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
                    if ( audio_out_[i] == NULL ) throw string("Failed register audio output port");
                    break;
                case 2 :
                    //cout << "register midi in : " << n << " idx " << i-ai-ao << endl;
                    port_[i]   = jack_port_register( client_, names[i], JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
                    if ( midi_in_[i] == NULL ) throw string("Failed register midi input port");
                    break;
                case 3 :
                    //cout << "register midi out : " << n << " idx " << i-ai-ao-mi << endl;
                    port_[i] = jack_port_register( client_, names[i], JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
                    if ( midi_out_[i] == NULL ) throw string("Failed register midi output port");
                    break;
            }
        }
        return false;
}

