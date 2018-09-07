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
 * File:   ModuleBase.h
 * Author: Space-Computer
 *
 * Created on 30 août 2018, 20:22
 */

#ifndef MODULEBASE_H
#define MODULEBASE_H

#include <map>
#include <list>
#include <vector>
#include <memory>

#include "noyau/types.h"
#include "noyau/utils.h"
#include "noyau/midi.h"

#ifdef __ARCH_LINUX__
#include "process/module/jackWrapper.h"
#endif

struct EffectUnit;

/**
 * Objet représentant un type d'effet, contiens les informations relatives
 *  et les fonctions permettant d'initialiser un EffectUnit correspondant
 */
extern "C"
struct Module
{
    
    /**
     * Structure utilisée pour representer une possible connection
     *  entre un parametre et une valeur midi
     */
    struct Slot
    {
        /**
         * Fonction appelée pour modifier la valeur controlée par le slot
         * @param sfx::hex_t Valeur à attribuer (valeur 0-127), ou >= 128 pour demander la valeur courante
         * @param void* Pointeur vers le bloc de paramètres du module
         * @return la valeur nouvelle valeur du slot;
         */
        typedef float (*slot_f)(sfx::hex_t, EffectUnit*);
        
        std::string internal_name;
        slot_f callback;
        
        std::string display_name;
        sfx::hex_t default_value;
    };
    /**
     * Table stoquant les Slots par nom
     */
    typedef std::map<std::string, std::shared_ptr<Slot>> SlotTable;
    /**
     * Fonction utilisée pour enregistrer un slot
     * @param table la table où ajouter le slot
     * @param internal_name nom interne du slot
     * @param display_name nom d'affichage du slot
     * @param callback fonction appekée pour modifier la valeur controlée par le slot
     */
    static inline void register_slot(SlotTable& table, sfx::hex_t def_val
        , std::string internal_name, std::string display_name, Slot::slot_f callback)
    {
        table[internal_name] = std::shared_ptr<Slot>(new Slot{ 
            .internal_name = internal_name,
            .callback      = callback,
            .display_name  = display_name,
            .default_value = def_val
        });
    }
    
    /**
     * Prototype de la fonction de déclaration des Slots
     */
    typedef SlotTable (*register_slot_table_f)(void);
    /**
     * Proptotype de la fonction pour creer les datas d'un Module
     */
    typedef void* (*create_module_data_f)(EffectUnit*);
    typedef void (*destroy_module_data_f)(void*);
    
    struct ShortInfo
    {
        ShortInfo(std::string n, std::string v):name(n),version(v)
        {
        }
        
        std::string name;
        std::string version;
    }infos;
    
    typedef ShortInfo (*register_module_info_f)(void);

    Module::create_module_data_f builder;
    Module::destroy_module_data_f destructor;

    Module::SlotTable slots;

#ifdef __ARCH_LINUX__
    JackProcessCallback callback;
#endif

    void* lib_handle;
    
////////////////////////////////////////////////////////////////////
// Fonctions d'info
////////////////////////////////////////////////////////////////////
    
    void logModuleLoadedInfos() const;
};

extern "C"
struct EffectUnit
{
#ifdef __ARCH_LINUX__
////////////////////////////////////////////////////////////////////
// Attributs Jack
////////////////////////////////////////////////////////////////////
    
    jack_client_t* client;
    
    inline void registerAudioInput(std::string name)
    {
        this->audioIns.push_back(Port{name, sfx::registerAudioInput(client, name)
            });
    }
    inline void registerAudioOutput(std::string name)
    {
        this->audioOuts.push_back(Port{name, sfx::registerAudioOutput(client, name)
            });
    }
    inline void registerMidiInput(std::string name)
    {
        this->midiIns.push_back(Port{name, sfx::registerMidiInput(client, name)
            });
    }
    inline void registerMidiOutput(std::string name)
    {
        this->midiOuts.push_back(Port{name, sfx::registerMidiOutput(client, name)
            });
    }
    
    /**
     * Fonction à appeller dans la boucle de process pour traiter les CC midi et 
     *  déclancher les slots eventuels
     * @param event pointeur vers le buffer de l'evenement midi 
     */
    void veryfyAndComputeCCMessage(sfx::hex_t* event);
#else
    
    inline void registerAudioInput(std::string name)
    {
        this->audioIns.push_back(Port{name});
    }
    inline void registerAudioOutput(std::string name)
    {
        this->audioOuts.push_back(Port{name});
    }
    inline void registerMidiInput(std::string name)
    {
        this->midiIns.push_back(Port{name});
    }
    inline void registerMidiOutput(std::string name)
    {
        this->midiOuts.push_back(Port{name});
    }
    
#endif
    
////////////////////////////////////////////////////////////////////
// Attributs Communs
////////////////////////////////////////////////////////////////////
    
    EffectUnit(std::shared_ptr<Module> module);
    ~EffectUnit();
    
