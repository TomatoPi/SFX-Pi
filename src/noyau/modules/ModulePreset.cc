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
 * File:   ModulePreset.cc
 * Author: Space-Computer
 * 
 * Created on 8 septembre 2018, 01:46
 */

#include <fstream>

#include "ModulePreset.h"

#define NAME "Module-Serial"

/**
 *  string : nom unique
 *  string : version
 */
void serialize_infos(std::ofstream& flux, const Module::ShortInfo* obj)
{
    sfx::serial::write<std::string> (flux, obj->unique_name);
    sfx::serial::write<std::string> (flux, obj->version);
}
int deserialize_infos(std::ifstream& flux, Module::ShortInfo* obj)
{
    std::string name = sfx::serial::read<std::string> (flux);
    std::string version = sfx::serial::read<std::string> (flux);
    
    *obj = Module::ShortInfo(name, "", version);
    return 0;
}

////////////////////////////////////////////////////////////////////
// Serialisation
////////////////////////////////////////////////////////////////////
#undef NAME
#define NAME "Effect-Serial"
/**
 *  usize_t : nombre de paramètres
 *  {
 *      string : nom unique
 *      hex_t  : valeur
 *  }
 */
void serialize_bank(std::ofstream& flux, const EffectUnit::ParamArray* obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj->size());
    for (auto& slot : *obj)
    {
        sfx::serial::write<std::string>(flux, slot.first);
        sfx::serial::write<sfx::hex_t> (flux, slot.second);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
int deserialize_bank(std::ifstream& flux, EffectUnit::ParamArray* obj)
{
    try 
    {
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_Begin");
        
        EffectUnit::ParamArray parsed_bank;
        
        sfx::usize_t size = sfx::serial::read<sfx::usize_t>(flux);
        for (size_t i = 0; i < size; ++i)
        {
            std::string name = sfx::serial::read<std::string>(flux);
            sfx::hex_t value = sfx::serial::read<sfx::hex_t> (flux);
            
            parsed_bank[name] = value;
        }
        
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_End");
        
        *obj = parsed_bank;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *      bank  : la banque
 *  }
 */
void serialize_banktable(std::ofstream& flux, const EffectUnit::BankTable* obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj->size());
    for (auto& bank : *obj)
    {
        sfx::serial::write<sfx::hex_t>(flux, bank.first);
        serialize_bank(flux, &bank.second);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
int deserialize_banktable(std::ifstream& flux, EffectUnit::BankTable* obj)
{
    try 
    {
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_Table_Begin");
        
        EffectUnit::BankTable parsed_table;
        
        sfx::usize_t size = sfx::serial::read<sfx::usize_t>(flux);
        for (size_t i = 0; i < size; ++i)
        {
            sfx::hex_t id = sfx::serial::read<sfx::hex_t> (flux);
            EffectUnit::ParamArray parsed_bank;
            if (deserialize_bank(flux, &parsed_bank))
                throw std::ios_base::failure("Failed_Parse_Bank");
            
            parsed_table[id] = parsed_bank;
        }
        
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_Table_End");
        
        *obj = parsed_table;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *  }
 */
void serialize_bankidlist(std::ofstream& flux, const EffectUnit::BankIdList* obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj->size());
    for (auto& id : *obj)
    {
        sfx::serial::write<sfx::hex_t> (flux, id);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
int deserialize_bankidlist(std::ifstream& flux, EffectUnit::BankIdList* obj)
{
    try 
    {
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_ID_List_Begin");
        
        EffectUnit::BankIdList parsed_list;
        
        sfx::usize_t size = sfx::serial::read<sfx::usize_t>(flux);
        for (size_t i = 0; i < size; ++i)
        {
            sfx::hex_t id = sfx::serial::read<sfx::hex_t> (flux);
            
            parsed_list.push_back(id);
        }
        
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_ID_List_End");
        
        *obj = parsed_list;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}

/**
 *  usize_t : nombre de liens
 *  {
 *      hex_t : channel du cc
 *      hex_t : source du cc
 *      usize_t : nombre de cibles
 *      {
 *          string : nom du slot
 *      }
 *  }
 */
void serialize_linktable(std::ofstream& flux, const EffectUnit::LinkTable* obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj->size());
    for (auto& link : *obj)
    {
        sfx::serial::write<sfx::hex_t> (flux, link.first.first);
        sfx::serial::write<sfx::hex_t> (flux, link.first.second);
        
        sfx::serial::write<sfx::usize_t>(flux, link.second.size());
        for (auto& slot : link.second)
        {
            sfx::serial::write<std::string> (flux, slot);
        }
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
int deserialize_linktable(std::ifstream& flux, EffectUnit::LinkTable* obj)
{
    try 
    {
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Link_Table_Begin");
        
        EffectUnit::LinkTable parsed_table;
        
        sfx::usize_t link_size = sfx::serial::read<sfx::usize_t>(flux);
        for (size_t i = 0; i < link_size; ++i)
        {
            sfx::hex_t ccc = sfx::serial::read<sfx::hex_t> (flux);
            sfx::hex_t ccs = sfx::serial::read<sfx::hex_t> (flux);
            
            sfx::hex_pair_t cc = std::make_pair(ccc, ccs);
            parsed_table[cc] = std::vector<std::string>();
            
            sfx::usize_t slot_size = sfx::serial::read<sfx::usize_t>(flux);
            for(size_t k = 0; k < slot_size; ++k)
            {
                std::string slot = sfx::serial::read<std::string> (flux);
                parsed_table[cc].push_back(slot);
            }
        }
        
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Link_Table_End");
        
        *obj = parsed_table;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
    
////////////////////////////////////////////////////////////////////
// Preset
////////////////////////////////////////////////////////////////////
#undef NAME
#define NAME "Effect-Preset"

/**
 *  flag_t : EffectUnit_Preset_File
 *  Module Infos
 *  string : Nom de l'effet
 *  string : Nom du fichier des banques
 *  string : Nom du fichier des links
 */
int save_PresetFile(std::string file_path, const EffectUnit* obj)
{
    std::ofstream flux;
    sfx::debug(NAME, "Save effect : \"%s\" : to file : \"%s\"\n",
            obj->name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Preset_File);
        serialize_infos(flux, &obj->module->infos);
        
        // Write effect name
        sfx::serial::write<std::string>(flux, obj->name);
        
        // Write bank file
        if (save_BankFile(file_path + std::string("_banks"), obj))
            throw std::ios_base::failure("Failed write bank file");
        
        // Write links file
        if (save_LinkFile(file_path + std::string("_links"), obj))
            throw std::ios_base::failure("Failed write links file");
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Writing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
int load_PresetFile(std::string file_path, EffectUnit* obj)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load effect from file : \"%s\"\n", file_path);
            
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Read file header
        sfx::serial::controlValue(flux, sfx::serial::EffectUnit_Preset_File, "Invalid_File_Flag");
        
        Module::ShortInfo infos("", "", "");
        deserialize_infos(flux, &infos);
        
        // Verify module type
        if (infos.unique_name != obj->module->infos.unique_name)
            throw std::ios_base::failure("Module_Type_Missmatch");
        // Verify module version
        if (infos.version != obj->module->infos.version)
            sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                    infos.version, obj->module->infos.version);
        
        // Read Effect Name
        std::string name = sfx::serial::read<std::string>(flux);
        EffectUnit tmp_unit = EffectUnit(obj->module);
        
        // Read bank file
        if (load_BankFile(file_path + std::string("_banks"), &tmp_unit))
            throw std::ios_base::failure("Failed read bank file");
        
        // Write links file
        if (load_LinkFile(file_path + std::string("_links"), &tmp_unit))
            throw std::ios_base::failure("Failed read links file");
        
        // If load is successfull, replace old effect with it new values
        obj->banks        = tmp_unit.banks;
        obj->banks_order  = tmp_unit.banks_order;
        obj->current_bank = obj->banks_order.begin();
        
        obj->links = tmp_unit.links;
        
        obj->updateCurrentBank();
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}

/**
 * flag_t  : EffectUnit_Bank_File
 * Module Infos
 * BankTable
 * BankIdList
 */
int save_BankFile(std::string file_path, const EffectUnit* obj)
{
    std::ofstream flux;
    sfx::debug(NAME, "Save banks : \"%s\" : to file : \"%s\"\n",
            obj->name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Bank_File);
        serialize_infos(flux, &obj->module->infos);
        
        // Write all banks
        serialize_banktable(flux, & obj->banks);
        serialize_bankidlist(flux, & obj->banks_order);
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Writing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
int load_BankFile(std::string file_path, EffectUnit* obj)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load banks from file : \"%s\"\n", file_path);
            
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Read file header
        sfx::serial::controlValue(flux, sfx::serial::EffectUnit_Bank_File, "Invalid_File_Flag");
        
        Module::ShortInfo infos("", "", "");
        deserialize_infos(flux, &infos);
        
        // Verify module type
        if (infos.unique_name != obj->module->infos.unique_name)
            throw std::ios_base::failure("Module_Type_Missmatch");
        // Verify module version
        if (infos.version != obj->module->infos.version)
            sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                    infos.version, obj->module->infos.version);
        
        // Parse the Bank table
        EffectUnit::BankTable parsed_table;
        if (deserialize_banktable(flux, &parsed_table))
            throw std::ios_base::failure("Failed read Bank Table");
        
        // Parse Bank Order List
        EffectUnit::BankIdList parsed_orderlist;
        if (deserialize_bankidlist(flux, &parsed_orderlist))
            throw std::ios_base::failure("Failed read Bank Order");
        
        /*
         * On ecrème les banques des slots qui n'existeraient plus
         * Et on ajoute à chacune les slots qui manqueraient
         * de sorte à garrantir la compatibilité des presets
         */
        for (auto& bank : parsed_table)
        {
            // Utilisé pour stoquer la liste des slots pas encore rencontrés
            std::set<std::string> missing_slots;
            for (auto& slot : obj->module->slots)
                missing_slots.emplace(slot.second->internal_name);
            
            for(auto& slot : bank.second)
            {
                // Si le slot est valide on le retire des slots à trouver
                if (missing_slots.find(slot.first) != missing_slots.end())
                    missing_slots.erase(slot.first);
                else
                    sfx::wrn(NAME, "Invalid Slot : \"%s\"\n", slot.first);
            }
            
            // On rajoute les slots manquants
            for (auto& slot : missing_slots)
            {
                bank.second[slot] = obj->module->slots[slot]->default_value;
                sfx::wrn(NAME, "Manualy added Slot : \"%s\"\n", slot);
            }
        }
        
        // If load is successfull, replace old effect with it new values
        obj->banks        = parsed_table;
        obj->banks_order  = parsed_orderlist;
        obj->current_bank = obj->banks_order.begin();
        
        obj->updateCurrentBank();
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}

/**
 * flag_t  : EffectUnit_Link_File
 * Module Infos
 * LinksTable
 */
int save_LinkFile(std::string file_path, const EffectUnit* obj)
{
    std::ofstream flux;
    sfx::debug(NAME, "Save Links : \"%s\" : to file : \"%s\"\n",
            obj->name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Link_File);
        serialize_infos(flux, &obj->module->infos);
        
        // Write all links
        serialize_linktable(flux, & obj->links);
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Writing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}
int load_LinkFile(std::string file_path, EffectUnit* obj)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load links from file : \"%s\"\n", file_path);
            
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Read file header
        sfx::serial::controlValue(flux, sfx::serial::EffectUnit_Link_File, "Invalid_File_Flag");
        
        Module::ShortInfo infos("", "", "");
        deserialize_infos(flux, &infos);
        
        // Verify module type
        if (infos.unique_name != obj->module->infos.unique_name)
            throw std::ios_base::failure("Module_Type_Missmatch");
        // Verify module version
        if (infos.version != obj->module->infos.version)
            sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                    infos.version, obj->module->infos.version);
        
        // Parse the link table
        EffectUnit::LinkTable parsed_table;
        if (deserialize_linktable(flux, &parsed_table))
            throw std::ios_base::failure("Failed read Link Table");
        
        /*
         * On ecrème les liens en retirant les slots invalides
         */
        for (auto& link : parsed_table)
            for (std::vector<std::string>::iterator itr = link.second.begin();
                    itr != link.second.end();
                    ++itr)
            {
                if (obj->module->slots.find(*itr) == obj->module->slots.end())
                {
                    itr = link.second.erase(itr);
                    sfx::wrn(NAME, "Remove link : %i => \"%s\" : Invalid Slot\n",
                            link.first, *itr);
                }
            }
        
        // If load is successfull, replace old effect with it new values
        obj->links = parsed_table;
        
        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}