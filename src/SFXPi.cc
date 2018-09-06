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
 * File:   SFX-Pi.cc
 * Author: Space-Computer
 *
 * Created on 23 juillet 2018, 15:00
 */
#include <unistd.h>
#include <chrono>

#include "noyau/log.h"
#include "process/gpio/Footswitch.h"

#include "noyau/modules/ModuleLoader.h"

/*
#include "modules/TapTempo.h"
//#include "modules/gain/Distortion.h"
#include "modules/midi/Polysynth.h"
#include "modules/temps/Delay.h"

#include "modules/temps/Chorus.h"
#include "modules/modulation/LFO.h"
 */

#define NAME "SFX-Pi"

int main(int argc, char** argv)
{
    sfx::sfxStream << '\n' << 
    "====================================================================================================="
    << '\n' <<
    "=====================================================================================================\n"
    "         _________    _________ _          _  _                 _  ___                           \n"
    "        // ___       // ______   \\\\      _/ /                  // /__ \\       ###                \n"
    "       // /         // /          \\\\   _/ /                   //     \\ |    #    #      _/ /     \n"
    "      // /_____    // /_           \\\\_/ /      _ ______      // ___ / /      ###      _/ /    /  \n"
    "     //_____  /   // _____        _/\\\\/       / /_____/_/   // ____ -     _   _     _/ /___/ /_  \n"
    "          // /   // /           _/ / \\\\                    // /           // /     /______  _    \n"
    "      ___// /   // /          _/ /    \\\\                  // /           // /           // /     \n"
    "  _________/  _// /_        _/ /_      \\\\_              _// /_         _// /_         _// /_     \n"
    "                                                                                                 \n"
    "=====================================================================================================\n"
    << '\n' <<
    "   SFX-Pi : The Embedded System"
    << '\n' << 
    "   Program Version : " << sfx::VERSION
    << '\n' <<
    "      Build : " << sfx::BUILD
    << '\n' <<
    #ifndef NDEBUG
    "      DEBUG BUILD : Use \'make RELEASE=1\' to build a release"
    << '\n' <<
    #endif
    "====================================================================================================="
    << '\n' << 
    "====================================================================================================="
    << '\n' << '\n';
    
    std::unique_ptr<GPIOJackClient> GPIO(new GPIOJackClient({mcp23017::HEX_MCP_0, mcp23017::HEX_MCP_1}));
    
    std::shared_ptr<Module> DISTO_MODULE = loadModule("modules/gain/Distortion.so");
    if (!DISTO_MODULE)
    {
        exit(1);
    }
    
    std::shared_ptr<Module> SYNTH_MODULE = loadModule("modules/midi/Polysynth.so");
    if (!SYNTH_MODULE)
    {
        exit(1);
    }
    
    ///////////////////////////////////////////////////////////////
    
	const char **physic_in;
    physic_in = jack_get_ports (GPIO->client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (physic_in == NULL) 
    {
		sfx::err(NAME, "no physical capture ports\n");
	}

	const char **physic_out;
	physic_out = jack_get_ports (GPIO->client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (physic_out == NULL) 
    {
		sfx::err(NAME, "no physical playback ports\n");
	}
    
    const char **midi_in;
    midi_in = jack_get_ports(GPIO->client, NULL, JACK_DEFAULT_MIDI_TYPE,
            JackPortIsPhysical|JackPortIsOutput);
    if ( midi_in == NULL ){
        sfx::err(NAME, "No Physical MIDI Capture Ports");
    }
    
    ///////////////////////////////////////////////////////////////
    
    std::unique_ptr<EffectUnit> DISTO(new EffectUnit(DISTO_MODULE));
    
    DISTO->linkSlot(std::make_pair(12, 10), "in_lowcut");
    DISTO->linkSlot(std::make_pair(13, 10), "in_highcut");
    DISTO->linkSlot(std::make_pair(14, 10), "out_lowcut");
    DISTO->linkSlot(std::make_pair(15, 10), "out_highcut");
    
    ///////////////////////////////////////////////////////////////
    
    std::unique_ptr<EffectUnit> SYNTH(new EffectUnit(SYNTH_MODULE));
    
    SYNTH->linkSlot(std::make_pair(8, 10), "a");
    SYNTH->linkSlot(std::make_pair(9, 10), "d");
    SYNTH->linkSlot(std::make_pair(10, 10), "r");
    SYNTH->linkSlot(std::make_pair(11, 10), "volume");
    
    ///////////////////////////////////////////////////////////////
    
	if (jack_connect (DISTO->client, physic_in[0], jack_port_name (DISTO->audioIns[0].port))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (DISTO->client, jack_port_name (DISTO->audioOuts[0].port), physic_out[0])) 
    {
		sfx::err (NAME, "cannot connect output ports : \"%s\" \"%s\"\n", jack_port_name (DISTO->audioOuts[0].port), physic_out[0]);
	}
	if (jack_connect (DISTO->client, jack_port_name (DISTO->audioOuts[0].port), physic_out[1])) 
    {
		sfx::err (NAME, "cannot connect output ports : \"%s\" \"%s\"\n", jack_port_name (DISTO->audioOuts[0].port), physic_out[1]);
	}
    
	if (jack_connect (DISTO->client, midi_in[0], jack_port_name (DISTO->midiIns[0].port))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    ///////////////////////////////////////////////////////////////
    
	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audioOuts[0].port), physic_out[0])) 
    {
		sfx::err (NAME, "cannot connect output ports : \"%s\" \"%s\"\n", jack_port_name (SYNTH->audioOuts[0].port), physic_out[0]);
	}
	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audioOuts[0].port), physic_out[1])) 
    {
		sfx::err (NAME, "cannot connect output ports : \"%s\" \"%s\"\n", jack_port_name (SYNTH->audioOuts[0].port), physic_out[1]);
	}
    
	if (jack_connect (SYNTH->client, jack_port_name (DISTO->midiOuts[0].port), jack_port_name (SYNTH->midiIns[0].port))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	free (physic_in);
	free (physic_out);
    free (midi_in);
    
    while(1);
    
    unloadModule(DISTO_MODULE);
    
    /*
    std::unique_ptr<ModuleTapTempo> TTP(new ModuleTapTempo());
    
    //std::unique_ptr<ModuleDistortion> DIST(new ModuleDistortion());
    std::unique_ptr<ModulePolysynth> SYNTH(new ModulePolysynth());
    
    std::unique_ptr<ModuleChorus> CHORUS_L(new ModuleChorus());
    std::unique_ptr<ModuleChorus> CHORUS_R(new ModuleChorus());
    std::unique_ptr<ModuleChorus> CHORUS_SL(new ModuleChorus());
    std::unique_ptr<ModuleChorus> CHORUS_SR(new ModuleChorus());
    
    std::unique_ptr<ModuleLFO> LFO(new ModuleLFO());
    
    std::unique_ptr<ModuleDelay> DELAY_L(new ModuleDelay());
    std::unique_ptr<ModuleDelay> DELAY_R(new ModuleDelay());
    std::unique_ptr<ModuleDelay> DELAY_SL(new ModuleDelay());
    std::unique_ptr<ModuleDelay> DELAY_SR(new ModuleDelay());
    
    CHORUS_L->setBank1();
    CHORUS_R->setBank2();
    CHORUS_SL->setBank3();
    CHORUS_SR->setBank4();
    
    DELAY_L->setBank1();
    DELAY_R->setBank2();
    DELAY_SL->setBank3();
    DELAY_SR->setBank4();
    
	if (jack_connect (GPIO->client, jack_port_name(GPIO->logic_out), jack_port_name(TTP->midi_in))) 
    {
		sfx::err (NAME, "cannot connect ports\n");
	}
	if (jack_connect (TTP->client, jack_port_name(TTP->midi_out), jack_port_name(GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect ports\n");
	}
    //*/
    
    ///////////////////////////////////////////////////////////////
    /*
	const char **physic_in;
    physic_in = jack_get_ports (DIST->client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (physic_in == NULL) 
    {
		sfx::err(NAME, "no physical capture ports\n");
	}

	const char **physic_out;
	physic_out = jack_get_ports (DIST->client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (physic_out == NULL) 
    {
		sfx::err(NAME, "no physical playback ports\n");
	}
    
    const char **midi_in;
    midi_in = jack_get_ports(SYNTH->client, NULL, JACK_DEFAULT_MIDI_TYPE,
            JackPortIsPhysical|JackPortIsOutput);
    if ( midi_in == NULL ){
        sfx::err(NAME, "No Physical MIDI Capture Ports");
    }
    
    ///////////////////////////////////////////////////////////////

	if (jack_connect (DIST->client, physic_in[0], jack_port_name (DIST->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    
    
	if (jack_connect (DIST->client, jack_port_name (DIST->audio_out), jack_port_name (CHORUS_L->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (DIST->client, jack_port_name (DIST->audio_out), jack_port_name (CHORUS_R->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}

    
	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audio_out), jack_port_name (DELAY_L->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audio_out), jack_port_name (DELAY_R->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (LFO->client, jack_port_name (LFO->audio_out), jack_port_name (CHORUS_L->mod_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (LFO->client, jack_port_name (LFO->audio_out), jack_port_name (CHORUS_R->mod_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    
    
	if (jack_connect (CHORUS_L->client, jack_port_name (CHORUS_L->audio_out), jack_port_name (DELAY_L->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (CHORUS_R->client, jack_port_name (CHORUS_R->audio_out), jack_port_name (DELAY_R->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    

	if (jack_connect (DELAY_L->client, jack_port_name (DELAY_L->audio_out), physic_out[0])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
	if (jack_connect (DELAY_R->client, jack_port_name (DELAY_R->audio_out), physic_out[1])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
    
    
    ///////////////////////////////////////////////////////////////
    
	if (jack_connect (LFO->client, jack_port_name (LFO->audio_out), jack_port_name (CHORUS_SL->mod_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (LFO->client, jack_port_name (LFO->audio_out), jack_port_name (CHORUS_SR->mod_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    
    
	if (jack_connect (CHORUS_SL->client, jack_port_name (CHORUS_SL->audio_out), jack_port_name (DELAY_SL->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
	if (jack_connect (CHORUS_SR->client, jack_port_name (CHORUS_SR->audio_out), jack_port_name (DELAY_SR->audio_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    

	if (jack_connect (DELAY_SL->client, jack_port_name (DELAY_SL->audio_out), physic_out[2])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
	if (jack_connect (DELAY_SR->client, jack_port_name (DELAY_SR->audio_out), physic_out[3])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
    
    ///////////////////////////////////////////////////////////////

	if (jack_connect (SYNTH->client, midi_in[0], jack_port_name (SYNTH->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}

	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audio_out), physic_out[0])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
	if (jack_connect (SYNTH->client, jack_port_name (SYNTH->audio_out), physic_out[1])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
    
    ///////////////////////////////////////////////////////////////
    
	if (jack_connect (DIST->client, jack_port_name (DIST->midi_out), jack_port_name (GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (DELAY_L->client, jack_port_name (DELAY_L->midi_out), jack_port_name (GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (DELAY_R->client, jack_port_name (DELAY_R->midi_out), jack_port_name (GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (DELAY_SL->client, jack_port_name (DELAY_SL->midi_out), jack_port_name (GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (DELAY_SR->client, jack_port_name (DELAY_SR->midi_out), jack_port_name (GPIO->logic_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    
	if (jack_connect (GPIO->client, jack_port_name (GPIO->logic_out), jack_port_name (DIST->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (GPIO->client, jack_port_name (GPIO->logic_out), jack_port_name (DELAY_L->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (GPIO->client, jack_port_name (GPIO->logic_out), jack_port_name (DELAY_R->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (GPIO->client, jack_port_name (GPIO->logic_out), jack_port_name (DELAY_SL->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
	if (jack_connect (GPIO->client, jack_port_name (GPIO->logic_out), jack_port_name (DELAY_SR->midi_in))) 
    {
		sfx::err (NAME, "cannot connect input ports\n");
	}
    
    ///////////////////////////////////////////////////////////////

	if (jack_connect (DIST->client, physic_in[1], physic_out[0])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}
	if (jack_connect (DIST->client, physic_in[1], physic_out[1])) 
    {
		sfx::err (NAME, "cannot connect output ports\n");
	}

	free (physic_in);
	free (physic_out);
    free (midi_in);
    */
    //while(1);// UpdateMCP(regs, status, ftable, ltable);
    
    /*
    sfx::log(NAME, "Test Temps de lecture GPIO\n");
    
    std::clock_t c_start = std::clock();
    
    size_t n_test = 100;
    for (size_t i = 0; i < n_test; i++)
        UpdateMCP(regs, status);
    
    std::clock_t c_end = std::clock();
    
    double c_total = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
    
    sfx::log(NAME, "Temps de lecture total : %.2fms, Temps de lecture Moyen : %.2fms\n", c_total, c_total/n_test);
    //*/
    
    sfx::sfxStream << '\n' <<
    "====================================================================================================="
    << '\n' <<
    "   Progam End : GoodBye                                                                          "
    << '\n' <<
    "====================================================================================================="
    << '\n' << '\n';
    
    return 0;
}

