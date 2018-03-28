/**********************************************************************
 * @file types.h
 * @author TomatoPi
 * @version 1.0
 *
 * Typedefs used by the program
 **********************************************************************/
#ifndef DEF_TYPES_H
#define DEF_TYPES_H

#include <jack/jack.h>
#include <string>
#include <limits>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Program's main namespace
 **/
namespace SFXP{

    using namespace std;

    /**
     * ID sur 2 octet, compris entre 0x0000 et 0xffff
     **/
    typedef unsigned short int id1_t;
    const id1_t ErrId1 = numeric_limits<id1_t>::max();

    /**
     * ID sur 4 octets, compris entre 0x00000000 et 0xffffffff
     **/
    typedef unsigned int id2_t;
    const id2_t ErrId2 = numeric_limits<id2_t>::max();

    /**
     * TypeCode des effets, valeur comprise entre 0 et 65536
     **/
    typedef unsigned int tc_t;
    const tc_t ErrTypeCode = numeric_limits<tc_t>::max();

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
     * Masks for IO IDs
     **/
    const id2_t ID_MASK = 0x00ff; /**< Mask to get ID of an input */
    const id2_t ID_FOOT = 0x0100; /**< Mask for Footswitches ids */
    const id2_t ID_LED  = 0x0200; /**< Mask for Leds ids */
    const id2_t ID_POT  = 0x0400; /**< Mask for Potentiometer's id */

    /**
     * IO Constants
     **/
    const usize_t MAX_POT = 8; /**< Maximum count of potentiometers */

    /**
     * Dossiers et fichiers par default
     **/
    const string DIR_CONF = "config/";
    const string DIR_PLUG = "plugins/";
    const string DIR_PRST = "preset/";

    const string FONT_FILE = DIR_CONF+"Ubuntu-R.ttf";

    const string VERSION = "3.0.0";
    const string BUILD   = string(__DATE__) + "   " + string(__TIME__);

    /**
     * Serveur Jack Par default
     **/
    const string JACK_SERVER = "Space_Fx";

    /**
     * Variables Globales
     **/
    #ifdef __DEBUG__
    extern bool GlobalIsDebugEnabled;
    #endif
    extern bool GlobalNeedToExit;
    extern int  GlobalErrState;

    /**
     * Enum of available Ports Type
     **/
    enum PortType{
        AudioIn =0,
        AudioOut=1,
        MidiIn  =2,
        MidiOut =3
    };
}

#ifdef __cplusplus
}
#endif

#endif
