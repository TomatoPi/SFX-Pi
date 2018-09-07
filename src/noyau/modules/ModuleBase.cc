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
 * File:   ModuleBase.cc
 * Author: Space-Computer
 * 
 * Created on 30 août 2018, 20:22
 */

#include "ModuleBase.h"

#define NAME "Module-API"

void Module::logModuleLoadedInfos() const
{
    sfx::log(NAME, "Module Informations : Name:\"%s\" Version:\"%s\"\n", infos.name, infos.version);
    sfx::sfxStream << "Slots : \n" <<
            sfx::formatString("%-20s|%-20s|%-20s|%-20s\n"
                , std::string("Clef"), std::string("Nom Interne")
                , std::string("Nom d'Affichage"), std::string("Valeur par défault"))
            << "--------------------+--------------------+--------------------+--------------------\n";
                
    for (auto& slot : slots)
    {
        sfx::sfxStream << sfx::formatString("%-20s|%-20s|%-20s|%3i\n"
                , slot.first, slot.second->internal_name, slot.second->display_name, slot.second->default_value);
    }
    sfx::sfxStream << "--------------------+--------------------+--------------------+--------------------\n";
}

#undef NAME
#define NAME "Effect-API"

EffectUnit::EffectUnit(std::shared_ptr<Module> module):
    name(module->infos.name),
    datas(nullptr),
    module(module),

    audioIns(), audioOuts(), midiIns(), midiOuts(),
        
    links(),
    slots(),

    banks(),
    banks_order(),
    current_bank()
{
    // Creation des slots
    for (auto& slot : module->slots)
    {
        slots[slot.first] = SlotStatus {
                .slot = slot.second,
                .value = 0,
                .status = SlotStatus::ENABLED
            };
    }
    
#ifdef __ARCH_LINUX__
    // Construction du client Jack
    client = sfx::openJackClient(name);
    jack_set_process_callback(client, module->callback, this);
#endif
    // Allocation des Ports
    this->registerMidiInput("Modulation");
    this->registerMidiOutput("Through");
    
    // Construction des datas du Module et allocation des ports specifiques
    datas = module->builder(this);
    
    // Creation et chargement de la banque par défault
    this->createBank(0);
    current_bank = banks_order.begin();
    this->updateCurrentBank();
    
#ifdef __ARCH_LINUX__
    sfx::activateClient(client);
#endif
}

EffectUnit::~EffectUnit()
{
#ifdef __ARCH_LINUX__
    jack_client_close(client);
#endif
    module->destructor(datas);
}

#ifdef __ARCH_LINUX__
/**
 * Fonction à appeller dans la boucle de process pour traiter les CC midi et 
 *  déclancher les slots eventuels
 * @param event pointeur vers le buffer de l'evenement midi 
 */
void EffectUnit::veryfyAndComputeCCMessage(sfx::hex_t* event)
{
    sfx::hex_t ccc = sfx::Midi_read_CCChannel(event);
    sfx::hex_t ccs = sfx::Midi_read_CCSource(event);
    sfx::hex_t val = sfx::Midi_read_CCValue(event);
    sfx::hex_pair_t cc = std::make_pair(ccc, ccs);

    sfx::debug("Native-MIDI", "Process Midi Event : %i %i\n", ccc, ccs);

    if (links.find(cc) != links.end())
    {
        for (auto& slot : links[cc])
            if (slots.find(slot) != slots.end())
            {
                sfx::debug("Native-MIDI", "Call slot : \"%s\" with value : %i\n", module->slots[slot]->internal_name, val);
                this->setSlotValue(slot, val);
            }
            else
            {
                sfx::wrn("Native-MIDI", "Unable to find slot : \"%s\" linked to CC : %i %i\n", slot, ccc, ccs);
            }
    }
}
#endif

