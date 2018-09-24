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

#include "noyau/midi.h"
#include "noyau/utils.h"
#include "noyau/modules/ModuleBase.h"

#include "lib/filter/GraphicEQ.h"

#define UNIQUE_NAME "filtre_tonestack_3_bandes"
#define NAME "3-ToneStack"
#define VERSION "1.0"

struct MODULE_DATAS
{

    MODULE_DATAS(int sr):
        
        samplerate(sr),
        
        tone_eq(nullptr),
        
        low(220),high(830),
        gains(),
        
        volume(1)
    {
        float f[] = {low, high};
        tone_eq  = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, samplerate));
        
        gains[0] = 1;
        gains[1] = 1;
        gains[2] = 1;
    }

    int samplerate;
    
    std::unique_ptr<GraphicEQ> tone_eq;
    
    float low, high;

    float gains[3];
    
    float volume;
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo(UNIQUE_NAME, NAME, VERSION);
}

#ifdef __SFX_PI__
extern "C"
void* function_create_jack_module(EffectUnit* effect)
{
    effect->registerAudioInput("Input");
    effect->registerAudioOutput("Output");
            
    return new MODULE_DATAS(jack_get_sample_rate(effect->getClient()));
}
#else
extern "C"
void* function_create_non_jack_module(EffectUnit* effect)
{
    effect->registerAudioInput("Input");
    effect->registerAudioOutput("Output");
           
    return new MODULE_DATAS(48000);
}
#endif

extern "C"
void function_destroy_module(void* datas)
{
    delete (MODULE_DATAS*) datas;
}

extern "C"
Module::SlotTable function_register_module_slots(void)
{
    Module::SlotTable table;
    
    Module::register_slot(table, 42, "volume", "Volume", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((MODULE_DATAS*)effect->getDatas())->volume = sfx::mapfm_db(val, -50, +10);
            return ((MODULE_DATAS*)effect->getDatas())->volume;
        });

    Module::register_slot(table, 57, "lowcut", "Lowcut", [](sfx::hex_t val, EffectUnit* effect)
       {
           if (val < 128)
           {
               float res = sfx::mapfm_freq(val);
               ((MODULE_DATAS*)effect->getDatas())->low = res;
               ((MODULE_DATAS*)effect->getDatas())->tone_eq->setFrequency(0, res, ((MODULE_DATAS*)effect->getDatas())->samplerate);
           }
           return ((MODULE_DATAS*)effect->getDatas())->low;
       });
    Module::register_slot(table, 79, "highcut", "Highcut", [](sfx::hex_t val, EffectUnit* effect)
       {
           if (val < 128)
           {
               float res = sfx::mapfm_freq(val);
               ((MODULE_DATAS*)effect->getDatas())->high = res;
               ((MODULE_DATAS*)effect->getDatas())->tone_eq->setFrequency(1, res, ((MODULE_DATAS*)effect->getDatas())->samplerate);
           }
           return ((MODULE_DATAS*)effect->getDatas())->high;
       });
   Module::register_slot(table, 75, "lowgain", "Lowgain", [](sfx::hex_t val, EffectUnit* effect)
       {
           if (val < 128)
               ((MODULE_DATAS*)effect->getDatas())->gains[0] = sfx::mapfm_db(val, -30, 30);
           return ((MODULE_DATAS*)effect->getDatas())->gains[0];
       });
   Module::register_slot(table, 80, "midgain", "Midgain", [](sfx::hex_t val, EffectUnit* effect)
       {
           if (val < 128)
               ((MODULE_DATAS*)effect->getDatas())->gains[1] = sfx::mapfm_db(val, -30, 30);
           return ((MODULE_DATAS*)effect->getDatas())->gains[1];
       });
   Module::register_slot(table, 87, "highgain", "Highgain", [](sfx::hex_t val, EffectUnit* effect)
       {
           if (val < 128)
               ((MODULE_DATAS*)effect->getDatas())->gains[2] = sfx::mapfm_db(val, -30, 30);
           return ((MODULE_DATAS*)effect->getDatas())->gains[2];
       });

    return table;
}

#ifdef __SFX_PI__
extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    EffectUnit* effect = (EffectUnit*) arg;
    MODULE_DATAS* datas = (MODULE_DATAS*) effect->getDatas();

    sfx::sample_t *in, *out;
    in = (sfx::sample_t*)jack_port_get_buffer(effect->getAudioInputs(0).port, nframes);
    out = (sfx::sample_t*)jack_port_get_buffer(effect->getAudioOutputs(0).port, nframes);

    void* midi_port = jack_port_get_buffer(effect->getMidiInputs(0).port, nframes);
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

    void* midi_throught = jack_port_get_buffer(effect->getMidiOutputs(0).port, nframes);
    jack_midi_clear_buffer(midi_throught);

    jack_midi_event_get(&in_event, midi_port, 0);
    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        if (in_event.time == i && event_index < event_count)
        {
            sfx::Midi_reserve_MidiThroughMessage(midi_throught, in_event);

            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                effect->veryfyAndComputeCCMessage(in_event.buffer);
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }

        /*
         * Code du callback ici
         */
        out[i] = datas->volume * datas->tone_eq->compute(in[i], 3, datas->gains);
    }

    return 0;
}
#endif