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
 * File:   LFO.cc
 * Author: Space-Computer
 * 
 * Created on 11 août 2018, 02:37
 */

#include "LFO.h"

#define NAME "LFO"

ModuleLFO::ModuleLFO() :
client(nullptr),
midi_in(nullptr), audio_out(nullptr),

waveform(WSIN),
frequency(0.5f), phase(0), sign(1), volume(1),
param1(0), param2(0), phase_distortion(0.5), phase_fix(0),
        
ramp(0), rbs(0),

tfunc(castWaveform(WSIN))
{
    client = sfx::openJackClient(NAME);

    jack_set_process_callback(client, jackCallback, this);

    midi_in = sfx::registerMidiInput(client, "Input");
    audio_out = sfx::registerAudioOutput(client, "Output");

    sfx::activateClient(client);
    
    sfx::Midi_calcMidiTable(jack_get_sample_rate(client));
    this->setBank1();
}

ModuleLFO::~ModuleLFO()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModuleLFO::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModuleLFO* lfo = (ModuleLFO*) arg;
    sfx::sample_t *out = (sfx::sample_t*)jack_port_get_buffer(lfo->audio_out, nframes);

    lfo->sign = (lfo->sign < 0) ? -1 : 1;

    float o = lfo->phase_fix, d = lfo->phase_distortion;

    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        lfo->ramp += lfo->rbs;
        lfo->ramp = fmod(lfo->ramp - o, 1.0f);

        float pp = lfo->ramp;
        if ( d != 0.5f ){
            pp = (pp>d)? (pp+1.0f - 2.0f*d)/(2.0f*(1.0f-d)) : pp/(2.0f*d);
            pp += o;
            pp = fmod(pp, 1.0f);
        }

        out[i] = lfo->volume * (*lfo->tfunc)(pp, lfo->sign, lfo->param1, lfo->param2);
    }

    return 0;
}

void ModuleLFO::setBank1(void)
{
    waveform = WSIN;
    
    frequency = 2.0f;
    phase = 0;
    sign = 1;
    volume = 1;
    
    param1 = 0;
    param2 = 0;
    
    phase_distortion = 0.5f;
    phase_fix = 0;
    
    ramp = 0;
    rbs = frequency / jack_get_sample_rate(client);
    
    tfunc = castWaveform(waveform);
}