/**
 * Fonction utilisée pour connecter un slot à un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom interne du slot à connecter
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::linkSlot(sfx::hex_pair_t cc, std::string slot)
{
    sfx::debug(name, "Try Link slot : \"%s\" to cc : %i:%i \n", slot, cc.first, cc.second);
    
    if (module->slots.find(slot) == module->slots.end())
        return MISSING_SLOT;
    
    if (links.find(cc) == links.end())
        links[cc] = std::vector<std::string>();
    
    //if (links[cc].find(slot) != links[cc].end())
    //    return 1;
    
    sfx::debug(name, "Success !\n");
    links[cc].push_back(slot);
    return 0;
}
/**
 * Fonction utilisée pour déconnecter un slot à d'un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom interne du slot à connecter
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, 1 if link doesn't exist
 */
int EffectUnit::unlinkSlot(sfx::hex_pair_t cc, std::string slot)
{
    if (module->slots.find(slot) == module->slots.end())
        return MISSING_SLOT;
    
    if (links.find(cc) == links.end())
        return 1;
    
    //if (links[cc].find(slot) == links[cc].end())
    //    return 1;
    
    bool missing = true;
    
    for (std::vector<std::string>::iterator itr = links[cc].begin();
        itr != links[cc].end();
        ++itr)
    {
        if (slot == *itr)
        {
            itr = links[cc].erase(itr);
            missing = false;
        }
    }
    
    if (links[cc].size() == 0) links.erase(cc);
    
    return (int)missing;
}

/**
 * Fonction utilisée pour activer un slot
 * @param slot le nom interne du slot à activer
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::enableSlot(std::string slot)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;
    
    sfx::debug(name, "Enabled slot : \"%s\"\n", slot);
    slots[slot].status = SlotStatus::ENABLED;
    return 0;
}
/**
 * Fonction utilisée pour désactiver un slot
 * @param slot le nom interne du slot à désactiver
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::disableSlot(std::string slot)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;
    
    sfx::debug(name, "Disabled slot : \"%s\"\n", slot);
    slots[slot].status = SlotStatus::DISABLED;
    return 0;
}

/**
 * Fonction utiliée pour changer la valeur d'un slot
 *  Si le slot est désactivé la fonction est sans effet
 * @param slot nom interne du slot à changer
 * @param value valleur à assigner au slot (sur 7bit)
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
 */
int EffectUnit::setSlotValue(std::string slot, sfx::hex_t value, bool force)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;
    
    //sfx::debug(NAME, "1 Called slot : \"%s\" : Status : %i\n", slot, slots[slot].status);
    
    if (!!(slots[slot])) // If slot is enabled
    {
        float v = (*slots[slot].slot->callback)(value, this);
        slots[slot].value = value;
        sfx::debug("Native-MIDI", "Called slot : \"%s\" with value : %i => %f\n", slot, value, v);
    }
    else if (force) // Else if slot is disabled, save value anyway
    {
        slots[slot].value = value;
    }
    else return DISABLED_SLOT; // Else return an error
        
    return 0;
}
/**
 * Fonction utiliée pour récuperer la valeur interne d'un slot
 *  Si le slot est désactivé la fonction est sans effet.
 *  laisser à nullptr les valeurs non souhaitées
 * @param slot nom interne du slot ciblé
 * @param x_result pointeur pour récuperer la valeur sur 7bit du slot
 * @param f_result pointeur pour récuperer la valeur interne du slot
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
 */
int EffectUnit::getSlotValue(std::string slot, sfx::hex_t* x_result, float* f_result)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;
    
    if (!!(slots[slot]))
    {
        float value = (*slots[slot].slot->callback)(255, this);
        
        if (x_result) *x_result = slots[slot].value;
        if (f_result) *f_result = value;
    }
    else return DISABLED_SLOT;
        
    return 0;
}

void EffectUnit::logLinkedSlots() const
{
    sfx::log(name, "Linked Slots : \n");
    sfx::sfxStream << sfx::formatString("%-10s|%-10s|%-20s", 
            std::string("Channel"), std::string("Source"), std::string("Slot"))
            << "----------+----------+--------------------\n";
    for (auto& link : links)
        for (auto& slot : link.second)
        {
            sfx::sfxStream << sfx::formatString("%-10i|%-10i|%-20s\n"
                    , link.first.first, link.first.second
                    , slot);
        }
    sfx::sfxStream << "----------+----------+--------------------\n";
}
  
