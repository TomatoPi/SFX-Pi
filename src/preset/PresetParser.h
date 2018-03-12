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

#include "preset/Preset.h"

#include "types.h"
#include "log.h"

namespace PresetParser {

    using namespace SFXP;

    /**
     * PresetParser Version
     **/
    const std::string ParserVersion = "1.1.2";

    /**
     * Funcions to write and read datas to a binary stream
     **/
    template<typename T>
    inline void write(std::ofstream& flux, T bits){

        /*
        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write : %0*x\n", sizeof(bits), bits);
        #endif
        * */
        
        flux.write((char*)&bits, sizeof(T));
    }
    template<typename T>
    inline T read(std::ifstream& flux){

        T bits;
        flux.read((char*)&bits, sizeof(T));

        /*
        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed : %0*x\n", sizeof(bits), bits);
        #endif
        * */
            
        return bits;
    }

    template<>
    inline void write<std::string>(std::ofstream& flux, std::string text){

        /*
        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write String : %s\n", text.c_str());
        #endif
        * */
        
        write<usize_t>(flux, text.size());
        flux.write(text.c_str(), text.size());
    }

    template<>
    inline std::string read<std::string>(std::ifstream& flux){

        usize_t size = read<usize_t>(flux);
        char buffer[size+1];
        flux.read(buffer, size);

        // Add a null char at the end of the buffer to ensure correct
        //  cast to string
        buffer[size] = '\0';

        /*
        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed String : %lu:\"%s\"\n", size, buffer);
        #endif
        * */

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
    template<>
    inline void controlValue<std::string>(std::ifstream& flux, std::string targetValue, std::string ErrString) {

        std::string value = read<std::string>(flux);
        if ( value.compare(targetValue) ){

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
     * >>EffectBeginFlag        ( flag_t )
     * >>Effect Type Code       ( tc_t )
     * >>Effect ID              ( id1_t )
     * >>Number of Banks        ( usize_t )
     * >>Size of a Bank         ( usize_t )
     * 
     * For each Bank :
     *   >>BankBeginFlag        ( flag_t )
     *   >>Bank's ID            ( id1_t )
     *   >>All Banks Params     ( Size of a bank : float )
     *   >>BankEndFlag          ( flag_t )
     * 
     * >>EffectEndFlag          ( flag_t )
     **/
    ParamSet* readEffectParamSet(std::ifstream& flux);
    int writeEffectParamSet(std::ofstream& flux, ParamSet* effect);

    /**
     * Parse a connection from a binary file
     * Return parsed connection if success
     * Connection with source and target id to ErrId1 if not
     *
     * Binary format is :
     *
     * >>ConnectionBeginFlag    ( flag_t )
     *  >>ConnectionSourceID    ( id1_t )
     *  >>ConnectionSourcePort  ( usize_t )
     *  >>ConnectionTargetID    ( id1_t )
     *  >>ConectionTargetPort   ( usize_t )
     *  >>ConnectionType        ( bool )
     * >>ConnectionEndFlag      ( flag_t )
     **/
    Connection readConnection(std::ifstream& flux);
    int writeConnection(std::ofstream& flux, Connection c);

    /**
     * Parse an EventSequencer from a binary flux
     * Return parsed sequencer or null pointer if failed
     *
     * Binary format is :
     * 
     * >>SequencerBeginFlag     ( flag_t )
     *  >>Sequencer ID          ( id2_t )
     * 
     *  >>Number of Links       ( usize_t )
     *  For each Link :
     *      >>LinkBeginFlag     ( flag_t )
     *      >>Trigger           ( IOEvent )
     *      >>ActionType        ( flag_t )
     *      >>TargetSeqID       ( id2_t )
     *      >>LinkEndFlag       ( flag_t )
     * 
     *  >>Number of Sequencies  ( usize_t )
     *  For each Sequence :
     *      >>SequenceBeginFlag ( flag_t )
     *      >>Sequence ID       ( id2_t )
     *      >>Sequence Size     ( usize_t )
     *      List of Events :
     *          >>Events        ( SFXPEvent )
     *      >>SequenceEndFlag   ( flag_t )
     * 
     * >>SequencerEndFlag       ( flag_t )
     **/
    EventSequencer* readSequencer(std::ifstream& flux);
    int writeSequencer(std::ofstream& flux, EventSequencer* seq);

    /**
     * Parse an event from a binary flux
     *
     * Binary format is :
     *
     * >>EventBeginFlag         ( flag_t )
     * >>EventType              ( flag_t )
     * >>EventTarget            ( flag_t )
     * >>IOEvent                ( IOEvent )
     * >>PresetEvent            ( PresetEvent )
     * >>EffectEvent            ( EffectEvent )
     * >>GraphEvent             ( GraphEvent )
     * >>EditionEvent           ( EditionEvent )
     * >>EVHEvent               ( EVHEvent )
     * >>EventEndFlag           ( flag_t )
     **/
    SFXPEvent readSFXPEvent(std::ifstream& flux);
    int writeSFXPEvent(std::ofstream& flux, SFXPEvent event);

    /**
     * Parse an IOEvent data field to a binary flux
     *
     * >>Status     ( int )
     * >>Value      ( float )
     * >>ID         ( id2_t )
     **/
    IOEvent readIOEvent(std::ifstream& flux);
    int writeIOEvent(std::ofstream& flux, IOEvent event);

    /**
     * Parse a PresetEvent data field to a binary flux
     *
     * >>File       ( string )
     **/
    PresetEvent readPresetEvent(std::ifstream& flux);
    int writePresetEvent(std::ofstream& flux, PresetEvent event);

    /**
     * Parse an EffectEvent data field to a binary flux
     *
     * >>ID         ( id1_t )
     * >>IDX        ( usize_t )
     * >>Value      ( float )
     **/
    EffectEvent readEffectEvent(std::ifstream& flux);
    int writeEffectEvent(std::ofstream& flux, EffectEvent event);

    /**
     * Parse a GraphEvent data field to a binary flux
     *
     * >>Connection ( Connection )
     **/
    GraphEvent readGraphEvent(std::ifstream& flux);
    int writeGraphEvent(std::ofstream& flux, GraphEvent event);

    /**
     * Parse a EditionEvent data field to a binary flux
     *
     * >>ID         ( id1_t )
     * >>Type       ( tc_t )
     **/
    EditionEvent readEditionEvent(std::ifstream& flux);
    int writeEditionEvent(std::ofstream& flux, EditionEvent event);

    /**
     * Parse an EVHEvent data field to a binary flux
     *
     * >>ID         ( id2_t )
     * >>Trigger    ( id2_t )
     * >>Status     ( int )
     * >>Action     ( int )
     * >>Target     ( id2_t )
     **/
    EVHEvent readEVHEvent(std::ifstream& flux);
    int writeEVHEvent(std::ofstream& flux, EVHEvent event);

    /**
     * Parse an AnalogLink from a binary flux
     *
     * Binary format is :
     *
     * >>AnalogBeginFlag    ( flag_t )
     *  >>Index             ( usize_t )
     *  >>Curve             ( flag_t )
     *  >>Tmin              ( float )
     *  >>Tmax              ( float )
     *  >>IsdB              ( bool )
     *  >>IsRelative        ( bool )
     * >>AnalogEndFlag      ( flag_t )
     **/
    AnalogLink* readAnalogLink(std::ifstream& flux);
    int writeAnalogLink(std::ofstream& flux, AnalogLink* link);
};

#endif
