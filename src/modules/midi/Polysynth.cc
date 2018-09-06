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

#include <list>
#include <map>

#include "noyau/midi.h"
#include "noyau/modules/ModuleBase.h"

#include "lib/envelope/ADSR.h"

#include "lib/math/FonctionsOndes.h"

#define NAME "Polysynth"
#define VERSION "1.0"

struct Note
{
    float ramp, rbs;
    float vol;
    ADSR envelope;
};

struct PolysynthDatas
{

    PolysynthDatas(int sr):
        phase(0), sign(1), volume(0.1f),
        param1(3), param2(1.587401051f), phase_distortion(0.5), phase_fix(0),
        
        samplerate(sr),
        
        a(100), d(100), s(0.5), r(100),

        notes_map(),

        waveform(sfx::WHAR),
        tfunc(sfx::castWaveform(sfx::WHAR))
    {
    }        
    
    float phase, sign, volume;
    float param1, param2, phase_distortion, phase_fix;
    
    int samplerate;
    
    float a, d, s, r;
    
    std::list<std::pair<sfx::hex_t, Note>> notes_map;
    
    sfx::WaveForm waveform;
    sfx::transfert_f tfunc;
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo(NAME, VERSION);
}

#ifdef __ARCH_LINUX__

extern "C"
void* function_create_jack_module(EffectUnit* effect)
{
    /*
     * Register the ports with :
     * 
     * module->registerAudioInput("NOM");
     * 
     * module->registerAudioOutput("NOM");
     * 
     * module->registerMidiInput("NOM");
     * 
     * module->registerMidiOutput("NOM");
     */
    /*
     * Do the Jack Related Stuff
     */
    effect->registerMidiInput("Input");
    effect->registerAudioOutput("Output");
    
    sfx::Midi_calcMidiTable(jack_get_sample_rate(effect->client));
    
    return new PolysynthDatas(jack_get_sample_rate(effect->client));
}
#endif

extern "C"
void* function_create_non_jack_module(EffectUnit* effect)
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
    effect->registerMidiInput("Input");
    effect->registerAudioOutput("Output");
    
    return new PolysynthDatas(48000);
}

extern "C"
void function_destroy_module(void* datas)
{
    delete (PolysynthDatas*) datas;
}

extern "C"
Module::SlotTable function_register_module_slots(void)
{
    Module::SlotTable table;

    /*
     * Register all slots with :
     *
    table["NOM"] = Module::Slot{"NOM", 0, [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)mod->datas)->PARAM = sfx::mapfm_lin(val, MIN, MAX); 
     
                // Valeur de gain en db : renvois facteur gain
                ((PolysynthDatas*)mod->datas)->PARAM = sfx::mapfm_db(val, MIN, MAX);  
     
                // Valeur representant une frequence [~20Hz;~20kHz]
                ((PolysynthDatas*)mod->datas)->PARAM = sfx::mapfm_freq(val, MIN, MAX);
      
                // Valeur representant une durée (entree ms ; retour en sample)
                ((PolysynthDatas*)mod->datas)->PARAM = sfx::mapfm_mstos(val, MIN, MAX, SAMPLERATE); 
      
                // Valeur representant une durée (entree sample ; retour en ms)
                ((PolysynthDatas*)mod->datas)->PARAM = sfx::mapfm_stoms(val, MIN, MAX, SAMPLERATE); 
            }
            return ((PolysynthDatas*)mod->datas)->PARAM; // Retourner la valeur du paramètre modulé
        });
     */
    Module::register_slot(table, 0, "phase", "Phase", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->phase = sfx::mapfm_lin(val, 0, 1); 
            }
            return ((PolysynthDatas*)effect->datas)->phase; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "signe", "Signe", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->sign = sfx::mapfm_lin(val, -1, 1); 
            }
            return ((PolysynthDatas*)effect->datas)->sign; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "volume", "Volume", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur de gain en db : renvois facteur gain
                ((PolysynthDatas*)effect->datas)->volume = sfx::mapfm_db(val, -50, 10); 
            }
            return ((PolysynthDatas*)effect->datas)->volume; // Retourner la valeur du paramètre modulé
        });
        
        
    Module::register_slot(table, 0, "param1", "Param1", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->param1 = sfx::mapfm_lin(val, 0, 10);
            }
            return ((PolysynthDatas*)effect->datas)->param1; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "param2", "Param2", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->param1 = sfx::mapfm_lin(val, 0, 10);
            }
            return ((PolysynthDatas*)effect->datas)->param1; // Retourner la valeur du paramètre modulé
        });
        
    Module::register_slot(table, 0, "distortion_phase", "Distortion-Phase", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->phase_distortion = sfx::mapfm_lin(val, 0, 1);
            }
            return ((PolysynthDatas*)effect->datas)->phase_distortion; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "point_fixe_phase", "Point-Fixe-Phase", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->phase_fix = sfx::mapfm_lin(val, 0, 10);
            }
            return ((PolysynthDatas*)effect->datas)->phase_fix; // Retourner la valeur du paramètre modulé
        });
        
    Module::register_slot(table, 0, "a", "A", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->a = sfx::mapfm_mstos(val, 0, 1000, ((PolysynthDatas*)effect->datas)->samplerate); 
            }
            return ((PolysynthDatas*)effect->datas)->a; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "d", "D", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->d = sfx::mapfm_mstos(val, 0, 1000, ((PolysynthDatas*)effect->datas)->samplerate); 
            }
            return ((PolysynthDatas*)effect->datas)->d; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "s", "S", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur variant lineairement
                ((PolysynthDatas*)effect->datas)->s = sfx::mapfm_db(val, -50, 10); 
            }
            return ((PolysynthDatas*)effect->datas)->s; // Retourner la valeur du paramètre modulé
        });
    Module::register_slot(table, 0, "r", "R", [](sfx::hex_t val, EffectUnit* effect)
        {
            if (val < 128)
            {
                // Valeur de gain en db : renvois facteur gain
                ((PolysynthDatas*)effect->datas)->r = sfx::mapfm_mstos(val, 0, 1000, ((PolysynthDatas*)effect->datas)->samplerate); 
            }
            return ((PolysynthDatas*)effect->datas)->r; // Retourner la valeur du paramètre effectulé
        });
        
    return table;
}

