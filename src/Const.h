/**********************************************************************
 * @file Const.h
 * @author TomatoPi
 * @version 1.0
 *
 * Programm Constants
 **********************************************************************/
#ifndef DEF_CONST_H
#define DEF_CONST_H

namespace SFXP{

    const std::string PRESET_PATH   = "./files/";
    const uint8_t VERSION           = 1;

    const uint8_t TC_DRIVE  = 0;
    const uint8_t TC_MDRIVE = 1;
    
    const uint8_t TC_FFTC       = 2;
    const uint8_t TC_FFTR       = 3;
    const uint8_t TC_FFTFILTER  = 4;
    const uint8_t TC_FFTDELAY   = 5;

    const uint8_t TC_DELAY      = 6;

    const uint8_t TC_LFO        = 7;

    const uint8_t TC_CAPTURE  = 254;
    const uint8_t TC_PLAYBACK = 255;

    extern bool GlobalNeedToExit;
    extern bool GlobalIsDebugEnabled;

    const size_t JACK_BLOCK_SIZE = 8;
};

#endif
