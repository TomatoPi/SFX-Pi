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
#include "ModuleBase.h"

#define NAME "Module-Serial"

/**
 *  string : nom unique
 *  string : version
 */
void Module::serialize_infos(std::ofstream& flux, const ShortInfo& obj)
{
    sfx::serial::write<std::string> (flux, obj.unique_name);
    sfx::serial::write<std::string> (flux, obj.version);
}
Module::ShortInfo Module::deserialize_infos(std::ifstream& flux)
{
    std::string name = sfx::serial::read<std::string> (flux);
    std::string version = sfx::serial::read<std::string> (flux);
    
    return Module::ShortInfo(name, "", version);
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
void EffectUnit::serialize_bank(std::ofstream& flux, const EffectUnit::ParamArray& obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj.size());
    for (auto& slot : obj)
    {
        sfx::serial::write<std::string>(flux, slot.first);
        sfx::serial::write<sfx::hex_t> (flux, slot.second);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
EffectUnit::ParamArray EffectUnit::deserialize_bank(std::ifstream& flux)
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
        
        return parsed_bank;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        throw;
    }
}
/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *      bank  : la banque
 *  }
 */
void EffectUnit::serialize_banktable(std::ofstream& flux, const EffectUnit::BankTable& obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj.size());
    for (auto& bank : obj)
    {
        sfx::serial::write<sfx::hex_t>(flux, bank.first);
        serialize_bank(flux, bank.second);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
EffectUnit::BankTable EffectUnit::deserialize_banktable(std::ifstream& flux)
{
    try 
    {
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_Table_Begin");
        
        EffectUnit::BankTable parsed_table;
        
        sfx::usize_t size = sfx::serial::read<sfx::usize_t>(flux);
        for (size_t i = 0; i < size; ++i)
        {
            sfx::hex_t id = sfx::serial::read<sfx::hex_t> (flux);
            EffectUnit::ParamArray parsed_bank = deserialize_bank(flux);
            
            parsed_table[id] = parsed_bank;
        }
        
        sfx::serial::controlValue(flux, sfx::serial::ControlBits, "Invalid_Bank_Table_End");
        
        return parsed_table;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        throw;
    }
}
/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *  }
 */
