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
 * File:   Module_Module.cc
 * Author: Space-Computer
 * 
 * Created on 30 août 2018, 22:21
 */

#include "ModuleBase.h"

#define NAME "Module-API"

////////////////////////////////////////////////////////////////////
// Informations sur le Module
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Gestion des slots
////////////////////////////////////////////////////////////////////

/**
 * Fonction appelée au chargement du Module pour enregistrer les slots
 *  natifs
 */
void Module::register_common_slots()
{
    register_slot(slots, Slot::TRIGGER, 0, "sfx_next_bank", "Next-Bank",
            [](sfx::hex_t val, EffectUnit* effect)->float
            {
                if (val < 128) effect->nextBank();
                return effect->getCurrentBank();
            });
    register_slot(slots, Slot::TRIGGER, 0, "sfx_prev_bank", "Prev-Bank",
            [](sfx::hex_t val, EffectUnit* effect)->float
            {
                if (val < 128) effect->prevBank();
                return effect->getCurrentBank();
            });
    register_slot(slots, Slot::TRIGGER, 0, "sfx_set_bank", "Set-Bank",
            [](sfx::hex_t val, EffectUnit* effect)->float
            {
                if (val < 128) effect->setBank(val);
                return effect->getCurrentBank();
            });
}
    
////////////////////////////////////////////////////////////////////
// Fonctions de creation et destruction des datas des EffectUnits
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Fonctions specifiques aux Modules sous SFX-Pi
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Gestion de la librairie dynamique
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Gestion des EffectUnits
////////////////////////////////////////////////////////////////////

int Module::addRelatedUnit(std::string unit_unique_name)
{
    if (relatedUnits.find(unit_unique_name) != relatedUnits.end())
        return 1;
    
    relatedUnits.insert(unit_unique_name);
    return 0;
}

void Module::removeRelatedUnit(std::string unit_unique_name)
{
    relatedUnits.erase(unit_unique_name);
}

////////////////////////////////////////////////////////////////////
// Constructeur et Destructeur
////////////////////////////////////////////////////////////////////

Module::Module(const ShortInfo& infos,
        const SlotTable& slots,

        const create_module_data_f& builder,
        const destroy_module_data_f& destructor,
#ifdef __SFX_PI__
        const JackProcessCallback& callback,
#endif
        void* lib_handle) :
infos(infos),
slots(slots),
builder(builder),
destructor(destructor),
#ifdef __SFX_PI__
callback(callback),
#endif
lib_handle(lib_handle),
relatedUnits()
{
    this->register_common_slots();
}

Module::~Module()
{
}

////////////////////////////////////////////////////////////////////
// Fonctions d'info
////////////////////////////////////////////////////////////////////

void Module::logModuleCompleteInfos() const
{
    sfx::log(NAME, "Module Full Informations : \n");
    sfx::sfxStream
            << sfx::formatString(" Unique_Name : \"%s\"\n Name:\"%s\"\n Version:\"%s\"\n %s\n"
            , infos.unique_name, infos.name, infos.version
            , std::string((infos.type==ShortInfo::MODULE)?"Regular Module":"System Module"))
            << "--------------------+--------------------+--------------------+--------------------\n"
            << sfx::formatString("%-20s|%-20s|%-20s|%-20s\n"
            , std::string("Clef"), std::string("Nom Interne")
            , std::string("Nom d'Affichage"), std::string("Valeur par défault"))
            << "====================|====================|====================|====================\n";

    for (auto& slot : slots)
    {
        sfx::sfxStream << sfx::formatString("%-20s|%-20s|%-20s|%3i\n"
                , slot.first, slot.second->internal_name, slot.second->display_name, slot.second->default_value);
    }
    sfx::sfxStream << "--------------------+--------------------+--------------------+--------------------\n";
}

////////////////////////////////////////////////////////////////////
// ModuleLoading Structures
////////////////////////////////////////////////////////////////////
Module::LoadedModulesTable Module::ModuleTable = Module::LoadedModulesTable();

const Module::LoadedModulesTable& Module::getModuleTable()
{
    return Module::ModuleTable;
}

std::weak_ptr<Module> Module::getModule(const std::string& unique_name)
{
    if (ModuleTable.find(unique_name) == ModuleTable.end())
    {
        sfx::wrn(NAME, "Module Doesn't Exist : \"%s\"\n", unique_name);;
        return std::weak_ptr<Module>();
    }
    
    return ModuleTable[unique_name];
}

/**
 * @brief Fonction à appeler au lancement du programme pour charger les modules du fichier
 *  de configuration donné
 * @param config_path chemin du fichier de configuration
 * @return 0 on success
 */
int Module::loadModuleTable(std::string config_path)
{
    std::ifstream config_file(config_path.c_str());

    if (config_file.is_open())
    {
        std::string module_path;
        while (getline(config_file, module_path, '\n'))
        {
            try
            {
                loadModule(module_path);
            }
            catch (std::runtime_error const& e)
            {
                sfx::err(NAME, "Error : %s\n", e.what());
            }
        }
        config_file.close();

        return ModuleTable.size() == 0; // Return 0 if at least one module has been loaded
    }
    else return 1;
}

void Module::unloadModuleTable()
{
    while (ModuleTable.size() != 0)
    {
        logLoadedModuleTable();
        forceUnloadModule(ModuleTable.begin()->first);
    }
}

/**
 * Fonction pour afficher la liste des modules chargés
 */
