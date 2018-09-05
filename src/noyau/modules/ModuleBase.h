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
#include <vector>
#include <memory>
#include <set>

#include "noyau/types.h"
#include "noyau/utils.h"
#include "noyau/midi.h"

#ifdef __ARCH_LINUX__
#include "process/module/jackWrapper.h"
#endif

struct Module
{
////////////////////////////////////////////////////////////////////
// Declarations des structures communes
////////////////////////////////////////////////////////////////////
    
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
        typedef float (*slot_f)(sfx::hex_t, Module*);
        
//        Slot():Slot("Empty", 0, nullptr)
//        {
//        }
//        Slot(std::string n, sfx::hex_t d, slot_f s):name(n), value(d),callback(s)
//        {
//        }
        
        std::string name;
        //sfx::hex_t  value;
        
        slot_f callback;
    };
    /**
     * Table stoquant les Slots par nom
     */
    typedef std::map<std::string, Slot> SlotTable;
    typedef std::map<sfx::hex_pair_t, std::set<std::string>> LinkTable;
    /**
     * Prototype de la fonction de déclaration des Slots
     */
    typedef SlotTable (*register_slot_table_f)(void);
    
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
    
    /**
     * Proptotype de la fonction pour creer les datas d'un Module
     */
    typedef void* (*create_module_f)(Module*);
    typedef void (*destroy_module_f)(void*);
    
    struct ShortInfo
    {
        std::string name;
        std::string version;
    };
    typedef ShortInfo (*register_module_info_f)(void);
    
    struct Info
    {
        ShortInfo infos;
    
        Module::create_module_f builder;
        Module::destroy_module_f destructor;

        Module::SlotTable slots;
        
#ifdef __ARCH_LINUX__
        JackProcessCallback callback;
#endif

        void* lib_handle;
    };
    
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
    
    inline void veryfyAndComputeCCMessage(sfx::hex_t* event)
    {
        sfx::hex_t ccc = sfx::Midi_read_CCChannel(event);
        sfx::hex_t ccs = sfx::Midi_read_CCSource(event);
        sfx::hex_pair_t cc = std::make_pair(ccc, ccs);
        
        //sfx::debug("Native-MIDI", "Process Midi Event : %i %i\n", ccc, ccs);
        
        if (links.find(cc) != links.end())
        {
            for (auto& slot : links[cc])
                if (infos->slots.find(slot) != infos->slots.end())
                {
                    //sfx::debug("Native-MIDI", "Call slot : \"%s\" with value : %i\n", infos->slots[slot].name, sfx::Midi_read_CCValue(event));
                    float v = (*infos->slots[slot].callback)(sfx::Midi_read_CCValue(event), this);
                    sfx::debug("Native-MIDI", "Called slot : \"%s\" with value : %i => %f\n", slot, sfx::Midi_read_CCValue(event), v);
                }
                else
                {
                    //sfx::wrn("Native-MIDI", "Unable to find slot : \"%s\" linked to CC : %i %i\n", slot, ccc, ccs);
                }
        }
    }
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
    
    std::string name;
    
    void* datas;
    
    std::vector<Port> audioIns, audioOuts, midiIns, midiOuts;
    
    std::shared_ptr<Info> infos;
    
    LinkTable links;
    
////////////////////////////////////////////////////////////////////
// Fonctions Membres
////////////////////////////////////////////////////////////////////
    
    Module(std::shared_ptr<Info> infos);
    ~Module();
    
    /**
     * Fonction utilisée pour connecter un slot à un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom du slot à connecter
     * @return 0 on success, -1 is slot doesn't exist, 1 if link already exist
     */
    int linkSlot(sfx::hex_pair_t cc, std::string slot);
    /**
     * Fonction utilisée pour déconnecter un slot à d'un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom du slot à connecter
     * @return 0 on success, -1 is slot doesn't exist, 1 if link doesn't exist
     */
    int unlinkSlot(sfx::hex_pair_t cc, std::string slot);
};

#endif /* MODULEBASE_H */