void EffectUnit::serialize_bankidlist(std::ofstream& flux, const EffectUnit::BankIdList& obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj.size());
    for (auto& id : obj)
    {
        sfx::serial::write<sfx::hex_t> (flux, id);
    }
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
}
EffectUnit::BankIdList EffectUnit::deserialize_bankidlist(std::ifstream& flux)
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
        
        return parsed_list;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        throw;
    }
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
void EffectUnit::serialize_linktable(std::ofstream& flux, const EffectUnit::LinkTable& obj)
{
    sfx::serial::write<sfx::flag_t> (flux, sfx::serial::ControlBits);
    
    sfx::serial::write<sfx::usize_t>(flux, obj.size());
    for (auto& link : obj)
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
EffectUnit::LinkTable EffectUnit::deserialize_linktable(std::ifstream& flux)
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
        
        return parsed_table;
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        throw;
    }
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
int EffectUnit::save_PresetFile(std::string file_path, std::weak_ptr<EffectUnit> obj,
            std::string bank_file, std::string link_file)
{
    std::shared_ptr<EffectUnit> unit;
    if (!(unit = obj.lock()))
    {
        sfx::err(NAME, "Effect Unit has been Deleted\n");
        return 1;
    }
    
    std::ofstream flux;
    sfx::debug(NAME, "Save effect : \"%s\" : to file : \"%s\"\n",
            unit->unique_name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Preset_File);
        if (auto module = unit->module.lock())
            Module::serialize_infos(flux, module->getInformations());
        else
            throw std::runtime_error("Module_Expired");
        
        // Write effect name
        sfx::serial::write<std::string>(flux, unit->display_name);
        
        // Write bank file path
        sfx::serial::write<std::string>(flux, bank_file);
        // Write link file path
        sfx::serial::write<std::string>(flux, link_file);
        
        // Write bank file
        if (save_BankFile(bank_file, unit))
            throw std::ios_base::failure("Failed write bank file");
        
        // Write links file
        if (save_LinkFile(link_file, unit))
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
/**
 * flag_t  : EffectUnit_Bank_File
 * Module Infos
 * BankTable
 * BankIdList
 */
int EffectUnit::save_BankFile(std::string file_path, std::weak_ptr<EffectUnit> obj)
{
    std::shared_ptr<EffectUnit> unit;
    if (!(unit = obj.lock()))
    {
        sfx::err(NAME, "Effect Unit has been Deleted\n");
        return 1;
    }
    
    std::ofstream flux;
    sfx::debug(NAME, "Save banks : \"%s\" : to file : \"%s\"\n",
            unit->unique_name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Bank_File);
        if (auto module = unit->module.lock())
            Module::serialize_infos(flux, module->getInformations());
        else
            throw std::runtime_error("Module_Expired");
        
        // Write all banks
        serialize_banktable(flux, unit->banks);
        serialize_bankidlist(flux, unit->banks_order);
        
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
int EffectUnit::load_BankFile(std::string file_path, std::weak_ptr<EffectUnit> obj)
{
    std::shared_ptr<EffectUnit> unit;
    if (!(unit = obj.lock()))
    {
        sfx::err(NAME, "Effect Unit has been Deleted\n");
        return 1;
    }
    
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
        
        
        // Verify Module Informations
        if (auto module = unit->module.lock())
        {
            Module::ShortInfo infos = Module::deserialize_infos(flux);
            
            // Verify module type
            if (infos.unique_name != module->getInformations().unique_name)
                throw std::ios_base::failure("Module_Type_Missmatch");
            // Verify module version
            if (infos.version != module->getInformations().version)
                sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                        infos.version, module->getInformations().version);
        }
        else throw std::runtime_error("Module_Expired");
        
        // Parse the Bank table
        EffectUnit::BankTable parsed_table = deserialize_banktable(flux);
        
        // Parse Bank Order List
        EffectUnit::BankIdList parsed_orderlist = deserialize_bankidlist(flux);
        
        /*
         * On ecrème les banques des slots qui n'existeraient plus
         * Et on ajoute à chacune les slots qui manqueraient
         * de sorte à garrantir la compatibilité des presets
         */
        UIDManager<sfx::hex_t> id_manager(0);
        for (auto& bank : parsed_table)
        {
            // Utilisé pour stoquer la liste des slots pas encore rencontrés
            std::set<std::string> missing_slots;
            for (auto& slot : unit->slots)
                missing_slots.emplace(slot.second.slot->internal_name);
            
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
                bank.second[slot] = unit->slots[slot].slot->default_value;
                sfx::wrn(NAME, "Manualy added Slot : \"%s\"\n", slot);
            }
            
            // On verouille l'id de la banque
            id_manager.reserveUID(bank.first);
        }
        
        // If load is successfull, replace old effect with it new values
        unit->banks        = parsed_table;
        unit->banks_order  = parsed_orderlist;
        unit->current_bank = unit->banks_order.begin();
        
        unit->bank_id_manager = id_manager;
        
        unit->updateCurrentBank();
        
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
int EffectUnit::save_LinkFile(std::string file_path, std::weak_ptr<EffectUnit> obj)
{
    std::shared_ptr<EffectUnit> unit;
    if (!(unit = obj.lock()))
    {
        sfx::err(NAME, "Effect Unit has been Deleted\n");
        return 1;
    }
    
    std::ofstream flux;
    sfx::debug(NAME, "Save Links : \"%s\" : to file : \"%s\"\n",
            unit->unique_name, file_path);
    
    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
        
        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::EffectUnit_Link_File);
        if (auto module = unit->module.lock())
            Module::serialize_infos(flux, module->getInformations());
        else
            throw std::runtime_error("Module_Expired");
        
        // Write all links
        serialize_linktable(flux, unit->links);
        
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
int EffectUnit::load_LinkFile(std::string file_path, std::weak_ptr<EffectUnit> obj)
{
    std::shared_ptr<EffectUnit> unit;
    if (!(unit = obj.lock()))
    {
        sfx::err(NAME, "Effect Unit has been Deleted\n");
        return 1;
    }
    
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
        
        // Verify Module Informations
        if (auto module = unit->module.lock())
        {
            Module::ShortInfo infos = Module::deserialize_infos(flux);
            
            // Verify module type
            if (infos.unique_name != module->getInformations().unique_name)
                throw std::ios_base::failure("Module_Type_Missmatch");
            // Verify module version
            if (infos.version != module->getInformations().version)
                sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                        infos.version, module->getInformations().version);
        }
        else throw std::runtime_error("Module_Expired");
        
        // Parse the link table
        EffectUnit::LinkTable parsed_table = deserialize_linktable(flux);
        
        /*
         * On ecrème les liens en retirant les slots invalides
         */
        for (auto& link : parsed_table)
            for (std::vector<std::string>::iterator itr = link.second.begin();
                    itr != link.second.end();
                    ++itr)
            {
                if (unit->slots.find(*itr) == unit->slots.end())
                {
                    itr = link.second.erase(itr);
                    sfx::wrn(NAME, "Remove link : %i => \"%s\" : Invalid Slot\n",
                            link.first, *itr);
                }
            }
        
        // If load is successfull, replace old effect with it new values
        unit->links = parsed_table;
        
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