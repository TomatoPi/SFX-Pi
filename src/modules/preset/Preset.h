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

#include "../ProcessGraph.h"
#include "../UnitFactory.h"

#include "../../Const.h"

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
         **/
        static AbstractEffectUnit* parseEffectUnit(std::ifstream& flux);
        static int writeEffectUnit(std::ofstream& flux, AbstractEffectUnit* unit);

        static ProcessGraph::Connection parseConnection(std::ifstream& flux);
        static int writeConnection(std::ofstream& flux, ProcessGraph::Connection c);
};

#endif
