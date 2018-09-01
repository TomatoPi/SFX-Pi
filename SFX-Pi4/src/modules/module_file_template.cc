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
#include "noyau/modules/ModuleBase.h"

#define NAME "MODULE_NAME"
#define VERSION "1.0.0"

struct MODULE_DATAS
{
    MODULE_DATAS()
    {
        
    }
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo{NAME, VERSION};
}

#ifdef __ARCH_LINUX__
extern "C"
void* function_create_jack_module(Module* module)
{
    /*
     * Register the ports with :
     * 
     * module->registerAudioInput("NOM"));
     * 
     * module->registerAudioOutput("NOM"));
     * 
     * module->registerMidiInput("NOM"));
     * 
     * module->registerMidiOutput("NOM"));
     */
    /*
     * Do the Jack Related Stuff
     */
    return new MODULE_DATAS();
}
#endif

extern "C"
void* function_create_non_jack_module(Module* module)
{
    /*
     * Register the ports with :
     * 
     * module->registerAudioInput("NOM"));
     * 
     * module->registerAudioOutput("NOM"));
     * 
     * module->registerMidiInput("NOM"));
     * 
     * module->registerMidiOutput("NOM"));
     */
    return new MODULE_DATAS();
}

extern "C"
void function_destroy_module(void* datas)
{
    delete (MODULE_DATAS*)datas;
}

extern "C"
Module::SlotTable function_register_module_slots(void)
{
    Module::SlotTable table;
    
    /*
     * Register all slots with :
     *
    table["NOM"] = Module::Slot{"NOM", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                // Code du Slot ici
            }
            return 0; // Retourner la valeur du paramètre modulé
        }};
     */
    
    return table;
}

#ifdef __ARCH_LINUX__
extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    Module* mod = (Module*) arg;
    MODULE_DATAS* disto = (MODULE_DATAS*) mod->datas;
    
    sfx::sample_t *in, *out;
    in  = (sfx::sample_t*)jack_port_get_buffer( mod->audioIns[0].port , nframes );
    out = (sfx::sample_t*)jack_port_get_buffer( mod->audioOuts[0].port, nframes );

    void* midi_port = jack_port_get_buffer(mod->midiIns[0].port, nframes);
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
    
    void* midi_out = jack_port_get_buffer(mod->midiOuts[0].port, nframes);
    jack_midi_clear_buffer(midi_out);
    
    jack_midi_event_get(&in_event, midi_port, 0);
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        if (in_event.time == i && event_index < event_count)
        {
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                
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
    }
    
    return 0;
}
#endif