    std::string name;
    
    void* datas;
    
    std::shared_ptr<Module> module;
    
////////////////////////////////////////////////////////////////////
// Attributs Communs
////////////////////////////////////////////////////////////////////
    
    /**
     * Structure utilisée pour representer un Port
     */
    struct Port
    {
        std::string name;
#ifdef __ARCH_LINUX__
        jack_port_t* port;
#endif
    };
    /**
     * Liste des Ports d'un Effet
     */
    typedef std::vector<Port> PortArray;
    
    PortArray audioIns, audioOuts, midiIns, midiOuts;
    
////////////////////////////////////////////////////////////////////
// Gestion des Slots
////////////////////////////////////////////////////////////////////
    
    static const int MISSING_SLOT   = -1;
    static const int DISABLED_SLOT  = 1;
    
    struct SlotStatus
    {
        std::shared_ptr<Module::Slot> slot;
        sfx::hex_t value;
        
        enum
        {
            ENABLED=0,DISABLED
        } status;
        
        inline bool operator!() const { return status!=ENABLED; };
    };
    
    typedef std::map<sfx::hex_pair_t, std::vector<std::string>> LinkTable;
    LinkTable links;
    
    typedef std::map<std::string, SlotStatus> SlotStatusTable;
    SlotStatusTable slots;
    
    /**
     * Fonction utilisée pour connecter un slot à un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom interne du slot à connecter
     * @return 0 on success, MISSING_SLOT is slot doesn't exist
     */
    int linkSlot(sfx::hex_pair_t cc, std::string slot);
    /**
     * Fonction utilisée pour déconnecter un slot à d'un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom interne du slot à connecter
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, 1 if link doesn't exist
     */
    int unlinkSlot(sfx::hex_pair_t cc, std::string slot);
    
    /**
     * Fonction utilisée pour activer un slot
     * @param slot le nom interne du slot à activer
     * @return 0 on success, MISSING_SLOT if slot doesn't exist
     */
    int enableSlot(std::string slot);
    /**
     * Fonction utilisée pour désactiver un slot
     * @param slot le nom interne du slot à désactiver
     * @return 0 on success, MISSING_SLOT if slot doesn't exist
     */
    int disableSlot(std::string slot);
    
    /**
     * Fonction utiliée pour changer la valeur d'un slot
     *  Si le slot est désactivé la fonction est sans effet
     * @param slot nom interne du slot à changer
     * @param value valleur à assigner au slot (sur 7bit)
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
     */
    int setSlotValue(std::string slot, sfx::hex_t value, bool force=false);
    /**
     * Fonction utiliée pour récuperer la valeur interne d'un slot
     *  Si le slot est désactivé la fonction est sans effet.
     *  laisser à nullptr les valeurs non souhaitées
     * @param slot nom interne du slot ciblé
     * @param x_result pointeur pour récuperer la valeur sur 7bit du slot
     * @param f_result pointeur pour récuperer la valeur interne du slot
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
     */
    int getSlotValue(std::string slot, sfx::hex_t* x_result, float* f_result);
    
    void logLinkedSlots() const;
    
////////////////////////////////////////////////////////////////////
// Gestion des Banques
////////////////////////////////////////////////////////////////////
    
    static const int MISSING_BANK   = -1;
    static const int EXISTING_BANK  = 1;
    
    typedef std::map<std::string,sfx::hex_t>    ParamArray;
    typedef std::map<sfx::hex_t, ParamArray>    BankTable;
    typedef std::list<sfx::hex_t>               BankIdList;
    
    BankTable  banks;
    BankIdList banks_order;
    
    BankIdList::iterator current_bank;
    
    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    int setBank(sfx::hex_t id);
    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    void nextBank();
    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    void prevBank();
    
    /**
     * Fonction interne utilisée pour mettre à jour l'effet en appelant tout les slots
     *  actifs de la banque courante
     * @pre current_bank bank is valid
     */
    void updateCurrentBank();
    
    /**
     * @brief method to add a bank to the effect
     *  Usage :
     *      createBank() to create a bank filled with default values
     *      createBank(id, size, vals) to create a bank from a valid array of values
     * @pre if specified, bank is a valid expansion of effect's config tree
     */
    int createBank(sfx::hex_t id);
    /**
     * @brief method to duplicate a bank of the effect
     *  Usage :
     *      copyBank(sid) to copy bank[sid] to a new one
     *      copyBank(sid, tid) to copy bank[sid] in bank[tid]
     * @pre sid != tid
     */
    int copyBank(sfx::hex_t sid, sfx::hex_t tid);
    /**
     * @brief Method used to remove a bank from the effect
     *  if the bank removed is the last one, automaticaly regenerate the default bank
     * @pre _currentBank bank is valid
     * @post _currentBank bank is valid
     */
    int removeBank(sfx::hex_t id);
};

#endif /* MODULEBASE_H */
