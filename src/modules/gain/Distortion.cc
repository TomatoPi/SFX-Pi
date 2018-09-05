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

#include <math.h>

#include "noyau/midi.h"
#include "noyau/utils.h"
#include "noyau/modules/ModuleBase.h"

#include "lib/filter/GraphicEQ.h"

#define NAME "Distortion"

struct DistortionDatas
{
    DistortionDatas(int samplerate):shape(0.37), softness(8.4), gain(748), volume(0.01f),
        samplerate(samplerate),
        tone_in(nullptr), tone_out(nullptr),
        seuil(0.8), shoot(2), knee(0.01), pout(0)
    {
        // Setup Equalizer, Param used here are not important
        float f[] = {100, 1000};
        tone_in  = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, samplerate));
        tone_out = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, samplerate));

        tone_in_params.low  = 220;
        tone_in_params.high = 830;

        tone_in_params.gains[0] = 1;//1.81f;
        tone_in_params.gains[1] = 1;//2.26f;
        tone_in_params.gains[2] = 1;//1.17f;

        tone_out_params.low  = 246;
        tone_out_params.high = 622;

        tone_out_params.gains[0] = 1;//1.31f;
        tone_out_params.gains[1] = 1;//2.02f;
        tone_out_params.gains[2] = 1;//1.63f;
    }
    
    float shape, softness, gain, volume;
    int samplerate;
    
    std::unique_ptr<GraphicEQ> tone_in, tone_out;
    
    struct GEQ_Params
    {
        float low;
        float high;
        
        float gains[3];
    }tone_in_params, tone_out_params;
    
    float seuil, shoot, knee, pout;
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo{NAME, "1.0.0"};
}

#ifdef __ARCH_LINUX__
extern "C"
void* function_create_jack_module(Module* module)
{
    module->registerAudioInput("Input");
    module->registerAudioOutput("Output");
            
    return new DistortionDatas(jack_get_sample_rate(module->client));
}
#endif

extern "C"
void* function_create_non_jack_module(Module* module)
{
    module->registerAudioInput("Input");
    module->registerAudioOutput("Output");
            
    return new DistortionDatas(48000);
}

extern "C"
void function_destroy_module(void* datas)
{
    delete (DistortionDatas*)datas;
}

float test_slot(sfx::hex_t v, Module* mod) { return 1.0f; };

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
     
                // Valeur variant lineairement
                //((MODULE_DATAS*)mod->datas)->PARAM = sfx::mapfm_lin(val, MIN, MAX); 
     
                // Valeur de gain en db : renvois facteur gain
                //((MODULE_DATAS*)mod->datas)->PARAM = sfx::mapfm_db(val, MIN, MAX);  
     
                // Valeur representant une frequence [~20Hz;~20kHz]
                //((MODULE_DATAS*)mod->datas)->PARAM = sfx::mapfm_freq(val, MIN, MAX);
            }
            return ((MODULE_DATAS*)mod->datas)->PARAM; // Retourner la valeur du paramètre modulé
        }};
     */
    
    table["Shape"] = Module::Slot{"Shape", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->shape = sfx::mapfm_lin(val, 0, 1);
            return ((DistortionDatas*)mod->datas)->shape;
        }};
    table["Softness"] = Module::Slot{"Softness", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->softness = sfx::mapfm_lin(val, 1, 60);
            return ((DistortionDatas*)mod->datas)->softness;
        }};
    table["Gain"] = Module::Slot{"Gain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->gain = sfx::mapfm_db(val, -10, +50);
            return ((DistortionDatas*)mod->datas)->gain;
        }};
    table["Volume"] = Module::Slot{"Volume", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->volume = sfx::mapfm_db(val, -50, +10);
            return ((DistortionDatas*)mod->datas)->volume;
        }};
        
        /////////////////////////////////////////////////////////////
        // Input Filter
        /////////////////////////////////////////////////////////////
        
    table["In Lowcut"] = Module::Slot{"In Lowcut", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                float res = sfx::mapfm_freq(val);
                ((DistortionDatas*)mod->datas)->tone_in_params.low = res;
                ((DistortionDatas*)mod->datas)->tone_in->setFrequency(0, res, jack_get_sample_rate(mod->client));
            }
            return ((DistortionDatas*)mod->datas)->tone_in_params.low;
        }};
    table["In Highcut"] = Module::Slot{"In Highcut", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                float res = sfx::mapfm_freq(val);
                ((DistortionDatas*)mod->datas)->tone_in_params.high = res;
                ((DistortionDatas*)mod->datas)->tone_in->setFrequency(1, res, jack_get_sample_rate(mod->client));
            }
            return ((DistortionDatas*)mod->datas)->tone_in_params.high;
        }};
    table["In Lowgain"] = Module::Slot{"In Lowgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_in_params.gains[0] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_in_params.gains[0];
        }};
    table["In Midgain"] = Module::Slot{"In Midgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_in_params.gains[1] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_in_params.gains[1];
        }};
    table["In Highgain"] = Module::Slot{"In Highgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_in_params.gains[2] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_in_params.gains[2];
        }};
        
        /////////////////////////////////////////////////////////////
        // Output Filter
        /////////////////////////////////////////////////////////////
        
    table["Out Lowcut"] = Module::Slot{"Out Lowcut", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                float res = sfx::mapfm_freq(val);
                ((DistortionDatas*)mod->datas)->tone_out_params.low = res;
                ((DistortionDatas*)mod->datas)->tone_out->setFrequency(0, res, jack_get_sample_rate(mod->client));
            }
            return ((DistortionDatas*)mod->datas)->tone_out_params.low;
        }};
    table["Out Highcut"] = Module::Slot{"Out Highcut", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                float res = sfx::mapfm_freq(val);
                ((DistortionDatas*)mod->datas)->tone_out_params.high = res;
                ((DistortionDatas*)mod->datas)->tone_out->setFrequency(1, res, jack_get_sample_rate(mod->client));
            }
            return ((DistortionDatas*)mod->datas)->tone_out_params.high;
        }};
    table["Out Lowgain"] = Module::Slot{"Out Lowgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_out_params.gains[0] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_out_params.gains[0];
        }};
    table["Out Midgain"] = Module::Slot{"Out Midgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_out_params.gains[1] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_out_params.gains[1];
        }};
    table["Out Highgain"] = Module::Slot{"Out Highgain", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
                ((DistortionDatas*)mod->datas)->tone_out_params.gains[2] = sfx::mapfm_db(val, -30, 30);
            return ((DistortionDatas*)mod->datas)->tone_out_params.gains[2];
        }};
    
    return table;
}

