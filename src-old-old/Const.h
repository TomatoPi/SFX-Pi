/**********************************************************************
 * @file Const.h
 * @author TomatoPi
 * @version 1.0
 *
 * Programm Constants
 **********************************************************************/
#ifndef DEF_CONST_H
#define DEF_CONST_H

#include "./core/Typedefs.h"

#include <stdint.h>
#include <string>

namespace SFXP{

    /*
     * Program consts
     */
    const std::string PRESET_PATH   = "files/";
    const id1_t VERSION           = 0x02;

    const usize_t JACK_BLOCK_SIZE = 8;
    const std::string JACK_SERVER = "Space_Fx";

    /*
     * Effects Type Codes
     */
    const id1_t TC_DRIVE  = 0;
    const id1_t TC_MDRIVE = 1;
    
    const id1_t TC_FFTC       = 2;
    const id1_t TC_FFTR       = 3;
    const id1_t TC_FFTFILTER  = 4;
    const id1_t TC_FFTDELAY   = 5;

    const id1_t TC_DELAY      = 6;
    const id1_t TC_REVERB     = 8;
    const id1_t TC_CHORUS     = 9;

    const id1_t TC_LFO        = 7;

    const id1_t TC_CAPTURE  = 254;
    const id1_t TC_PLAYBACK = 255;

    /*
     * IO ID type specifiers
     **/
    const id2_t ID_MASK = 0xff00;
    
    const id2_t ID_FOOT = 0x0100;
    const id2_t ID_LED  = 0x0200;
    const id2_t ID_SEQ  = 0x0400;
    const id2_t ID_POT  = 0x0800;
    const id2_t ID_DISP = 0x1000;

    /*
     * Global Vars
     */
    extern bool GlobalNeedToExit;
    extern bool GlobalIsDebugEnabled;
};

#endif
