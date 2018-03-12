/**********************************************************************
 * @file MIDIBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing MIDI related general stuff
 **********************************************************************/
#include "MIDIBase.h"

MIDIBase* MIDIBase::m_instance = NULL;

/**
 * Setup MIDI Base
 * Calculate MIDI Table from current samplerate, avoid recalculating
 *  frequency from midi note at each frame
 * Register a callback to update frequency table if samplerate
 *  change
 **/
int MIDIBase::initMIDI(){

    if ( m_instance == NULL ){

        try{
            
            m_instance = new MIDIBase();
            
            calcMIDITable(jack_get_sample_rate(m_instance->m_client), 0);
        }
        catch ( std::string & s ){

            printf("%s", s.c_str());
            m_instance = NULL;
            return 1;
        }
    }
    else{

        printf("MIDI Base : Warning : MIDI is already initialized\n");
        return 1;
    }
    
    return 0;
}

/**
 * Kill MIDI Base
 **/
int MIDIBase::killMIDI(){

    if ( m_instance ){

        delete m_instance;
        m_instance = NULL;

        return 0;
    }
    else{

        printf("MIDI Base : Warning : MIDI is not initialized\n");

        return 1;
    }
}

/**
 * Convert a midi note to a frequency in Hz
 * @warning : Called during jack process graph, must be realtime
 *  friendly
 **/
sample_t MIDIBase::getNoteFrequency(uint8_t note){

    if ( m_instance ){

        return m_instance->m_table[note];
    }
}

/**
 * Constructor and Destructor
 **/
MIDIBase::MIDIBase(){

    printf("MIDIBase : Setup\n");

    jack_options_t options = JackNullOption;
    jack_status_t status;

    m_client = jack_client_open( "MIDIBase", options, &status, SFXP::JACK_SERVER );
    if ( m_client == NULL ){

        throw std::string("MIDIBase : Error : Failed Create JACK Client\n");
    }

    jack_set_sample_rate_callback(m_client, calcMIDITable, 0);

    printf("MIDIBase : Setup : Done\n");
}
MIDIBase::~MIDIBase(){

    printf("MIDIBase : Kill\n");
    jack_client_close(m_client);
}

/**
 * Recalculate MIDI Table from current samplerate
 **/
int MIDIBase::calcMIDITable(unsigned int srate, void* arg){

    if ( m_instance ){

        printf("MIDIBase : Update : Update MIDI table\n");
        
        for ( uint8_t i = 0; i < 128; i++ ){

            m_instance->m_table[i] = (440.0 / 32.0) * pow(2, ((sample_t)i-9.0)/12.0 );
            //printf("Note %i : F=%4.10f : Rbs=%4.10f\n", i, m_instance->m_table[i], m_instance->m_table[i]/srate);
            m_instance->m_table[i] /= srate;
        }
    }
    return 0;
}
