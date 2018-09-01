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
 * File:   Distortion.cc
 * Author: Space-Computer
 * 
 * Created on 1 août 2018, 15:22
 */

#include "Distortion.h"

#define NAME "Distortion"

ModuleDistortion::ModuleDistortion():
client(nullptr),

audio_in(nullptr),
audio_out(nullptr),
        
shape(0.25), softness(5), gain(1000), volume(0.1f),
        
tone_in(nullptr), tone_out(nullptr)
{
    
    // Setup Equalizer, Param used here are not important
    float f[] = {100, 1000};
    tone_in  = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, 48000));
    tone_out = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, 48000));
    
    client = sfx::openJackClient(NAME);

    jack_set_process_callback(client, jackCallback, this);

    audio_in = sfx::registerAudioInput(client, "Input");
    audio_out= sfx::registerAudioOutput(client, "Output");
    
    midi_in  = sfx::registerMidiInput(client, "MidiInput");
    midi_out = sfx::registerMidiOutput(client, "MidiOutput");

    sfx::activateClient(client);
    
    this->setBank1();
}
ModuleDistortion::~ModuleDistortion()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModuleDistortion::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModuleDistortion* disto = (ModuleDistortion*)arg;
    
    sfx::sample_t *in, *out;
    in  = (sfx::sample_t*)jack_port_get_buffer( disto->audio_in , nframes );
    out = (sfx::sample_t*)jack_port_get_buffer( disto->audio_out, nframes );

    void* midi_port = jack_port_get_buffer(disto->midi_in, nframes);
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
    
    void* midi_out = jack_port_get_buffer(disto->midi_out, nframes);
    jack_midi_clear_buffer(midi_out);
    
    jack_midi_event_get(&in_event, midi_port, 0);
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        if (in_event.time == i && event_index < event_count)
        {
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                /*
                sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
                    , i, in_event.time, *(in_event.buffer));
                //*/
                
                if (sfx::Midi_verify_CCSource(in_event.buffer, 64))
                {
                    //sfx::debug(NAME, "Changement vers banque 1\n");
                    disto->setBank1();
                    
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 64, true);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 65, false);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 66, false);
                }
                else if (sfx::Midi_verify_CCSource(in_event.buffer, 65))
                {
                    //sfx::debug(NAME, "Changement vers banque 2\n");
                    disto->setBank2();
                    
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 64, false);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 65, true);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 66, false);
                }
                else if (sfx::Midi_verify_CCSource(in_event.buffer, 66))
                {
                    //sfx::debug(NAME, "Changement vers banque 3\n");
                    disto->setBank3();
                    
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 64, false);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 65, false);
                    sfx::Midi_reserve_UpdateOutput(midi_out, i, 66, true);
                }
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }

        out[i] = disto->tone_in->compute(in[i], 3, disto->tone_in_params.gains);
        
        out[i] *= disto->gain;
        out[i] = (1 - disto->shape)*tanh( out[i] ) + disto->shape*tanh( out[i] / disto->softness );
        
        out[i] = disto->volume * disto->tone_out->compute(out[i], 3, disto->tone_out_params.gains);
    }
    
    return 0;
}

void ModuleDistortion::setBank1(void)
{
    shape = 0.5;
    softness = 10;
    gain = 30;
    volume = 0.25f;
    
    tone_in_params.low = 350;
    tone_in_params.high = 800;
    
    tone_in_params.gains[0] = 1.0f;
    tone_in_params.gains[1] = 0.8f;
    tone_in_params.gains[2] = 1.3f;
    
    tone_in->setFrequency(0, tone_in_params.low, jack_get_sample_rate(client));
    tone_in->setFrequency(1, tone_in_params.high, jack_get_sample_rate(client));
    
    tone_out_params.low = 350;
    tone_out_params.high = 800;
    
    tone_out_params.gains[0] = 1.8f;
    tone_out_params.gains[1] = 0.7f;
    tone_out_params.gains[2] = 2.3f;
    
    tone_out->setFrequency(0, tone_out_params.low, jack_get_sample_rate(client));
    tone_out->setFrequency(1, tone_out_params.high, jack_get_sample_rate(client));
}

void ModuleDistortion::setBank2(void)
{
    shape = 0.25;
    softness = 3;
    gain = 500;
    volume = 0.1f;
    
    tone_in_params.low = 200;
    tone_in_params.high = 800;
    
    tone_in_params.gains[0] = 1.0f;
    tone_in_params.gains[1] = 0.3f;
    tone_in_params.gains[2] = 1.1f;
    
    tone_in->setFrequency(0, tone_in_params.low, jack_get_sample_rate(client));
    tone_in->setFrequency(1, tone_in_params.high, jack_get_sample_rate(client));
    
    tone_out_params.low = 350;
    tone_out_params.high = 800;
    
    tone_out_params.gains[0] = 1.0f;
    tone_out_params.gains[1] = 1.5f;
    tone_out_params.gains[2] = 0.7f;
    
    tone_out->setFrequency(0, tone_out_params.low, jack_get_sample_rate(client));
    tone_out->setFrequency(1, tone_out_params.high, jack_get_sample_rate(client));
}

void ModuleDistortion::setBank3(void)
{
    shape = 0.25;
    softness = 5;
    gain = 250;
    volume = 0.1f;
    
    tone_in_params.low = 350;
    tone_in_params.high = 600;
    
    tone_in_params.gains[0] = 2.0f;
    tone_in_params.gains[1] = 2.7f;
    tone_in_params.gains[2] = 0.5f;
    
    tone_in->setFrequency(0, tone_in_params.low, jack_get_sample_rate(client));
    tone_in->setFrequency(1, tone_in_params.high, jack_get_sample_rate(client));
    
    tone_out_params.low = 350;
    tone_out_params.high = 600;
    
    tone_out_params.gains[0] = 1.8f;
    tone_out_params.gains[1] = 0.7f;
    tone_out_params.gains[2] = 2.3f;
    
    tone_out->setFrequency(0, tone_out_params.low, jack_get_sample_rate(client));
    tone_out->setFrequency(1, tone_out_params.high, jack_get_sample_rate(client));
}