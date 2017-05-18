#ifndef DEF_PRESETS_H
#define DEF_PRESETS_H
#include <iostream>
#include <fstream>

#include <json/json.h>

#include "Modules.h"
#include "Utility.h"

#include "../modules/Drive.h"
#include "../modules/LFO.h"

/*
* Load drive module from JSON formated file
*/
int sav_LoadDrive(const char *file, Drive *drive);

/*
* Load tripole filter from JSON
*/
int sav_LoadTripole(const char *file, spi_tripole *tripole);

#endif
