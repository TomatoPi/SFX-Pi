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

#include <deque>

#include "noyau/midi.h"
#include "noyau/modules/ModuleBase.h"

#define UNIQUE_NAME "natif_tap_tempo"
#define NAME "TapTempo"
#define VERSION "1.0"

struct MODULE_DATAS
{

    MODULE_DATAS():
    clock(0), next_clock(0),
    delta_tempo(0), delta_on(0),
    source(69), tempo(0),

    signature_num(4), signature_den(4),
    signature_count(0),

    last_time()
    {

    }
    
    jack_time_t clock, next_clock;
    jack_time_t delta_tempo, delta_on;
    
    sfx::hex_t source;
    int tempo;
    
    int signature_num, signature_den;
    int signature_count;
    
    std::deque<jack_time_t> last_time;
    
    enum {
        ON,
        OFF
    }status;
    
    void setSignature(int num, int den)
    {
        if (num != 0 && den != 0)
        {
            signature_num = num;
            signature_den = den;

            signature_count = 0;

            delta_tempo = (60000000 * 4) / (signature_den * tempo);
            delta_on = delta_tempo * 0.125;

            sfx::log(NAME, "Tempo : %i  Signature : %i/%i\n", tempo, signature_num, signature_den);
        }
        else
        {
            sfx::wrn(NAME, "Invalid Signature : %i/%i\n", num, den);
        }
    }
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo(UNIQUE_NAME, NAME, VERSION);
}

#ifdef __ARCH_LINUX__

extern "C"
void* function_create_jack_module(EffectUnit* effect)
{
    /*
     * Register the ports with :
     * 
     * effect->registerAudioInput("NOM"));
     * 
     * effect->registerAudioOutput("NOM"));
     * 
     * effect->registerMidiInput("NOM"));
     * 
     * effect->registerMidiOutput("NOM"));
     */
    /*
     * Do the Jack Related Stuff
     */
    effect->registerMidiInput("ClickIn");
    effect->registerMidiOutput("TempoOut");
    return new MODULE_DATAS();
}
#endif

extern "C"
void* function_create_non_jack_module(EffectUnit* effect)
{
    /*
     * Register the ports with :
     * 
     * effect->registerAudioInput("NOM"));
     * 
     * effect->registerAudioOutput("NOM"));
     * 
     * effect->registerMidiInput("NOM"));
     * 
     * effect->registerMidiOutput("NOM"));
     */
    effect->registerMidiInput("ClickIn");
    effect->registerMidiOutput("TempoOut");
    
    return new MODULE_DATAS();
}

extern "C"
void function_destroy_module(void* datas)
{
    delete (MODULE_DATAS*) datas;
}