////////////////////////////////////////////////////////////////////
// Gestion des Banques
////////////////////////////////////////////////////////////////////
   
/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
int EffectUnit::setBank(sfx::hex_t id)
{
    if (banks.find(id) == banks.end()) return -1;
    
    // Update current_bank iterator
    for(BankIdList::iterator itr = banks_order.begin();
        itr != banks_order.end();
        ++itr)
        if (*itr == id)
        {
            current_bank = itr;
            break;
        }
    
    // Update all params
    this->updateCurrentBank();
    return 0;
}
/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
void EffectUnit::nextBank()
{
    BankIdList::iterator nitr(current_bank);
    ++nitr;
    if (nitr == banks_order.end())
        nitr = banks_order.begin();
    
    // Update all params
    current_bank = nitr;
    this->updateCurrentBank();
}
/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
void EffectUnit::prevBank()
{
    BankIdList::iterator nitr(current_bank);
    
    if (nitr == banks_order.begin())
        nitr = banks_order.end();
    --nitr;
    
    // Update all params
    current_bank = nitr;
    this->updateCurrentBank();
}

/**
 * Fonction interne utilisée pour mettre à jour l'effet en appelant tout les slots
 *  actifs de la banque courante
 * @pre current_bank bank is valid
 */
void EffectUnit::updateCurrentBank()
{
    for (auto& slot : banks[*current_bank])
    {
        this->setSlotValue(slot.first, slot.second, true);
    }
}

/**
 * @brief method to add a bank to the effect
 *  Usage :
 *      createBank() to create a bank filled with default values
 *      createBank(id, size, vals) to create a bank from a valid array of values
 * @pre if specified, bank is a valid expansion of effect's config tree
 */
int EffectUnit::createBank(sfx::hex_t id)
{
    // If bank doesn't exist create it
    if (banks.find(id) == banks.end()) {

        ParamArray array;
        for (auto& slot : module->slots)
            array[slot.first] = slot.second->default_value;
        
        banks[id] = array;
        return 0;
    }
    // Else this is an error
    else return EXISTING_BANK;
}
/**
 * @brief method to duplicate a bank of the effect
 *  Usage :
 *      copyBank(sid) to copy bank[sid] to a new one
 *      copyBank(sid, tid) to copy bank[sid] in bank[tid]
 * @pre sid != tid
 */
int EffectUnit::copyBank(sfx::hex_t sid, sfx::hex_t tid)
{
    assert(sid != tid);
    
    if (banks.find(sid) != banks.end()) {

        if (banks.find(tid) == banks.end()) this->createBank(tid);
        
        for (auto& slot : banks[tid])
        {
            slot.second = banks[sid][slot.first];
        }

        if (*current_bank == tid) this->updateCurrentBank();
        
        return 0;
    }
    else return MISSING_BANK;
}
/**
 * @brief Method used to remove a bank from the effect
 *  if the bank removed is the last one, automaticaly regenerate the default bank
 * @pre _currentBank bank is valid
 * @post _currentBank bank is valid
 */
int EffectUnit::removeBank(sfx::hex_t id)
{
    assert(current_bank != banks_order.end());
    
    if (banks.find(id) == banks.end()) return MISSING_BANK;
    banks.erase(id);
    
    bool updateAll = false;
    
    if (*current_bank == id)
    {
        if (current_bank != banks_order.begin())
            --current_bank;
        else if (current_bank != --banks_order.end())
            ++current_bank;
        
        updateAll = true;
        
        current_bank = banks_order.erase(current_bank);
    }
    else banks_order.remove_if([id](sfx::hex_t n)->bool{return n == id;});


    // Always keep one bank inside the ParamSet
    if (banks.size() == 0)
    {
        this->createBank(0);
        current_bank = banks_order.begin();
        updateAll = true;
    }
    
    if (updateAll) this->updateCurrentBank();
    
    assert(current_bank != banks_order.end());
    return 0;
}