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
 * File:   TapTempo.cc
 * Author: Space-Computer
 * 
 * Created on 23 juillet 2018, 15:03
 */

#include "TapTempo.h"

#define NAME "TapTempo"

ModuleTapTempo::ModuleTapTempo() :
client(nullptr),
midi_in(nullptr), midi_out(nullptr),

clock(0), delta_tempo(0), delta_on(0),
source(69), tempo(0),

signature_num(4), signature_den(4),
signature_count(0),

last_time()
{
    client = sfx::openJackClient(NAME);

    jack_set_process_callback(client, jackCallback, this);

    midi_in = sfx::registerMidiInput(client, "ClickIn");
    midi_out = sfx::registerMidiOutput(client, "TempoOut");

    sfx::activateClient(client);
}

ModuleTapTempo::~ModuleTapTempo()
{
    jack_client_close(client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int ModuleTapTempo::jackCallback(jack_nframes_t nframes, void* arg)
{
    ModuleTapTempo* tap = (ModuleTapTempo*) arg;
    void* port_buff = jack_port_get_buffer(tap->midi_in, nframes);

    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buff);

    bool clicked = false;

    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, port_buff, i);

        //*
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
                , i, in_event.time, *(in_event.buffer));
        //*/

        if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
        {
            if (sfx::Midi_verify_CCSource(in_event.buffer, tap->source))
                clicked = !!in_event.buffer[2];
        }
        else sfx::wrn(NAME, "Unknown Event Type\n");
    }

    jack_midi_clear_buffer(port_buff);

    jack_time_t time = jack_get_time();
    if (clicked)
    {
        tap->last_time.push_back(time);
        while (tap->last_time.size() > 4) tap->last_time.pop_front();

        //for (auto& t : tap->last_time) sfx::debug(NAME, "TIME list : %llu\n", t);

        if (tap->last_time.size() == 4)
        {
            tap->delta_tempo = (tap->last_time.back() - tap->last_time.front()) / 3;

            tap->tempo = (60000000) / (tap->delta_tempo);

            tap->setSignature(tap->signature_num, tap->signature_den);

            /*
            sfx::debug(NAME, "Tempo Calculated : t1:%llu t2:%llu tempo:%i, delta_Tempo:%llu, delta_on:%llu\n"
                    ,tap->last_time.front(), tap->last_time.back(), tap->tempo
                    ,tap->delta_tempo, tap->delta_on);
            //*/

            tap->clock = time;
            tap->status = OFF;
        }
    }
    if (tap->last_time.size() != 0 && time - tap->last_time.back() > 5000000)
    {
        tap->last_time.clear();
    }
    if (tap->tempo != 0)
    {
        jack_time_t delta = time - tap->clock;

        void* tempo_buffer = jack_port_get_buffer(tap->midi_out, nframes);
        jack_midi_clear_buffer(tempo_buffer);

        jack_time_t delta_on = tap->signature_count == 1 ? tap->delta_on * 4 : tap->delta_on;

        /*
        if (delta < delta_on && tap->status == OFF 
            && tap->signature_count%(tap->signature_den/4) == 0)
        {
            sfx::debug(NAME, "1Allumage Led : Count : %i Index : %i Mod : %i\n"
            , tap->signature_count, tap->signature_den/4
            , tap->signature_count%(tap->signature_den/4));
                
            sfx::Midi_reserve_UpdateOutput(tempo_buffer, 0, tap->target, true);
            tap->status = ON;
        }
        else 
        //*/
        if (delta > delta_on && tap->status == ON)
        {
            sfx::Midi_reserve_UpdateOutput(tempo_buffer, 0, tap->source, false);
            tap->status = OFF;

        }
        if (delta > tap->delta_tempo)
        {
            if (tap->signature_count % (tap->signature_den / 4) == 0)
            {
                sfx::Midi_reserve_UpdateOutput(tempo_buffer, 0, tap->source, true);
                tap->status = ON;
            }
            tap->clock = time;
            tap->signature_count = (tap->signature_count + 1) % tap->signature_num;
        }
    }

    return 0;
}

void ModuleTapTempo::setSignature(int num, int den)
{
    if (num != 0 && den != 0)
    {
        signature_num = num;
        signature_den = den;

        signature_count = num - 1;

        delta_tempo = (60000000 * 4) / (signature_den * tempo);
        delta_on = delta_tempo * 0.125f;

        sfx::log(NAME, "Tempo : %i  Signature : %i/%i\n", tempo, signature_num, signature_den);
    }
    else
    {
        sfx::wrn(NAME, "Invalid Signature : %i/%i\n", num, den);
    }
}