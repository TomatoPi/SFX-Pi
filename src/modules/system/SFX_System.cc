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
#include <cstring>

#include "noyau/midi.h"
#include "noyau/modules/ModuleBase.h"

#define UNIQUE_NAME "sfx_system"
#ifdef __SFX_PI__
#define NAME "SFX-Pi"
#else
#define NAME "SFX-Pe"
#endif
#define VERSION "1.0"

struct MODULE_DATAS
{
    MODULE_DATAS()
    {
    }
#ifdef __SFX_PI__
    MODULE_DATAS(jack_client_t* client):
    unit_client(client)
    {
        for (size_t i = 0; i < 4; ++i)
            outputs[i] = sfx::registerAudioOutput(client, sfx::formatString("sys_playback_%u", i));
        
        for (size_t i = 0; i < 2; ++i)
            inputs[i] = sfx::registerAudioInput(client, sfx::formatString("sys_capture_%u", i));
        
        midi_out = sfx::registerMidiOutput(client, sfx::formatString("sys_midi_playback"));
        midi_in = sfx::registerMidiInput(client, sfx::formatString("sys_midi_capture"));
    }
    ~MODULE_DATAS()
    {
        for (size_t i = 0; i < 4; ++i)
            jack_port_unregister(unit_client, outputs[i]);
        
        for (size_t i = 0; i < 2; ++i)
            jack_port_unregister(unit_client, inputs[i]);
        
        jack_port_unregister(unit_client, midi_in);
        jack_port_unregister(unit_client, midi_out);
    }
    
    jack_port_t *outputs[4], *inputs[2], *midi_in, *midi_out;
    jack_client_t *unit_client;
#endif
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo(UNIQUE_NAME, NAME, VERSION, Module::ShortInfo::SYSTEM);
}

#ifdef __SFX_PI__
extern "C"
void* function_create_jack_module(EffectUnit* effect)
{
    /*
     * Register the ports with :
     * 
     * effect->registerAudioInput("NOM");
     * 
     * effect->registerAudioOutput("NOM");
     * 
     * effect->registerMidiInput("NOM");
     * 
     * effect->registerMidiOutput("NOM");
     */
    /*
     * Do the Jack Related Stuff
     */
    
    effect->registerAudioInput("Playback_1");
    effect->registerAudioInput("Playback_2");
    effect->registerAudioInput("Playback_3");
    effect->registerAudioInput("Playback_4");
    
    effect->registerAudioOutput("Capture_1");
    effect->registerAudioOutput("Capture_2");
    
    effect->registerMidiInput("Midi_Playback_1");
    effect->registerMidiOutput("Midi_Capture_1");
    
    return new MODULE_DATAS(effect->getClient());
}
#else
extern "C"
void* function_create_non_jack_module(EffectUnit* effect)
{
    /*
     * Register the ports with :
     * 
     * effect->registerAudioInput("NOM");
     * 
     * effect->registerAudioOutput("NOM");
     * 
     * effect->registerMidiInput("NOM");
     * 
     * effect->registerMidiOutput("NOM");
     */
    
    effect->registerAudioInput("Playback_1");
    effect->registerAudioInput("Playback_2");
    effect->registerAudioInput("Playback_3");
    effect->registerAudioInput("Playback_4");
    
    effect->registerAudioOutput("Capture_1");
    effect->registerAudioOutput("Capture_2");
    
    effect->registerMidiInput("Midi_Playback_1");
    effect->registerMidiOutput("Midi_Capture_1");
    
    return new MODULE_DATAS();
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

    Module::register_slot(table, Module::Slot::POST_INIT, 0, "0_sys_connect_ports","Connect-Physical-Ports"
        , [](sfx::hex_t val, EffectUnit* effect) -> float
        {
#ifdef __SFX_PI__
            const char **physic_in;
            physic_in = jack_get_ports (effect->getClient(), NULL, NULL,
                        JackPortIsPhysical|JackPortIsOutput);
            if (physic_in == NULL) 
            {
                sfx::err(NAME, "no physical capture ports\n");
            }

            const char **physic_out;
            physic_out = jack_get_ports (effect->getClient(), NULL, NULL,
                        JackPortIsPhysical|JackPortIsInput);
            if (physic_out == NULL) 
            {
                sfx::err(NAME, "no physical playback ports\n");
            }

            const char **midi_in;
            midi_in = jack_get_ports(effect->getClient(), NULL, JACK_DEFAULT_MIDI_TYPE,
                    JackPortIsPhysical|JackPortIsOutput);
            if ( midi_in == NULL ){
                sfx::err(NAME, "No Physical MIDI Capture Ports");
            }

            const char **midi_out;
            midi_out = jack_get_ports(effect->getClient(), NULL, JACK_DEFAULT_MIDI_TYPE,
                    JackPortIsPhysical|JackPortIsInput);
            if ( midi_out == NULL ){
                sfx::err(NAME, "No Physical MIDI Playback Ports");
            }

            for (size_t i = 0; i < 2; ++i)
                if (jack_connect (effect->getClient(), physic_in[i], jack_port_name (((MODULE_DATAS*) effect->getDatas())->inputs[i]))) 
                    sfx::err (NAME, "cannot connect capture port %u\n", i);

            for (size_t i = 0; i < 4; ++i)
                if (jack_connect (effect->getClient(), jack_port_name (((MODULE_DATAS*) effect->getDatas())->outputs[i]), physic_out[i])) 
                    sfx::err (NAME, "cannot connect playback port %u\n", i);

            if (jack_connect (effect->getClient(), midi_in[0], jack_port_name (((MODULE_DATAS*) effect->getDatas())->midi_in))) 
                    sfx::err (NAME, "cannot connect midi capture port %u\n", 0);

            if (jack_connect (effect->getClient(), jack_port_name (((MODULE_DATAS*) effect->getDatas())->midi_out), midi_out[0])) 
                    sfx::err (NAME, "cannot connect midi playback port %u\n", 0);

            free (physic_in);
            free (physic_out);
            free (midi_in);
            free (midi_out);
#endif
            return 0;
        });

    return table;
}

