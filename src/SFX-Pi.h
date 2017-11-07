/**********************************************************************
 * @file SFX-Pi.h
 * @author TomatoPi
 * @version 1.0
 *
 * Main
 **********************************************************************/
#ifndef DEF_SFX_PI_H
#define DEF_SFX_PI_H

#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Const.h"

// CORE
#include "./core/Parser.h"
#include "./core/Utility.h"

// Include Effects
#include "./modules/UnitFactory.h"
#include "./modules/ProcessGraph.h"
#include "./modules/ProcessGraphCommands.h"

#include "./modules/preset/Preset.h"
#include "./modules/preset/PresetCommands.h"

#include "./modules/effects/List.h"

// Include IO
#include "./io/command/CommandListener.h"
#include "./io/command/CommandParser.h"
#include "./io/command/Commands.h"

#endif
