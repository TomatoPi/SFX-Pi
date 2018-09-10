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
 * File:   ModuleLoader.cc
 * Author: Space-Computer
 * 
 * Created on 30 août 2018, 22:21
 */

#include "ModuleLoader.h"

#define NAME "Module-Loader"

////////////////////////////////////////////////////////////////////
// Module Handling structures
////////////////////////////////////////////////////////////////////
namespace
{
    LoadedModulesTable ModuleTable;
}

LoadedModulesTable& getModuleTable()
{
    return ModuleTable;
}

/**
 * @brief Fonction à appeler au lancement du programme pour charger les modules du fichier
 *  de configuration donné
 * @param config_path chemin du fichier de configuration
 * @return 0 on success
 */
int loadModuleTable(std::string config_path)
{
    std::ifstream config_file(config_path.c_str());

    if (config_file.is_open())
    {
        std::string module_path;
        while (getline(config_file, module_path, '\n'))
        {
            loadModule(module_path);
        }
        config_file.close();

        return ModuleTable.size() == 0; // Return 0 if at least one module has been loaded
    }
    else return 1;
}

void unloadModuleTable()
{
    for (auto& module : ModuleTable) unloadModule(module.first);
}

/**
 * Fonction pour afficher la liste des modules chargés
 */
void logLoadedModuleTable()
{
    sfx::log(NAME, "Loaded Modules :\n");
    sfx::sfxStream << sfx::formatString("%-30s | %-30s | %-30s | %-7s\n",
            std::string("Nom Unique"), std::string("Nom d'Affichage"), std::string("Version"))
            << "-------------------------------+--------------------------------+--------------------------------+--------\n";
    for (auto& module : ModuleTable)
    {
        const Module::ShortInfo& infos = module.second->infos;
        sfx::sfxStream << sfx::formatString("%-30s | %-30s | %-30s | %-7s\n",
                module.first, infos.unique_name, infos.name, infos.version);
    }
    sfx::sfxStream
            << "-------------------------------+--------------------------------+--------------------------------+--------\n";
}

////////////////////////////////////////////////////////////////////
// ModuleLoading Structures
////////////////////////////////////////////////////////////////////

/**
 * @brief Fonction utilisée pour charger un Module
 * @param path Chemin d'accès au module à charger
 * @return Pointeur vers le module chargé ou nullptr si le chargement a échoué
 */
int loadModule(std::string path)
{
    sfx::log(NAME, "Chargement du Module : \"%s\" ... ", path);

#if defined(__UNIX__)

    try
    {

        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle)
            throw std::ios_base::failure("Echec Ouverture Librairie : " + std::string(dlerror()));

        dlerror();

        // Chargement des Infos du Module
        Module::register_module_info_f infos_load_fptr = (Module::register_module_info_f)load_function(handle, "function_register_module_info");
        Module::ShortInfo loaded_infos = (*infos_load_fptr)();

        // Chargement de la fonction Builder
#ifdef __ARCH_LINUX__
        Module::create_module_data_f builder_fptr = (Module::create_module_data_f)load_function(handle, "function_create_jack_module");
#else
        Module::create_module_data_f builder_fptr = (Module::create_module_data_f)load_function(handle, "function_create_non_jack_module");
#endif

        // Chargement de la fonction Destructor
        Module::destroy_module_data_f destructor_fptr = (Module::destroy_module_data_f)load_function(handle, "function_destroy_module");

        // Chargement de la Table des Slots
        Module::register_slot_table_f slots_load_fptr = (Module::register_slot_table_f)load_function(handle, "function_register_module_slots");
        Module::SlotTable loaded_slot_table = (*slots_load_fptr)();

#ifdef __ARCH_LINUX__
        JackProcessCallback process_callback_fptr = (JackProcessCallback) load_function(handle, "function_process_callback");
#endif

        sfx::sfxStream << "Succès du Chargement \n";
        std::shared_ptr<Module> loaded_module = std::shared_ptr<Module>(new Module{
            .infos = loaded_infos,
            .builder = builder_fptr,
            .destructor = destructor_fptr,
            .slots = loaded_slot_table,
#ifdef __ARCH_LINUX__
            .callback = process_callback_fptr,
#endif
            .lib_handle = handle
        });

        if (ModuleTable.find(loaded_infos.unique_name) != ModuleTable.end())
        {
            sfx::wrn(NAME, "Module : \"%s\" is Already loaded ...\n", loaded_infos.unique_name);
            return 1;
        }

        ModuleTable[loaded_infos.unique_name] = loaded_module;

        return 0;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Echec : %s\n", e.what());
        return 1;
    }
    catch (std::bad_alloc const& e)
    {
        sfx::err(NAME, "Echec : %s\n", e.what());
        return 1;
    }

#elif defined(__WINDOWS__)
#endif
}

/**
 * @brief Fonction à appeller pour décharger un Module
 * @param module module à décharger
 */
void unloadModule(std::string unique_name)
{
    auto module = ModuleTable[unique_name];

#if defined(__UNIX__)

    dlclose(module->lib_handle);

#elif defined(__WINDOWS__)
#endif

    ModuleTable.erase(unique_name);
}

/**
 * @brief Fonction de wrapping pour charger une fonction depuis une librairie dynamique
 * @param name Nom de la fonction à charger
 * @return pointeur vers la fonction chargée
 * @throw ios_base::failure
 */
void* load_function(void* handle, std::string name)
{
    void* function_fptr = dlsym(handle, name.c_str());
    if (!function_fptr)
        throw std::ios_base::failure(dlerror());
    return function_fptr;
}