void Module::logLoadedModuleTable()
{
    sfx::log(NAME, "Loaded Modules :\n");
    sfx::sfxStream 
            << "-----------+--------------------------------++--------------------------------+--------------------------------+---------\n"
            << sfx::formatString("%-10s | %-30s || %-30s | %-30s | %-7s\n",
            std::string("Type"), std::string("Clef"),
            std::string("Nom Unique"), std::string("Nom d'Affichage"), std::string("Version"))
            << "===========|================================||================================|================================|=========\n";
    for (auto& module : ModuleTable)
    {
        const Module::ShortInfo& infos = module.second->infos;
        sfx::sfxStream << sfx::formatString("%-10s | %-30s || %-30s | %-30s | %-7s\n",
                std::string((infos.type==ShortInfo::MODULE)?"Module":"System"), module.first,
                infos.unique_name, infos.name, infos.version);
    }
    sfx::sfxStream
            << "-----------+--------------------------------++--------------------------------+--------------------------------+---------\n";
}

/**
 * Fonction à appeller pour charger un module depuis un fichier de librairie dynamique
 * @param path chemin du fichier librairie
 * @return 0 on success
 */
int Module::loadModule(std::string path)
{
    sfx::log(NAME, "Chargement du Module : \"%s\" ... \n", path);

#if defined(__UNIX__)

    try
    {

        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle)
            throw std::ios_base::failure("Echec Ouverture Librairie : " + std::string(dlerror()));

        dlerror();

        // Chargement des Infos du Module
        register_module_info_f infos_load_fptr = (register_module_info_f) load_function(handle, "function_register_module_info");
        ShortInfo infos = (*infos_load_fptr)();

        // Chargement de la fonction Builder
#ifdef __SFX_PI__
        create_module_data_f builder = (create_module_data_f) load_function(handle, "function_create_jack_module");
#else
        create_module_data_f builder = (create_module_data_f) load_function(handle, "function_create_non_jack_module");
#endif

        // Chargement de la fonction Destructor
        destroy_module_data_f destructor = (destroy_module_data_f) load_function(handle, "function_destroy_module");

        // Chargement de la Table des Slots
        register_slot_table_f slots_load_fptr = (register_slot_table_f) load_function(handle, "function_register_module_slots");
        SlotTable slots = (*slots_load_fptr)();

#ifdef __SFX_PI__
        JackProcessCallback callback = (JackProcessCallback) load_function(handle, "function_process_callback");
#endif

        if (ModuleTable.find(infos.unique_name) != ModuleTable.end())
        {
            sfx::wrn(NAME, "Module : \"%s\" is Already loaded ...\n", infos.unique_name);
            throw std::runtime_error("Module_Name_Already_Used");
        }

        ModuleTable.emplace(infos.unique_name, std::make_shared<Module>(
                infos,
                slots,
                builder,
                destructor,
#ifdef __SFX_PI__
                callback,
#endif
                handle
                ));

        sfx::sfxStream << "\b Succès du Chargement\n";
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::wrn(NAME, "Echec de Chargement : %s\n", e.what());
        return 1;
    }
    catch (std::bad_alloc const& e)
    {
        sfx::wrn(NAME, "Echec d'Allocation : %s\n", e.what());
        return 1;
    }
    catch (std::runtime_error const& e)
    {
        sfx::wrn(NAME, "Echec d'enregistrement : %s\n", e.what());
        return 1;
    }

#elif defined(__WINDOWS__)
#endif

    return 0;
}

int Module::unloadModule(std::string unique_name)
{
    if (ModuleTable.find(unique_name) == ModuleTable.end())
    {
        sfx::err(NAME, "Module : \"%s\" is not loaded ...\n", unique_name);
        return 1;
    }
    
    if (ModuleTable[unique_name]->infos.type == ShortInfo::SYSTEM)
    {
        sfx::err(NAME, "Unloading System Module \"%s\" is Forbiden ...\n", unique_name);
        return 1;
    }

    // Remove all effects from this module
    for (auto& unit : ModuleTable[unique_name]->relatedUnits)
    {
        EffectUnit::destroyEffectUnit(unit);
    }

    // Unload the library
#if defined(__UNIX__)

    dlclose(ModuleTable[unique_name]->lib_handle);

#elif defined(__WINDOWS__)
#endif

    ModuleTable.erase(unique_name);
    return 0;
}
int Module::forceUnloadModule(std::string unique_name)
{
    if (ModuleTable.find(unique_name) == ModuleTable.end())
    {
        sfx::err(NAME, "Module : \"%s\" is not loaded ...\n", unique_name);
        return 1;
    }

    // Remove all effects from this module
    for (auto& unit : ModuleTable[unique_name]->relatedUnits)
    {
        EffectUnit::forceDestroyEffectUnit(unit);
    }

    // Unload the library
#if defined(__UNIX__)

    dlclose(ModuleTable[unique_name]->lib_handle);

#elif defined(__WINDOWS__)
#endif

    ModuleTable.erase(unique_name);
    return 0;
}

/**
 * @brief Fonction de wrapping pour charger une fonction depuis une librairie dynamique
 * @param name Nom de la fonction à charger
 * @return pointeur vers la fonction chargée
 * @throw ios_base::failure
 */
void* Module::load_function(void* handle, std::string name)
{
#if defined(__UNIX__)
    void* function_fptr = dlsym(handle, name.c_str());
    if (!function_fptr)
        throw std::ios_base::failure(dlerror());
    return function_fptr;
#elif defined(__WINDOWS__)
    return nullptr;
#endif
}
