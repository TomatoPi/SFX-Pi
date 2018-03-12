/**********************************************************************
 * @file Const.h
 * @author TomatoPi
 * @version 1.0
 *
 * Constantes generales a tout le programme
 **********************************************************************/
#ifndef DEF_CONST_H
#define DEF_CONST_H

#include <jack/jack.h>
#include <string>
#include <iostream>

/**
 * namepace principal du programme
 **/
namespace SFXP{

    using namespace std;

    /**
     * ID sur 2 octet, compris entre 0x0000 et 0xffff
     **/
    typedef unsigned short int id1_t;
    const id1_t ErrId1 = 0xffff;

    /**
     * ID sur 4 octets, compris entre 0x00000000 et 0xffffffff
     **/
    typedef unsigned int id2_t;
    const id2_t ErrId2 = 0xffffffff;

    /**
     * TypeCode des effets, valeur comprise entre 0 et 65536
     **/
    typedef unsigned int tc_t;
    const tc_t ErrTypeCode = 0xffffffff;

    /**
     * Valeur Hexadecimale sur 1 octet
     **/
    typedef unsigned char hex_t;

    /**
     * Diminutif pour les samples de JACK
     **/
    typedef jack_default_audio_sample_t sample_t;

    /**
     * Flag utilisé pour baliser les fichiers binaires
     **/
    typedef unsigned char flag_t;

    /**
     * Uniformise le type size_t qui change entre Archlinux et Lubuntu
     **/
    typedef unsigned long usize_t;

    /**
     * Dossiers et fichiers par default
     **/
    const string DIR_CONF = "config/";
    const string DIR_PLUG = "effects/";
    const string DIR_PRST = "preset/";

    const string VERSION = "2.0.0";
    const string BUILD   = string(__DATE__) + "   " + string(__TIME__);

    /**
     * Serveur Jack Par default
     **/
    const string JACK_SERVER = "Space_Fx";

    /**
     * IO ID type specifiers
     **/
    const id2_t ID_MASK = 0xff00;
    
    const id2_t ID_FOOT = 0x0100;
    const id2_t ID_LED  = 0x0200;
    const id2_t ID_SEQ  = 0x0400;
    const id2_t ID_POT  = 0x0800;
    const id2_t ID_DISP = 0x1000;

    /**
     * Variables Globales
     **/
    extern bool GlobalIsDebugEnabled;
    extern bool GlobalNeedToExit;
    extern int  GlobalErrState;
}

namespace log{

    static inline std::ostream& source(const std::string& s) {

        return std::cout << "[" << s << "]";
    }

    static inline std::ostream& log(const std::string& s) {

        return source(s) << " : INFO : ";
    }

    static inline std::ostream& wrn(const std::string& s) {

        return source(s) << " : WARNING : ";
    }

    static inline std::ostream& err(const std::string& s) {

        return source(s) << " : ERROR : ";
    }
};

#endif