//#ifdef __ARCH_LINUX__

extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    EffectUnit* effect = (EffectUnit*) arg;
    PolysynthDatas* synth = (PolysynthDatas*) effect->datas;

    sfx::sample_t *out;
    out = (sfx::sample_t*)jack_port_get_buffer(effect->audioOuts[0].port, nframes);

    void* midi_port = jack_port_get_buffer(effect->midiIns[0].port, nframes);
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

    void* midi_throught = jack_port_get_buffer(effect->midiOuts[0].port, nframes);
    jack_midi_clear_buffer(midi_throught);
    
    float o = synth->phase_fix, d = synth->phase_distortion;
    synth->sign = (synth->sign < 0) ? -1 : 1;

    jack_midi_event_get(&in_event, midi_port, 0);
    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        if (in_event.time == i && event_index < event_count)
        {
            sfx::Midi_reserve_MidiThroughMessage(midi_throught, in_event);
            
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_NoteOn))
            {
                sfx::hex_t note = sfx::Midi_read_NoteValue(in_event.buffer);
                
                Note n = Note{
                    .ramp = synth->phase,
                    .rbs  = sfx::Midi_MidiTable[note],
                    .vol  = sfx::map(sfx::Midi_read_NoteVelocity(in_event.buffer), 0, 128, 0, 1)
                };
                
                n.envelope.setAttackRate(synth->a);
                n.envelope.setDecayRate(synth->d);
                n.envelope.setReleaseRate(synth->r);
                n.envelope.setSustainLevel(synth->s);
                n.envelope.gate(1);

                synth->notes_map.push_back(std::make_pair(note, n));
                
                synth->notes_map.reverse();
                synth->notes_map.unique(
                    [](std::pair<sfx::hex_t, Note> a,
                        std::pair<sfx::hex_t, Note> b)
                    {return a.first == b.first;});
                
                //*
                sfx::debug(NAME, "Midi note on : %x : Note : %i : Velocity : %i : Rbs : %f\n",
                        in_event.buffer[0], in_event.buffer[1], in_event.buffer[2], n.ramp);
                //*/
            }
            else if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_NoteOff))
            {
                sfx::hex_t note = sfx::Midi_read_NoteValue(in_event.buffer);
                
                for (auto& note_struct : synth->notes_map)
                    if(note_struct.first == note)
                    {
                        note_struct.second.envelope.gate(0);
                    }
                
                //*
                sfx::debug(NAME, "Midi note off : %x : Note : %i : Velocity : %i\n",
                        in_event.buffer[0], in_event.buffer[1], in_event.buffer[2]);
                //*/
            }
            else if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
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
        
        out[i] = 0;
        
        for (std::list<std::pair<sfx::hex_t, Note>>::iterator note_itr = synth->notes_map.begin();
            note_itr != synth->notes_map.end();
            ++note_itr)
        {
            if (note_itr->second.envelope.getState())
            {
                note_itr->second.ramp += note_itr->second.rbs;
                note_itr->second.ramp = fmod(note_itr->second.ramp - o, 1.0f);

                float pp = note_itr->second.ramp;
                if ( d != 0.5f )
                {
                    pp = (pp>d)? (pp+1.0f - 2.0f*d)/(2.0f*(1.0f-d)) : pp/(2.0f*d);
                    pp += o;
                    pp = fmod(pp, 1.0f);
                }

                //note_itr->second.envelope.process();
                out[i] += synth->volume * note_itr->second.envelope.process() * (*synth->tfunc)(pp, synth->sign, synth->param1, synth->param2);
            }
            else
            {
                note_itr = synth->notes_map.erase(note_itr);
                //sfx::debug(NAME, "Erased note : %u left\n", synth->notes_map.size());
            }
        }
        
        //sfx::debug(NAME, "%u left\n", synth->notes_map.size());
        
        if (out[i]>1.0f) out[i] = 1.0f;
        else if (out[i]<-1.0f) out[i] = -1.0f;
    }

    return 0;
}
//#endif