/**********************************************************************
 * @file Preset.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset saving stuff
 **********************************************************************/
#ifndef DEF_PRESET_H
#define DEF_PRESET_H

#include <fstream>
#include <iostream>
#include <string>

#include "../../Const.h"

#include "../ProcessGraph.h"
#include "../UnitFactory.h"

#include "../../io/UIManager.h"

class Preset{

    public :

        static int loadPreset(std::string file, bool bak=false);
        static int savePreset(std::string file);

        static AbstractEffectUnit* loadEffect(std::string file);
        static int saveEffect(std::string file, AbstractEffectUnit* unit);

    private :
        
        /**
         * Parse an effect unit from a binary file
         * return a pointer to created effect unit if creation is successfull
         * null pointer if not
         * 
         * Binary format is :
         * 
         * >>EffectBeginFlag    ( flag_t )
         * >>Effect Type Code   ( id1_t )
         * >>Effect ID          ( id1_t )
         * >>Number of Banks    ( usize_t : must be changed to platform independant type )
         * >>Size of a Bank     ( usize_t )
         * 
         * For each Bank :
         *   >>BankBeginFlag      ( flag_t )
         *   >>Bank's ID          ( id1_t )
         *   >>All Banks Params   ( Size of a bank : float )
         *   >>BankEndFlag        ( flag_t )
         * 
         * >>EffectEndFlag      ( flag_t )
         **/
        static AbstractEffectUnit* parseEffectUnit(std::ifstream& flux);
        static int writeEffectUnit(std::ofstream& flux, AbstractEffectUnit* unit);

        /**
         * Parse a connection from a binary file
         * Return parsed connection if success
         * 255 255 255 255 if not
         *
         * Binary format is :
         *
         * >>ConnectionBeginFlag    ( flag_t )
         * >>ConnectionSourceID     ( id1_t )
         * >>ConnectionSourcePort   ( id1_t )
         * >>ConnectionTargetID     ( id1_t )
         * >>ConectionTargetPort    ( id1_t )
         * >>ConnectionEndFlag      ( flag_t )
         **/
        static ProcessGraph::Connection parseConnection(std::ifstream& flux);
        static int writeConnection(std::ofstream& flux, ProcessGraph::Connection c);

        /**
         * Parse a Footswitch Configuration form a binary file
         * Return the parsed Footswitch
         * Null if parsing has failed
         **/
        static FootSwitch* parseFootSwitch(std::ifstream& flux);
        static int writeFootSwitch(std::ofstream& flux, FootSwitch *fswitch);

        /**
         * Parse a Command Sequencer from a binary file
         * Return Parsed Command Sequencer
         * Null if parsing has failed
         *
         * Binary format is :
         *
         * >>SequencerBeginFlag     ( flag_t )
         * >>Sequencer ID           ( id2_t )
         * >>Number Of Links        ( id2_t )
         *
         * For each Link :
         *   >>LinkBeginFlag        ( flag_t )
         *   >>Notification Source  ( id2_t )
         *   >>Notification Type    ( id1_t )
         *   >>Action Type          ( id1_t )
         *   >>Action Argument      ( id2_t )
         *   >>LinkEndFlag          ( flag_t )
         *
         * >>Number of Sequencies   ( usize_t )
         *
         * For each Sequence :
         *   >>SequenceBeginFlag    ( flag_t )
         *   >>Sequence ID          ( id2_t )
         *   >>Sequence Size        ( usize_t )
         *   For each Command :
         *     >>Command Size       ( usize_t )
         *     >>Command            ( std::string : Command Size )
         *   >>SequenceEndFlag      ( flag_t )
         *
         * >>SequencerEndFlag       ( flag_t )
         **/
        static CommandSequencer* parseSequencer(std::ifstream& flux);
        static int writeSequencer(std::ofstream& flux, CommandSequencer *seq);
};

#endif
