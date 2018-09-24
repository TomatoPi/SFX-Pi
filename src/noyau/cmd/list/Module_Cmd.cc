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
 * File:   Module_Cmd.cc
 * Author: Space-Computer
 * 
 * Created on 15 septembre 2018, 18:14
 */

#include <vector>

#include "Module_Cmd.h"

#define NAME ("Module-CMD")

void registerModulesCommands()
{
    ////////////////////////////////////////////////////////////////////
    // Modules Managment Commands
    ////////////////////////////////////////////////////////////////////
    
    registerStrictArgCommand("load-module",
            "Load a Module",
            "load-module [file_path] : Load the module at given path",
            1,
            [](std::vector<std::string> args)->void
            {
                if (Module::loadModule(args[0]))
                    sfx::wrn(NAME, "Please Verify if module file exist ...\n");
            });
    registerStrictArgCommand("unload-module",
            "Unload a Module",
            "unload-module [internal_name] : Unload the module with given internal name",
            1,
            [](std::vector<std::string> args)->void
            {
                if (Module::unloadModule(args[0]))
                    sfx::wrn(NAME, "Use \"loaded-modules\" to view list of loaded modules ...\n");
            });
    registerStrictArgCommand("loaded-modules",
            "Print Module Table",
            "loaded-modules : Print the table of curently loaded modules",
            0,
            [](std::vector<std::string> args)->void
            {
                Module::logLoadedModuleTable();
            });
            
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("info-module",
            "Print Module Infos",
            "view-module [internal_name] : Print the Module's complete Informations",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto module = Module::getModule(args[0]).lock())
                    module->logModuleCompleteInfos();
            });
            
    ////////////////////////////////////////////////////////////////////
    // EffectUnits Managment Commands
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("build-unit",
            "Create Default Unit",
            "build-unit [module_name] : Create an effect unit from given module",
            1,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::buildEffectUnitFromModule(args[0]))
                    sfx::wrn(NAME, "Use \"loaded-modules\" to view list of loaded modules ...\n");
            });
    registerStrictArgCommand("load-unit",
            "Load Unit Preset",
            "load-unit [file_path] : Create an effect unit from given preset file",
            1,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::buildEffectUnitFromPresetFile(args[0]))
                    sfx::wrn(NAME, "Please Verify if preset file exist ...\n");
            });
    registerVarArgCommand("save-unit",
            "Save Unit Preset",
            "save-unit [internal_name] [file_path] {banks_file} {links_file} : Save given Unit at the provided location",
            2, 4,
            [](std::vector<std::string> args)->void
            {
                while (args.size() < 4) args.push_back("");
                if (EffectUnit::saveEffectUnit(args[0], args[1], args[2], args[3]))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("delete-unit",
            "Delete Effect Unit",
            "delete-unit [internal_name] : Delete given effect Unit",
            1,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::destroyEffectUnit(args[0]))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("save-banks",
            "Save Unit Banks",
            "save-banks [internal_name] [file_path] : Save given Unit's Banks at the provided location",
            2,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::save_BankFile(args[1], EffectUnit::getEffectUnit(args[0])))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("save-links",
            "Save Unit Links",
            "save-banks [internal_name] [file_path] : Save given Unit's Links at the provided location",
            2,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::save_LinkFile(args[1], EffectUnit::getEffectUnit(args[0])))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    registerStrictArgCommand("load-banks",
            "Load Unit Banks",
            "load-banks [internal_name] [file_path] : Load given Unit's Banks at the provided location",
            2,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::load_BankFile(args[1], EffectUnit::getEffectUnit(args[0])))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("load-links",
            "Load Unit Links",
            "load-banks [internal_name] [file_path] : Load given Unit's Links at the provided location",
            2,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::load_LinkFile(args[1], EffectUnit::getEffectUnit(args[0])))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("link-slot",
            "Link a slot to a cc",
            "link-slot [internal_name] [slot] [cc_chanel] [cc_source] : Link given cc to unit's slot",
            4,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    if (unit->linkSlot(std::make_pair<sfx::hex_t, sfx::hex_t>(std::stoi(args[2]), std::stoi(args[3])),
                            args[1]))
                        sfx::wrn(NAME, "Use \"info-module\" to view slot table of this unit\n");
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("unlink-slot",
            "Unlink a slot to a cc",
            "unlink-slot [internal_name] [slot] [cc_chanel] [cc_source] : Unlink given cc from unit's slot",
            4,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    if (unit->unlinkSlot(std::make_pair<sfx::hex_t, sfx::hex_t>(std::stoi(args[2]), std::stoi(args[3])),
                            args[1]))
                        sfx::wrn(NAME, "Use \"info-module\" to view slot table of this unit\n");
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("clear-slots",
            "UnLink all unit's slots",
            "clear-slots [internal_name] : Remove all links from CC to Slots",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    unit->clearLinks();
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("loaded-units",
            "Show list of loaded Units",
            "loaded-units : Show list of curently loaded units",
            0,
            [](std::vector<std::string> args)->void
            {
                EffectUnit::logEffectUnitsTable();
            });
    registerStrictArgCommand("info-unit",
            "Show Informations of a loaded Unit",
            "info-unit [unique_name] : Show Informations of a loaded Unit",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                    unit->logCompleteInfos();
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
    // Connection Managment Commands
    ////////////////////////////////////////////////////////////////////
            
    registerVarArgCommand("connect",
            "Connect two ports together",
            "connect [source_name] [source_port] [target_name] [target_port] : Connect two effects units together, this version garantee that both units and ports exist\nconnect [unit_name] [source_complete_name] [target_complete_name] : Connect the ports together, use this version when one or both port doesn't belongs to an effect unit",
            3, 4,
            [](std::vector<std::string> args)->void
            {
                if (args.size() == 3)
                {
                    if (EffectUnit::connect(args[0], args[1], args[2]))
                        sfx::wrn(NAME, "Failed connect Ports, prefer use the 4 arguments version of this command\n");
                }
                else
                {
                    if (EffectUnit::connect(args[0], args[1], args[2], args[3]))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
                }
            });
    registerVarArgCommand("disconnect",
            "Disconnect two ports together",
            "disconnect [source_name] [source_port] [target_name] [target_port] : Disconnect two effects units together, this version garantee that both units and ports exist\ndisconnect [unit_name] [source_complete_name] [target_complete_name] : Disconnect the ports together, use this version when one or both port doesn't belongs to an effect unit",
            3, 4,
            [](std::vector<std::string> args)->void
            {
                if (args.size() == 3)
                {
                    if (EffectUnit::connect(args[0], args[1], args[2]))
                        sfx::wrn(NAME, "Failed disconnect Ports, prefer use the 4 arguments version of this command\n");
                }
                else
                {
                    if (EffectUnit::connect(args[0], args[1], args[2], args[3]))
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
                }
            });
    registerStrictArgCommand("graph",
            "Show Connection Graph",
            "graph : Print the list of connection made",
            0,
            [](std::vector<std::string> args)->void
            {
                EffectUnit::logConnectionGraph();
            });
            
    ////////////////////////////////////////////////////////////////////
    // Banks Managment Commands
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("set-bank",
            "Change Unit's current Bank",
            "set-bank [unit_name] [bank_id] : Change unit's Current Bank",
            2,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    if (unit->setBank(std::stoi(args[1])))
                        sfx::wrn(NAME, "Use \"info-module\" to view banks table of this unit\n");
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("next-bank",
            "Cycle to Unit's Next Bank",
            "next-bank [unit_name] : Cycle to Unit's Next Bank, and rewind to first bank if end is reached",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                    unit->nextBank();
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("prev-bank",
            "Cycle to Unit's Previous Bank",
            "prev-bank [unit_name] : Cycle to Unit's Previous Bank, and rewind to last bank if begining is reached",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                    unit->prevBank();
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("build-bank",
            "Add a bank to an Unit",
            "build-bank [unit_name] : Build a bank from Module's default values and add it to the unit",
            1,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                    unit->createBank();
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("copy-bank",
            "Dupplicate an Unit's Bank",
            "copy-bank [unit_name] [source_id] : Build a bank from and other one and add it to the unit",
            2,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    if (unit->copyBank(std::stoi(args[1])))
                        sfx::wrn(NAME, "Use \"info-module\" to view banks table of this unit\n");
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
    registerStrictArgCommand("delete-bank",
            "Delete an Unit's Bank",
            "delete-bank [unit_name] [source_id] : Remove a bank from the unit",
            2,
            [](std::vector<std::string> args)->void
            {
                if (auto unit = EffectUnit::getEffectUnit(args[0]).lock())
                {
                    if (unit->removeBank(std::stoi(args[1])))
                        sfx::wrn(NAME, "Use \"info-module\" to view banks table of this unit\n");
                }
                else
                    sfx::wrn(NAME, "Use \"loaded-units\" to view list of loaded effect units ...\n");
            });
            
    ////////////////////////////////////////////////////////////////////
    // Environement Managment Commands
    ////////////////////////////////////////////////////////////////////
            
    registerStrictArgCommand("save-env",
            "Save all Units and Connection Graph",
            "save-env [file_path] : Save all units and Connections to given files",
            1,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::save_EnvironementFile(args[0]))
                    sfx::wrn(NAME, "Failed save Environement\n");
            });
    registerStrictArgCommand("load-env",
            "Load all Units and Connection Graph",
            "load-env [file_path] : Load all units and Connections from given files",
            1,
            [](std::vector<std::string> args)->void
            {
                if (EffectUnit::load_EnvironementFile(args[0]))
                    sfx::wrn(NAME, "Failed load Environement\n");
            });
}