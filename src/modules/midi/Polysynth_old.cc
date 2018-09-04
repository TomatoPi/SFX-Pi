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
 * File:   Polysynth.cc
 * Author: Space-Computer
 * 
 * Created on 6 août 2018, 21:00
 */
#include "Polysynth.h"

#define NAME "PolySynth"

ModulePolysynth::ModulePolysynth() :
client(nullptr),
midi_in(nullptr), audio_out(nullptr),

waveform(WHAR),
phase(0), sign(1), volume(0.1f),
param1(4), param2(2), phase_distortion(0.5), phase_fix(0),

notes_map(),

tfunc(castWaveform(WHAR))
{
    client = sfx::openJackClient(NAME);

    jack_set_process_callback(client, jackCallback, this);

    midi_in = sfx::registerMidiInput(client, "Input");
    audio_out = sfx::registerAudioOutput(client, "Output");

    sfx::activateClient(client);
    
    sfx::Midi_calcMidiTable(jack_get_sample_rate(client));
}

ModulePolysynth::~ModulePolysynth()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModulePolysynth::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModulePolysynth* synth = (ModulePolysynth*) arg;
    sfx::sample_t *out = (sfx::sample_t*)jack_port_get_buffer(synth->audio_out, nframes);

    synth->sign = (synth->sign < 0) ? -1 : 1;

    void* midi_port = jack_port_get_buffer(synth->midi_in, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_nframes_t event_count = jack_midi_get_event_count(midi_port);
    
    float o = synth->phase_fix, d = synth->phase_distortion;

    jack_midi_event_get(&in_event, midi_port, 0);
    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        if (in_event.time == i && event_index < event_count)
        {
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_NoteOn))
            {
                sfx::hex_t note = sfx::Midi_read_NoteValue(in_event.buffer);
                
                Note n = Note{
                    .ramp = synth->phase,
                    .rbs  = sfx::Midi_MidiTable[note],
                    .vol  = sfx::map(sfx::Midi_read_NoteVelocity(in_event.buffer), 0, 128, 0, 1)
                };
                
                n.envelope.setAttackRate(sfx::mstos(20, jack_get_sample_rate(synth->client)));
                n.envelope.setDecayRate(sfx::mstos(100, jack_get_sample_rate(synth->client)));
                n.envelope.setReleaseRate(sfx::mstos(200, jack_get_sample_rate(synth->client)));
                n.envelope.setSustainLevel(sfx::dbtorms(-12));
                n.envelope.gate(1);

                synth->notes_map[note] = n;
                
                /*
                sfx::debug(NAME, "Midi note on : %x : Note : %i : Velocity : %i : Rbs : %f\n",
                        in_event.buffer[0], in_event.buffer[1], in_event.buffer[2], n.ramp);
                //*/
            }
            else if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_NoteOff))
            {
                sfx::hex_t note = sfx::Midi_read_NoteValue(in_event.buffer);
                
                synth->notes_map[note].envelope.gate(0);
                
                /*
                sfx::debug(NAME, "Midi note off : %x : Note : %i : Velocity : %i\n",
                        in_event.buffer[0], in_event.buffer[1], in_event.buffer[2]);
                //*/
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }
        
        out[i] = 0;
        
        for (auto& note : synth->notes_map)
        {
            if (note.second.envelope.getState())
            {
                note.second.ramp += note.second.rbs;
                note.second.ramp = fmod(note.second.ramp - o, 1.0f);

                float pp = note.second.ramp;
                if ( d != 0.5f )
                {
                    pp = (pp>d)? (pp+1.0f - 2.0f*d)/(2.0f*(1.0f-d)) : pp/(2.0f*d);
                    pp += o;
                    pp = fmod(pp, 1.0f);
                }

                out[i] += synth->volume * note.second.envelope.process() * (*synth->tfunc)(pp, synth->sign, synth->param1, synth->param2);
            }
            else
            {
                synth->notes_map.erase(note.first);
            }
        }
        
        if (out[i]>1.0f) out[i] = 1.0f;
        else if (out[i]<-1.0f) out[i] = -1.0f;
    }
    return 0;
}