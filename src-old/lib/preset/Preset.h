/**********************************************************************
 * @file Preset.h
 * @author TomatoPi
 * @version 1.0
 *
 * File provididing Preset Object, storing all informations loaded from
 *  a preset file
 **********************************************************************/
#ifndef DEF_PRESET_H
#define DEF_PRESET_H

#include <set>
#include <map>

#include "lib/unit/AbstractEffectUnit.h"
#include "lib/graph/Connection.h"

#include "Const.h"

class Preset {

    public :

        typedef std::map<SFXP::id1_t,AbstractEffectUnit*> EffectList;
        typedef std::set<Connection> ConnectionList;/*
        typedef std::map<SFXP::id2_t,CommandSequencer*> SequencerList;
        typedef std::map<SFXP::id2_t,FootSwitch*> FootArray;*/

        Preset();
        ~Preset();

        /**
         * Add and remove an effect from preset
         **/
        int addEffect(AbstractEffectUnit* e);
        int removeEffect(SFXP::id1_t id);
        EffectList getEffectList() const;

        /**
         * Add and remove an effect from preset
         **/
        int addConnection(Connection c);
        int removeConnection(Connection c);
        ConnectionList getConnectionList() const;

        /**
         * Add and remove a command sequencer from preset
         **//*
        int addSequencer(CommandSequencer* s);
        int removeSequencer(SFXP::id2_t id);
        SequencerList getSequencerList() const;*/

        /**
         * Add and remove a Footswitch from preset
         **//*
        int addFootSwitch(FootSwitch* f);
        int removeFootSwitch(SFXP::id2_t id);
        FootArray getFootSwitchList() const;*/

    protected :

        /** Store List of Effects **/
        EffectList m_effectList;

        /** Store List of Connections **/
        ConnectionList m_connectionList;

        /** Store list of Command Sequencer **/
        //SequencerList m_seqList;
        
        /** Store list of Footswitches **/
        //static FootArray m_footArray;
};

#endif
