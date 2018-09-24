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
#ifndef __SFX_PI__
#error This File is Specific To Processing Environement
#endif

#include <unistd.h>
#include <chrono>

#include "noyau/global.h"
#include "noyau/log.h"
#include "process/gpio/Footswitch.h"

#include "noyau/modules/ModuleBase.h"

#include "noyau/cmd/CommandListener.h"
#include "noyau/cmd/CommandRegistry.h"

#include "noyau/cmd/list/Module_Cmd.h"

/*
#include "modules/TapTempo.h"
//#include "modules/gain/Distortion.h"
#include "modules/midi/Polysynth.h"
#include "modules/temps/Delay.h"

#include "modules/temps/Chorus.h"
#include "modules/modulation/LFO.h"
 */

#define NAME "SFX-Pi"

bool sfx::Global_NeedToExit = false;

int main(int argc, char** argv)
{
    sfx::sfxStream << sfx::LogModifier(sfx::ModifierCode::RESET) << '\n' << 
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
    
    ///////////////////////////////////////////////////////////////
    // Register all Commands
    ///////////////////////////////////////////////////////////////
    
    registerModulesCommands();
    
    ///////////////////////////////////////////////////////////////
    
    logCommandTable();
    
    std::unique_ptr<CommandListener> COMMAND_LISTENER(new CommandListener());
    
    ///////////////////////////////////////////////////////////////
    // Build GPIO Handler
    ///////////////////////////////////////////////////////////////
    
    std::unique_ptr<GPIOJackClient> GPIO(new GPIOJackClient({mcp23017::HEX_MCP_0, mcp23017::HEX_MCP_1}));
    
    ///////////////////////////////////////////////////////////////
    // Build System Jack Unit
    ///////////////////////////////////////////////////////////////
    if (Module::loadModule("modules/system/SFX_System.so"))
    {
        sfx::fatal(NAME, "Failed Load SFX-System Module\n");
        exit(1);
    }
    
    if (EffectUnit::buildEffectUnitFromModule("sfx_system"))
    {
        sfx::fatal(NAME, "Failed Build SFX-System Unit\n");
        exit(1);
    }
    
    ///////////////////////////////////////////////////////////////
    // Load Module Table
    ///////////////////////////////////////////////////////////////
    
    if (Module::loadModuleTable("config/module_table"))
    {
        sfx::fatal(NAME, "Failed Load Module Table");
        exit(1);
    }
    
    Module::logLoadedModuleTable();
    
    sfx::sfxStream << '\n' <<
    "====================================================================================================="
    << '\n' <<
    "   Progam Setup Is OK : Let's get Started Mudda Fukkazz                                              "
    << '\n' <<
    "====================================================================================================="
    << '\n' << '\n';
    
    while(!sfx::Global_NeedToExit)
    {
        std::string terminal_input = COMMAND_LISTENER->getBuffer();
        performCommandHandling(terminal_input);
        COMMAND_LISTENER->clearBuffer();
        
        usleep(10000);
    }
    
    sfx::sfxStream << '\n' <<
    "====================================================================================================="
    << '\n' <<
    "   Progam End : Unload Environement and Release Ressources                                           "
    << '\n' <<
    "====================================================================================================="
    << '\n' << '\n';
    
    Module::unloadModuleTable();
    
    sfx::sfxStream << '\n' <<
    "====================================================================================================="
    << '\n' <<
    "   Progam End : GoodBye                                                                              "
    << '\n' <<
    "====================================================================================================="
    << '\n' << '\n';
    
    return 0;
}

