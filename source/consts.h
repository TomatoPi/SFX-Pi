#ifndef DEF_CONST_H
#define DEF_CONST_H

#include <string>

namespace PROG_CONST{

    /**
     * Misc Constants for program
     */
    const std::string PROG_VERSION = "0.4";
    const std::string PROG_RUN = "0095";
    const std::string PROG_SPLASH_SCREEN = "Space-Fx";
    const std::string PROG_JACK_SERVER = "Space_Fx";

    const std::string ARG_NO_SCREEN = "--no-screen";
    const std::string ARG_FULL_LOG = "--flog";

    extern bool FULL_LOG;

    /**
     * Constants for Presset Managing
     */
    const std::string PATH_PRESSET = "/home/sfx_pi/sfx/Files/";

    /**
     * Modules Type Codes
     */
    const int TC_DRIVE  = 0;
    const int TC_DELAY  = 1;
    const int TC_LFO    = 2;
    const int TC_RING   = 3;
    const int TC_TONE   = 4;
    const int TC_REV    = 5;
    const int TC_CHORUS = 6;
    const int TC_COMP   = 7;

    const int TC_LAST = -55;
    const int TC_BEGIN = -98;

    /**
     * Modules Formated Names
     */
    const std::string MN_DRIVE  = "Drive";
    const std::string MN_DELAY  = "Delay";
    const std::string MN_LFO    = "LFO";
    const std::string MN_RING   = "RingMod";
    const std::string MN_TONE   = "ToneStack";
    const std::string MN_REV    = "Reverb";
    const std::string MN_CHORUS = "Chorus";
    const std::string MN_COMP   = "Compressor";

    const std::string MN_LAST   = "Playback";
    const std::string MN_BEGIN  = "Capture";

};
#endif
