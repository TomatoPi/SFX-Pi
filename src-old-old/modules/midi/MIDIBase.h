/**********************************************************************
 * @file MIDIBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing MIDI related general stuff
 **********************************************************************/
#ifndef DEF_MIDI_BASE_H
#define DEF_MIDI_BASE_H

#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "../../Const.h"

class MIDIBase{

    public :

        /**
         * Setup MIDI Base
         * Calculate MIDI Table from current samplerate, avoid recalculating
         *  frequency from midi note at each frame
         * Register a callback to update frequency table if samplerate
         *  change
         **/
        static int initMIDI();

        /**
         * Kill MIDI Base
         **/
        static int killMIDI();

        /**
         * Convert a midi note to a frequency in Hz
         * @warning : Called during jack process graph, must be realtime
         *  friendly
         **/
        static sample_t getNoteFrequency(uint8_t note);

    public :

        static const hex_t MaskMessageType = 0xf0;
        static const hex_t MaskMessageChan = 0x0f;

        static const hex_t EventNoteOn  = 0x90;
        static const hex_t EventNoteOff = 0x80;

        static const hex_t EventPolyAftertouch = 0xA0;
        static const hex_t EventChanAftertouch = 0xD0;
        
        static const hex_t EventControlChange  = 0xB0;
        static const hex_t EventProgramChange  = 0xC0;

        static const hex_t EventPitchWheel = 0xE0;
        
        static const hex_t EventSystem = 0xF0;

    private :

        MIDIBase();
        ~MIDIBase();

        /**
         * Recalculate MIDI Table from current samplerate
         **/
        static int calcMIDITable(unsigned int srate, void*);

        static MIDIBase* m_instance; /**< Unique instance of MIDIBase */

        jack_client_t *m_client; /**< Client to monitor jack Server */
        sample_t m_table[128];   /**< Midi frequencies table */
};

#endif
