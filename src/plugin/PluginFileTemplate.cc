/**********************************************************************
 * @file PLUGIN_NAME.cc
 * @author TomatoPi
 * @version 1.0
 *
 **********************************************************************/
#include "PLUGIN_NAME.h"

#define POOL_SIZE
#define PARAM_COUNT
#define PORTS buildPortsNames()

namespace {

    const Plugin::ParamConfig PARAMS[PARAM_COUNT] =
    {
    };

    const SFXP::usize_t PORTCOUNT[4] = {0, 0, 0, 0};

    const std::string** buildPortsNames() {
        
        std::string** ports = new std::string*[4];
        for (int i = 0; i < 4; i++) {

            ports[i] = new std::string[PORTCOUNT[i]];
        }
        ports[0][0] = "";
        ports[1][0] = "";
        ports[2][0] = "";
        ports[3][0] = "";

        return (const std::string**)ports;
    }
}

PLUGIN_INTERFACE_DECLARATIONS(PLUGIN_NAME, PLUGIN_EFFECT_NAME)
