/**********************************************************************
 * @file DrivePlugin.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Basic Distortion Effect Plugin file
 **********************************************************************/
#include "DrivePlugin.h"

#define GRP1 "General"
#define GRP2 "Positive Half-Wave"
#define GRP3 "Negative Half-Wave"
#define GRP4 "Input EQ"
#define GRP5 "Output EQ"

#define POOL_SIZE 21
#define PARAM_COUNT 21
#define PORTS buildPortsNames()

namespace {

    const Plugin::ParamConfig PARAMS[PARAM_COUNT] =
    {
        Plugin::ParamConfig(GRP1, "Bypass", "Bypassed", "Activated")
        ,Plugin::ParamConfig(GRP1, "Volume", 0, 1)
        ,Plugin::ParamConfig(GRP1, "Asymetric", "Asymetric", "Symetric")

        ,Plugin::ParamConfig(GRP2, "Gain")
        ,Plugin::ParamConfig(GRP2, "Type", "HardClip", "SoftClip")
        ,Plugin::ParamConfig(GRP2, "Softness")
        ,Plugin::ParamConfig(GRP2, "Shape", 0, 1)

        ,Plugin::ParamConfig(GRP3, "Gain")
        ,Plugin::ParamConfig(GRP3, "Type", "HardClip", "SoftClip")
        ,Plugin::ParamConfig(GRP3, "Softness")
        ,Plugin::ParamConfig(GRP3, "Shape", 0, 1)

        ,Plugin::ParamConfig(GRP4, "Lowcut")
        ,Plugin::ParamConfig(GRP4, "Highcut")
        ,Plugin::ParamConfig(GRP4, "Lowgain")
        ,Plugin::ParamConfig(GRP4, "Midgain")
        ,Plugin::ParamConfig(GRP4, "Highgain")

        ,Plugin::ParamConfig(GRP5, "Lowcut")
        ,Plugin::ParamConfig(GRP5, "Highcut")
        ,Plugin::ParamConfig(GRP5, "Lowgain")
        ,Plugin::ParamConfig(GRP5, "Midgain")
        ,Plugin::ParamConfig(GRP5, "Highgain")
    };

    const SFXP::usize_t PORTCOUNT[4] = {1, 1, 0, 0};

    const std::string** buildPortsNames() {
        
        std::string** ports = new std::string*[4];
        for (int i = 0; i < 4; i++) {

            ports[i] = new std::string[PORTCOUNT[i]];
        }
        ports[0][0] = "AudioIn";
        ports[1][0] = "AudioOut";

        return (const std::string**)ports;
    }
}

PLUGIN_INTERFACE_DECLARATIONS(DrivePlugin, DriveEffect)