#ifdef __SFX_PI__
extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    EffectUnit* effect = (EffectUnit*) arg;
    MODULE_DATAS* datas = (MODULE_DATAS*) effect->getDatas();

    for (size_t i = 0; i < 2; ++i)
    {
        sfx::sample_t *physic, *virt;
        physic = (sfx::sample_t*)jack_port_get_buffer(datas->inputs[i], nframes);
        virt = (sfx::sample_t*)jack_port_get_buffer(effect->getAudioOutputs(i).port, nframes);
        
        // From physic to Virtual
        memcpy(virt, physic, nframes * sizeof(sfx::sample_t));
    }
    for (size_t i = 0; i < 4; ++i)
    {
        sfx::sample_t *physic, *virt;
        physic = (sfx::sample_t*)jack_port_get_buffer(datas->outputs[i], nframes);
        virt = (sfx::sample_t*)jack_port_get_buffer(effect->getAudioInputs(i).port, nframes);
        
        // From Virtual to Physic
        memcpy(physic, virt, nframes * sizeof(sfx::sample_t));
    }
    
    if (1)
    {
        // From Physic to Virtual
        void* midi_phy_capt = jack_port_get_buffer(datas->midi_in, nframes);
        void* midi_vir_capt = jack_port_get_buffer(effect->getMidiOutputs(1).port, nframes);
        jack_midi_clear_buffer(midi_vir_capt);
    
        jack_midi_event_t in_event;
        jack_nframes_t event_count = jack_midi_get_event_count(midi_phy_capt);
        
        for (jack_nframes_t i = 0; i < event_count; ++i)
        {
            jack_midi_event_get(&in_event, midi_phy_capt, i);
            sfx::Midi_reserve_MidiThroughMessage(midi_vir_capt, in_event);
        }
        
        // From Virtual to Physic
        void* midi_phy_play = jack_port_get_buffer(datas->midi_out, nframes);
        void* midi_vir_play = jack_port_get_buffer(effect->getMidiInputs(1).port, nframes);
        jack_midi_clear_buffer(midi_phy_play);
        
        event_count = jack_midi_get_event_count(midi_vir_play);
        
        for (jack_nframes_t i = 0; i < event_count; ++i)
        {
            jack_midi_event_get(&in_event, midi_vir_play, i);
            sfx::Midi_reserve_MidiThroughMessage(midi_phy_play, in_event);
        }
        
        // Slot Related MIDI
        void* midi_port = jack_port_get_buffer(effect->getMidiInputs(0).port, nframes);
        void* midi_throught = jack_port_get_buffer(effect->getMidiOutputs(0).port, nframes);
        jack_midi_clear_buffer(midi_throught);
        
        event_count = jack_midi_get_event_count(midi_port);
        
        for (jack_nframes_t i = 0; i < event_count; ++i)
        {
            jack_midi_event_get(&in_event, midi_port, i);
            sfx::Midi_reserve_MidiThroughMessage(midi_throught, in_event);
            
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                effect->veryfyAndComputeCCMessage(in_event.buffer);
            }
        }
    }

    return 0;
}
#endif