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

/**
 * @brief Fonction utilisée pour charger un Module
 * @param path Chemin d'accès au module à charger
 * @return Pointeur vers le module chargé ou nullptr si le chargement a échoué
 */
std::shared_ptr<Module::Info> loadModule(std::string path)
{
    sfx::log(NAME, "Chargement du Module : \"%s\" ... ", path);
    
#if defined(__UNIX__)
    
    try {

        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) 
            throw std::ios_base::failure("Echec Ouverture Librairie : "+std::string(dlerror()));

        dlerror();
        
        // Chargement des Infos du Module
        Module::register_module_info_f infos_load_fptr = (Module::register_module_info_f)load_function(handle, "function_register_module_info");
        Module::ShortInfo loaded_infos = (*infos_load_fptr)();
        
        // Chargement de la fonction Builder
#ifdef __ARCH_LINUX__
        Module::create_module_f builder_fptr = (Module::create_module_f)load_function(handle, "function_create_jack_module");
#else
        Module::create_module_f builder_fptr = (Module::create_module_f)load_function(handle, "function_create_non_jack_module");
#endif
        
        // Chargement de la fonction Destructor
        Module::destroy_module_f destructor_fptr = (Module::destroy_module_f)load_function(handle, "function_destroy_module");
        
        // Chargement de la Table des Slots
        Module::register_slot_table_f slots_load_fptr = (Module::register_slot_table_f)load_function(handle, "function_register_module_slots");
        Module::SlotTable loaded_slot_table = (*slots_load_fptr)();
        
#ifdef __ARCH_LINUX__
        JackProcessCallback process_callback_fptr = (JackProcessCallback)load_function(handle, "function_process_callback");
#endif
        
        sfx::sfxStream << "Succès du Chargement \n";
        return std::make_unique<Module::Info>(Module::Info{
            loaded_infos
                , builder_fptr, destructor_fptr
                , loaded_slot_table
#ifdef __ARCH_LINUX__
                , process_callback_fptr
#endif
                , handle});
    }
    catch(std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Echec : %s\n", e.what());
        return nullptr;
    }
    
#elif defined(__WINDOWS__)
#endif
}

/**
 * @brief Fonction à appeller pour décharger un Module
 * @param module module à décharger
 */
void unloadModule(std::shared_ptr<Module::Info> module)
{
#if defined(__UNIX__)
    
    dlclose(module->lib_handle);
    
#elif defined(__WINDOWS__)
#endif
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