/*
 * fonction donnant le facteur de reduction de la dérivée en fonction de la distance au max
 * il faut une fonction tendant vers 0+ plus on se raproche de 1-
 * et 0- plus on se raproche de 1+
 * et qui vaut 1 quand on est au seuil
 */
float gradiant(float valeur, float seuil, float shoot, float knee)
{
    if (valeur > seuil)
    {
        return knee*(exp((shoot*(valeur-seuil))/(seuil-0.95))-exp(-seuil));
    }
    else return 1;
}

/*
 * ici on va avoir besoin de la valeur courante de l'entrée, et la valeur précédante : in, prev
 * la sortie sera toujours notée out
 * La fonction du gradiant ne garantissant plus de rattraper correctement la courbe
 * on rajoute une condition disant que si le signal repasse sous le signal saturé, on arrete
 * de modifier le signal
 */
 float clip_gradiant(float in, float seuil, float gain
 , float shoot, float knee
 , float& prev_out)
 {
    float out = prev_out;
    in *= gain;
    
    if (in > 0)
    {
        if (in < seuil) out = in;
        else
        {
            float alpha = gradiant(prev_out, seuil, shoot, knee);
            out += alpha;
            
            // On verifie si il faut rattraper la courbe normale
            if (out > in) out = in;
            if (out > 1) out = 1;
        }
    }
    else 
    {
        if (in < -seuil) out = in;
        else
        {
            float alpha = gradiant(-prev_out, seuil, shoot, knee);
            out -= alpha;
            
            // On verifie si il faut rattraper la courbe normale
            if (out < in) out = in;
            if (out < -1) out = -1;
        }
    }
    
    prev_out = out;
    
    return out;
 }

//#ifdef __ARCH_LINUX__
extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    Module* mod = (Module*) arg;
    DistortionDatas* disto = (DistortionDatas*) mod->datas;
    
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
    
    void* midi_throught = jack_port_get_buffer(mod->midiOuts[0].port, nframes);
    jack_midi_clear_buffer(midi_throught);
    
    jack_midi_event_get(&in_event, midi_port, 0);
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        if (in_event.time == i && event_index < event_count)
        {
            sfx::Midi_reserve_MidiThroughMessage(midi_throught, in_event);
            
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                mod->veryfyAndComputeCCMessage(in_event.buffer);
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }

        out[i] = disto->tone_in->compute(in[i], 3, disto->tone_in_params.gains);
        
        //out[i] *= disto->gain;
        //out[i] = (1 - disto->shape)*tanh( out[i] ) + disto->shape*tanh( out[i] / disto->softness );
        out[i] = clip_gradiant(out[i], disto->seuil, disto->gain, disto->shoot, disto->knee, disto->pout);
        
        out[i] = disto->volume * disto->tone_out->compute(out[i], 3, disto->tone_out_params.gains);
    }
    
    return 0;
}
//#endif