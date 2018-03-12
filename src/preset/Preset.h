/**********************************************************************
 * @file Preset.h
 * @author TomatoPi
 * @version 1.1
 *
 * File provididing Preset Object, storing all informations loaded from
 *  a preset file
 **********************************************************************/
#ifndef DEF_PRESET_H
#define DEF_PRESET_H

#include <set>
#include <map>
#include <vector>

#include "effect/EffectParamSet.h"
#include "core/Connection.h"

#include "event/EventSequencer.h"

#include "analog/AnalogLink.h"

#include "types.h"

struct Preset {
    
        Preset(){
            for (SFXP::usize_t i = 0; i < SFXP::MAX_POT; i++)
                _links[i] = nullptr;
        }
        ~Preset(){

            for (auto& p : _paramSets) delete p.second;
            for (auto& s : _sequencers) delete s.second;
            for (SFXP::usize_t i = 0; i < SFXP::MAX_POT; i++)
            {
                if (_links[i]) {

                    for (auto& l : (*_links[i])) delete l;
                    delete _links[i];
                }
            }
        }

        typedef std::map<SFXP::id1_t,ParamSet*> ParamSetList;
        typedef std::set<Connection> ConnectionList;
        
        typedef std::map<SFXP::id2_t,EventSequencer*> SequencerList;

        typedef std::vector<AnalogLink*> ALinkList;

        /** List of param sets **/
        ParamSetList _paramSets;

        /** List of Jack Connections **/
        ConnectionList _connections;

        /** List of Event Sequencers **/
        SequencerList _sequencers;

        /** List of Potentiometers **/
        ALinkList* _links[SFXP::MAX_POT];
};

#endif
