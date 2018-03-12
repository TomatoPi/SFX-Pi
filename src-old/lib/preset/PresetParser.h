/**********************************************************************
 * @file PresetParser.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to read and write preset finary files
 **********************************************************************/
#ifndef DEF_PRESET_PARSER_H
#define DEF_PRESET_PARSER_H

#include <fstream>
#include <iostream>
#include <string>

#include "lib/unit/EffectPrototype.h"
#include "lib/graph/Connection.h"

#include "lib/io/logic/FootSwitch.h"

#include "lib/preset/Preset.h"

#include "Const.h"

namespace PresetParser {

    using namespace SFXP;

    /**
     * PresetParser Version
     **/
    const std::string ParserVersion = "1.0.0";

    /**
     * Funcions to write and read datas to a binary stream
     **/
    template<typename T>
    inline void write(std::ofstream& flux, T bits){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write : %0*x\n", sizeof(bits), bits);
            
        flux.write((char*)&bits, sizeof(T));
    }
    template<typename T>
    inline T read(std::ifstream& flux){

        T bits;
        flux.read((char*)&bits, sizeof(T));

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed : %0*x\n", sizeof(bits), bits);
            
        return bits;
    }

    template<>
    inline void write<std::string>(std::ofstream& flux, std::string text){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write String : %s\n", text.c_str());

        write<usize_t>(flux, text.size());
        flux.write(text.c_str(), text.size());
    }

    template<>
    inline std::string read<std::string>(std::ifstream& flux){

        usize_t size = read<usize_t>(flux);
        char buffer[size];
        flux.read(buffer, size);
        
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed String : %s\n", buffer);

        return std::string(buffer);
    }

    /**
     * Function to get a control value from given stream
     * @throw ErrString if value readed differs from targetValue
     **/
    template<typename T>
    inline void controlValue(std::ifstream& flux, T targetValue, std::string ErrString) {

        T value = read<T>(flux);
        if ( value != targetValue ){

            std::string err = ErrString;
            err += ":";
            err += value;
            
            throw err;
        }
    }

    /**
     * Function to read and write a Preset File
     **/
    int savePreset(std::string file, Preset* preset);
    Preset* loadPreset(std::string files);

    /**
     * Function to read and write an Effect File
     **/
    int saveEffectPrototype(std::string file, EffectPrototype* effect);
    EffectPrototype* loadEffectPrototype(std::string file);

    /**
     * Function to write File's header
     * Write Parser version, and Program Version and file's type
     **/
    void writeHeader(std::ofstream& flux, const std::string& parser, const std::string& program, flag_t type);
    /**
     * Function to read File's header
     * Verify that parser version and type flag match
     * return 0 is header is valid
     **/
    int readHeader(std::ifstream& flux, const std::string& parser, const std::string& program, flag_t type);

    /**
     * Parse an EffectPrototype from a binary file
     * return a pointer to created effect if creation is successfull
     * null pointer if not
     * 
     * Binary format is :
     * 
     * >>EffectBeginFlag    ( flag_t )
     * >>Effect Type Code   ( tc_t )
     * >>Effect ID          ( id1_t )
     * >>Number of Banks    ( usize_t )
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
    EffectPrototype* readEffectPrototype(std::ifstream& flux);
    int writeEffectPrototype(std::ofstream& flux, EffectPrototype* effect);
    
    /**
     * Parse a connection from a binary file
     * Return parsed connection if success
     * connection with source and target id to ErrId1 if not
     *
     * Binary format is :
     *
     * >>ConnectionBeginFlag    ( flag_t )
     *  >>ConnectionSourceID     ( id1_t )
     *  >>ConnectionSourcePort   ( usize_t )
     *  >>ConnectionTargetID     ( id1_t )
     *  >>ConectionTargetPort    ( usize_t )
     *  >>ConnectionType         ( bool )
     * >>ConnectionEndFlag      ( flag_t )
     **/
    Connection readConnection(std::ifstream& flux);
    int writeConnection(std::ofstream& flux, Connection c);

    /**
     * Parse a Footswitch Configuration form a binary file
     * Return the parsed Footswitch
     * Null if parsing has failed
     **/
    FootSwitch* readFootSwitch(std::ifstream& flux);
    int writeFootSwitch(std::ofstream& flux, FootSwitch *fswitch);

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
    CommandSequencer* readSequencer(std::ifstream& flux);
    int writeSequencer(std::ofstream& flux, CommandSequencer *seq);
};

#endif
