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
 * File:   Delay.cc
 * Author: Space-Computer
 * 
 * Created on 11 août 2018, 01:56
 */

#include "Delay.h"

#define NAME "Delay"

ModuleDelay::ModuleDelay():
client(nullptr),

audio_in(nullptr),
audio_out(nullptr),
        
length(100), feedback(0.1f), dry_wet(0.3f), volume(1),
        
buffer(nullptr)
{
    client = sfx::openJackClient(NAME);

    jack_set_process_callback(client, jackCallback, this);

    audio_in = sfx::registerAudioInput (client, "Input");
    audio_out= sfx::registerAudioOutput(client, "Output");
    
    midi_in = sfx::registerMidiInput (client, "MidiInput");
    midi_out= sfx::registerMidiOutput(client, "MidiOutput");

    buffer = std::unique_ptr<Buffer_S>(new Buffer_S(length, jack_get_sample_rate(client)));

    sfx::activateClient(client);
    
    this->setBank1();
}
ModuleDelay::~ModuleDelay()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModuleDelay::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModuleDelay* delay = (ModuleDelay*)arg;
    
    sfx::sample_t *in, *out;
    in  = (sfx::sample_t*)jack_port_get_buffer( delay->audio_in , nframes );
    out = (sfx::sample_t*)jack_port_get_buffer( delay->audio_out, nframes );

    void* midi_port = jack_port_get_buffer(delay->midi_in, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_nframes_t event_count = jack_midi_get_event_count(midi_port);
    
    /*
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, midi_port, i);
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
            , i, in_event.time, *(in_event.buffer));
    }
    //*/
    
    void* midi_out = jack_port_get_buffer(delay->midi_out, nframes);
    jack_midi_clear_buffer(midi_out);
    
    jack_midi_event_get(&in_event, midi_port, 0);
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        if (in_event.time == i && event_index < event_count)
        {
            /*
            sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
                , i, in_event.time, *(in_event.buffer));
            //*/
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                if (sfx::Midi_verify_CCSource(in_event.buffer, 67))
                {
                    delay->dry_wet = delay->bk_dry_wet;
                    //sfx::debug(NAME, "Delay ON\n");
                    
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 67, true);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 68, false);
                }
                else if (sfx::Midi_verify_CCSource(in_event.buffer, 68))
                {
                    //sfx::debug(NAME, "Delay OFF\n");
                    delay->dry_wet = 0;
                    
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 67, false);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 68, true);
                }
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }
        
        // Get Delayed signal back
        sfx::sample_t del = delay->buffer->read();

        // Output delayed signal with clear input
        out[i] = delay->volume * sfx::drywet( in[i], del, delay->dry_wet );

        // Write input + feedback inside buffer
        delay->buffer->write(in[i] + delay->feedback * del);
    }
    
    return 0;
}

void ModuleDelay::setBank1(void)
{
    length = 150;
    feedback = 0.45f;
    dry_wet = 0.2f;
    volume = 1;
    
    bk_dry_wet = dry_wet;
    
    buffer->set_length(length, jack_get_sample_rate(client));
}

void ModuleDelay::setBank2(void)
{
    length = 300;
    feedback = 0.3f;
    dry_wet = 0.15f;
    volume = 1;
    
    bk_dry_wet = dry_wet;
    
    buffer->set_length(length, jack_get_sample_rate(client));
}

void ModuleDelay::setBank3(void)
{
    length = 225;
    feedback = 0.25f;
    dry_wet = 0.15f;
    volume = 1;
    
    bk_dry_wet = dry_wet;
    
    buffer->set_length(length, jack_get_sample_rate(client));
}

void ModuleDelay::setBank4(void)
{
    length = 200;
    feedback = 0.4f;
    dry_wet = 0.1f;
    volume = 1;
    
    bk_dry_wet = dry_wet;
    
    buffer->set_length(length, jack_get_sample_rate(client));
}