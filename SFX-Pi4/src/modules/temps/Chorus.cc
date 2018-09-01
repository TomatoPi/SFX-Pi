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
 * File:   Chorus.cc
 * Author: Space-Computer
 * 
 * Created on 11 août 2018, 02:20
 */

#include <fstream>

#include "Chorus.h"

#define NAME "Chorus"

ModuleChorus::ModuleChorus():
client(nullptr),

audio_in(nullptr),
mod_in(nullptr),
audio_out(nullptr),
        
feedback(0.1f), dry_wet(0.7f), size(2), volume(1),
        
buffer(nullptr)
{
    client = sfx::openJackClient(NAME);
    
    jack_set_process_callback(client, jackCallback, this);

    audio_in  = sfx::registerAudioInput(client, "Input");
    mod_in    = sfx::registerAudioInput(client, "Modulation");
    audio_out = sfx::registerAudioOutput(client, "Output");
    
    int del[5] = {10};
    buffer = std::unique_ptr<Buffer_M>(new Buffer_M(100, jack_get_sample_rate(client), 5, del));

    sfx::activateClient(client);
    
    this->setBank1();
}
ModuleChorus::~ModuleChorus()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModuleChorus::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModuleChorus* chorus = (ModuleChorus*)arg;
    
    sfx::sample_t *in, *mod, *out;
    in  = (sfx::sample_t*)jack_port_get_buffer( chorus->audio_in , nframes );
    mod = (sfx::sample_t*)jack_port_get_buffer( chorus->mod_in   , nframes );
    out = (sfx::sample_t*)jack_port_get_buffer( chorus->audio_out, nframes );
    
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        // Get Delayed signal back
        sfx::sample_t del = 0;

        // Read Each bands
        for (size_t k = 0; k < chorus->size; k++) {

            del += chorus->voices_params[k].weight * chorus->buffer->read(
                    k,
                    1 + mod[i] * chorus->voices_params[k].depth);
        }

        // Output delayed signal with clear input
        out[i] = chorus->volume * sfx::drywet( in[i], del, chorus->dry_wet );

        // Write input + feedback inside buffer
        chorus->buffer->write(in[i] + chorus->feedback * del);
    }
    
    return 0;
}

void ModuleChorus::setBank1(void)
{
    feedback = 0.0f;
    dry_wet = 0.35f;
    size = 2;
    volume = 1;
    
    voices_params[0].delay = 8;
    voices_params[0].depth = 0.005f;
    voices_params[0].weight = 0.5f;
    
    voices_params[1].delay = 17;
    voices_params[1].depth = 0.05f;
    voices_params[1].weight = 0.2f;
    
    int l[(size_t)size];
    for (size_t k = 0; k < size; ++k) l[k] = voices_params[k].delay;
    buffer->set_reader(size, l, jack_get_sample_rate(client));
}

void ModuleChorus::setBank2(void)
{
    feedback = 0.0f;
    dry_wet = 0.35f;
    size = 2;
    volume = 1;
    
    voices_params[0].delay = 10;
    voices_params[0].depth = -0.005f;
    voices_params[0].weight = 0.5f;
    
    voices_params[1].delay = 20;
    voices_params[1].depth = -0.05f;
    voices_params[1].weight = 0.2f;
    
    int l[(size_t)size];
    for (size_t k = 0; k < size; ++k) l[k] = voices_params[k].delay;
    buffer->set_reader(size, l, jack_get_sample_rate(client));
}

void ModuleChorus::setBank3(void)
{
    feedback = 0.0f;
    dry_wet = 0.35f;
    size = 2;
    volume = 1;
    
    voices_params[0].delay = 5;
    voices_params[0].depth = 0.003f;
    voices_params[0].weight = 0.5f;
    
    voices_params[1].delay = 15;
    voices_params[1].depth = 0.01f;
    voices_params[1].weight = 0.2f;
    
    int l[(size_t)size];
    for (size_t k = 0; k < size; ++k) l[k] = voices_params[k].delay;
    buffer->set_reader(size, l, jack_get_sample_rate(client));
}

void ModuleChorus::setBank4(void)
{
    feedback = 0.0f;
    dry_wet = 0.35f;
    size = 2;
    volume = 1;
    
    voices_params[0].delay = 14;
    voices_params[0].depth = -0.004f;
    voices_params[0].weight = 0.5f;
    
    voices_params[1].delay = 26;
    voices_params[1].depth = -0.01f;
    voices_params[1].weight = 0.2f;
    
    int l[(size_t)size];
    for (size_t k = 0; k < size; ++k) l[k] = voices_params[k].delay;
    buffer->set_reader(size, l, jack_get_sample_rate(client));
}