extern "C"
Module::SlotTable function_register_module_slots(void)
{
    Module::SlotTable table;

    /*
     * Register all slots with :
     *
    Module::register_slot(table, VALEUR_DEFAUT, "NOM_INTERNE","NOM"
        , [](sfx::hex_t val, EffectUnit* effect) -> float
        {
            if (val < 128)
            {
                // Code du Slot ici
     
                // Valeur variant lineairement
                ((MODULE_DATAS*)effect->datas)->PARAM = sfx::mapfm_lin(val, MIN, MAX); 
     
                // Valeur de gain en db : renvois facteur gain
                ((MODULE_DATAS*)effect->datas)->PARAM = sfx::mapfm_db(val, MIN, MAX);  
     
                // Valeur representant une frequence [~20Hz;~20kHz]
                ((MODULE_DATAS*)effect->datas)->PARAM = sfx::mapfm_freq(val, MIN, MAX);
      
                // Valeur representant une durée (entree ms ; retour en sample)
                ((MODULE_DATAS*)effect->datas)->PARAM = sfx::mapfm_mstos(val, MIN, MAX, SAMPLERATE); 
      
                // Valeur representant une durée (entree sample ; retour en ms)
                ((MODULE_DATAS*)effect->datas)->PARAM = sfx::mapfm_stoms(val, MIN, MAX, SAMPLERATE); 
      
                // Valeur prenant un nombre discret de valeurs : renvois l'indice courant
                ((MODULE_DATAS*) effect->datas)->param1 = sfx::mapfm_enum(val, 3);
        
            }
            return ((MODULE_DATAS*)effect->datas)->PARAM; // Retourner la valeur du paramètre modulé
        });
     */
    
    Module::register_slot(table, 12, "signature_num","Numerateur"
        , [](sfx::hex_t val, EffectUnit* effect) -> float
        {
            if (val < 128)
            {
                // Valeur prenant un nombre discret de valeurs : renvois l'indice courant
                ((MODULE_DATAS*) effect->datas)->signature_num = 1+sfx::mapfm_enum(val, 32);
            }
            return ((MODULE_DATAS*)effect->datas)->signature_num; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 49, "signature_den","Denominateur"
        , [](sfx::hex_t val, EffectUnit* effect) -> float
        {
            if (val < 128)
            {
                // Valeur prenant un nombre discret de valeurs : renvois l'indice courant
                ((MODULE_DATAS*) effect->datas)->signature_den = 1 << sfx::mapfm_enum(val, 6);
            }
            return ((MODULE_DATAS*)effect->datas)->signature_den; // Retourner la valeur du paramètre modulé
        });

    return table;
}

//#ifdef __ARCH_LINUX__

extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    EffectUnit* effect = (EffectUnit*) arg;
    MODULE_DATAS* tap = (MODULE_DATAS*) effect->datas;

    void* midi_port = jack_port_get_buffer(effect->midiIns[0].port, nframes);
    void* midi_throught = jack_port_get_buffer(effect->midiOuts[0].port, nframes);
    jack_midi_clear_buffer(midi_throught);
    
    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(midi_port);
    
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, midi_port, i);
        sfx::Midi_reserve_MidiThroughMessage(midi_port, in_event);

        if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
        {
            effect->veryfyAndComputeCCMessage(in_event.buffer);
        }
    }

    /*
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, click_port, i);
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
            , i, in_event.time, *(in_event.buffer));
    }
    //*/
    
    void* click_port = jack_port_get_buffer(effect->midiIns[1].port, nframes);
    void* tempo_buffer = jack_port_get_buffer(effect->midiOuts[1].port, nframes);
    jack_midi_clear_buffer(tempo_buffer);
    
    event_count = jack_midi_get_event_count(click_port);

    bool clicked = false;
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, click_port, i);

        /*
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
                , i, in_event.time, *(in_event.buffer));
        //*/

        if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
        {
            if (sfx::Midi_verify_CCSource(in_event.buffer, tap->source))
            {
                clicked = !!in_event.buffer[2];
                //sfx::debug(NAME, "Clicked\n");
            }
                //sfx::debug(NAME, "Released\n");
                
        }
        else sfx::wrn(NAME, "Unknown Event Type\n");
    }

    jack_time_t block_start = jack_frames_to_time(effect->client
            , jack_last_frame_time(effect->client));
    jack_time_t block_end = block_start + jack_frames_to_time(effect->client, nframes);
            
    if (clicked)
    {
        tap->last_time.push_back(block_start);
        //sfx::debug(NAME, "TIME Count : %i\n", tap->last_time.size());
        
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

            tap->clock = block_start;
            tap->next_clock = block_start + tap->delta_tempo;
            tap->status = MODULE_DATAS::OFF;
        }
    }
    if (tap->last_time.size() != 0 && block_start - tap->last_time.back() > 5000000)
    {
        tap->last_time.clear();
    }
    if (tap->tempo != 0)
    {
        jack_time_t delta_on = tap->signature_count == 0 ? tap->delta_on * 2 : tap->delta_on;

        if (tap->status == MODULE_DATAS::ON && (tap->clock + delta_on < block_end))
        {
            //sfx::debug (NAME, "Led Off : Clock %llu : Delta %llu : Time : %llu\n",
            //        tap->clock, delta_on, block_start);
            sfx::Midi_reserve_UpdateOutput(tempo_buffer, 0, tap->source, false);
            tap->status = MODULE_DATAS::OFF;
        }
        else
        if (tap->next_clock < block_end)
        {
            if (tap->signature_den < 4 || tap->signature_count % (tap->signature_den / 4) == 0)
            {
                //sfx::debug (NAME, "Led On : Clock %llu : Delta %llu : Time : %llu\n",
                //    tap->clock, delta_on, block_start);
                sfx::Midi_reserve_UpdateOutput(tempo_buffer, 0, tap->source, true);
                tap->status = MODULE_DATAS::ON;
            }
            tap->clock = tap->next_clock;
            tap->next_clock += tap->delta_tempo;
            tap->signature_count = (tap->signature_count + 1) % tap->signature_num;
        }
    }

    return 0;
}
//#endif