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

Module::Module(std::shared_ptr<Info> infos):
    name(infos->infos.name),
        datas(nullptr),
        
        audioIns(), audioOuts(), midiIns(), midiOuts(),
        
        infos(infos)
{
#ifdef __ARCH_LINUX__
    // Construction du client Jack
    client = sfx::openJackClient(name);
    jack_set_process_callback(client, infos->callback, this);
    // Allocation des Ports
    midiIns.push_back(Port{"Modulation"
        , sfx::registerMidiInput(client, "Modulation")});
    midiOuts.push_back(Port{"Mod_Trougth"
        , sfx::registerMidiOutput(client, "Mod_Trougth")});
#else
    // Allocation des Ports
    midiIns.push_back(Port{"Modulation"});
    midiOuts.push_back(Port{"Mod_Trougth"});
#endif
    
    // Construction des datas du Module et allocation des ports specifiques
    datas = infos->builder(this);
    
#ifdef __ARCH_LINUX__
    sfx::activateClient(client);
#endif
}
Module::~Module()
{
#ifdef __ARCH_LINUX__
    jack_client_close(client);
#endif
    infos->destructor(datas);
}

/**
 * Fonction utilisée pour connecter un slot à un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom du slot à connecter
 * @return 0 on success, -1 is slot doesn't exist, 1 if link already exist
 */
int Module::linkSlot(sfx::hex_pair_t cc, std::string slot)
{
    
    if (infos->slots.find(slot) == infos->slots.end())
        return -1;
    
    if (links.find(cc) == links.end())
        links[cc] = std::set<std::string>();
    
    if (links[cc].find(slot) != links[cc].end())
        return 1;
    
    links[cc].insert(slot);
    return 0;
}
/**
 * Fonction utilisée pour déconnecter un slot à d'un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom du slot à connecter
 * @return 0 on success, -1 is slot doesn't exist, 1 if link doesn't exist
 */
int Module::unlinkSlot(sfx::hex_pair_t cc, std::string slot)
{
    if (infos->slots.find(slot) == infos->slots.end())
        return -1;
    
    if (links.find(cc) == links.end())
        return 1;
    
    if (links[cc].find(slot) == links[cc].end())
        return 1;
    
    links[cc].erase(slot);
    if (links[cc].size() == 0) links.erase(cc);
    
    return 0;
}