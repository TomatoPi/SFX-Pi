/*
 * Copyright (C) 2018 Space-Computer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   midi.h
 * Author: Space-Computer
 *
 * Created on 31 juillet 2018, 16:57
 */

#ifndef MIDI_H
#define MIDI_H

#include <math.h>
#include <assert.h>

#include "types.h"
#include <jack/midiport.h>

namespace sfx
{
////////////////////////////////////////////////////////////////////
// Midi Notes Tables
////////////////////////////////////////////////////////////////////

    static sample_t Midi_MidiTable[128];
    
    static void Midi_calcMidiTable(unsigned int srate)
    {
        for ( uint8_t i = 0; i < 128; i++ ){

            Midi_MidiTable[i] = (440.0 / 32.0) * pow(2, ((sample_t)i-9.0)/12.0 );
            //printf("Note %i : F=%4.10f : Rbs=%4.10f\n", i, m_instance->m_table[i], m_instance->m_table[i]/srate);
            Midi_MidiTable[i] /= srate;
        }
    }
    
////////////////////////////////////////////////////////////////////
// Status Bits
////////////////////////////////////////////////////////////////////
    
    const hex_t Midi_ControlChange      = 0xB0;
    
    const hex_t Midi_SystemExclusive    = 0xF0;
    const hex_t Midi_SystemExclusiveEnd = 0xF7;

    const hex_t Midi_NoteOn  = 0x90;
    const hex_t Midi_NoteOff = 0x80;
    
////////////////////////////////////////////////////////////////////
// Data Bytes
////////////////////////////////////////////////////////////////////
    
    const hex_t Midi_SFXIDCode          = 0x77;
    
    const hex_t Midi_UpdateOutput       = 0x02;
    
    const hex_t Midi_UpdateOutputOn     = 0x03;
    const hex_t Midi_UpdateOutputOff    = 0x02;
    
    const hex_t Midi_AnalogChange       = 11;
    
////////////////////////////////////////////////////////////////////
// Specific Midi Message Parsing Functions
////////////////////////////////////////////////////////////////////
    
    inline bool Midi_verify_ChanneledMessage(unsigned char* event, hex_t statusb)
    {
        return (event[0] & 0xF0) == statusb;
    }
    
    inline bool Midi_verify_ChanneledMessage(unsigned char* event, hex_t statusb, hex_t channel)
    {
        return (event[0] & 0xF0) == statusb && (event[0] & 0x0F) == channel;
    }
    
    inline bool Midi_verify_CCSource(unsigned char* event, hex_t source)
    {
        return event[1] == source;
    }
    
    inline hex_t Midi_read_CCChannel(unsigned char* event)
    {
        return event[0] & 0x0f;
    }
    inline hex_t Midi_read_CCSource(unsigned char* event)
    {
        return event[1];
    }
    inline hex_t Midi_read_CCValue(unsigned char* event)
    {
        return event[2] & 0x7f;
    }
    
    ////////////////////////////////////////////////////////////////
    
    inline void Midi_reserve_ControlChange(void* port_buff, jack_nframes_t i
        , const hex_t& chanel, const hex_t& ctrl, const hex_t& val)
    {
        unsigned char* buff = jack_midi_event_reserve(port_buff, i, 3);
        
        buff[0] = (Midi_ControlChange & 0xF0) | (chanel & 0x0F);
        
        buff[1] = ctrl;
        buff[2] = val;
    }
    
    inline void Midi_reserve_ControlChange_Throught(void* port_buff, jack_nframes_t i
        , const hex_t* source)
    {
        unsigned char* buff = jack_midi_event_reserve(port_buff, i, 3);
        
        buff[0] = source[0];
        
        buff[1] = source[1];
        buff[2] = source[2];
    }
    
    inline void Midi_reserve_14bitControlChange(void* port_buff, jack_nframes_t i
        , const hex_t& chanel, const hex_t& ctrl, const int& val)
    {
        unsigned char* buff = jack_midi_event_reserve(port_buff, i, 3);
        
        buff[0] = (Midi_ControlChange & 0xF0) | (chanel & 0x0F);
        
        buff[1] = ctrl;
        buff[2] = 0x7F & (val >> 8);
        
        buff = jack_midi_event_reserve(port_buff, i, 3);
        
        buff[0] = (Midi_ControlChange & 0xF0) | (chanel & 0x0F);
        
        buff[1] = ctrl + 32;
        buff[2] = 0x7F & val;
    }
    
    inline int Midi_read_14bitControlChange(unsigned char* event1, unsigned char* event2)
    {
        assert(event1[1] == event2[1]-32);
        return event1[2] * 128 + event2[1];
    }
    
    ////////////////////////////////////////////////////////////////
    
    inline sfx::hex_t Midi_read_NoteValue(unsigned char* event)
    {
        return event[1];
    }
    
    inline sfx::hex_t Midi_read_NoteVelocity(unsigned char* event)
    {
        return event[2];
    }
    
    ////////////////////////////////////////////////////////////////
    
    inline void Midi_reserve_UpdateOutput(void* port_buff, jack_nframes_t i
        , const hex_t& id, const bool& status)
    {
        unsigned char* buff = jack_midi_event_reserve(port_buff, i, 5);
        
        buff[0] = Midi_SystemExclusive;
        buff[1] = Midi_SFXIDCode;
        
        buff[2] = status?Midi_UpdateOutputOn:Midi_UpdateOutputOff;
        buff[3] = id;
        
        buff[4] = Midi_SystemExclusiveEnd;
    }
    inline bool Midi_is_UpdateOutput(unsigned char* event)
    {
        return event[2] == Midi_UpdateOutputOn || event[2] == Midi_UpdateOutputOff;
    }
    inline bool Midi_UpdateOutput_Status(unsigned char* event)
    {
        return event[2] == Midi_UpdateOutputOn;
    }
    inline hex_t Midi_UpdateOutput_ID(unsigned char* event)
    {
        return event[3];
    }
}

#endif /* MIDI_H */

