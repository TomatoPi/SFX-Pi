#ifndef DEF_PRESSET_H
#define DEF_PRESSET_H

#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <dirent.h>

#include "../core/Modules.h"
#include "../core/ModulesGraph.h"

#include "../io/Potar.h"
#include "../io/Button.h"

using namespace std;
/*
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*                           Preset Managing stuff
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*/

/*
*   ----------------------File identification keys-----------------------------
*/

#define MAX_LINE_LENGTH 256
#define MAX_TOKEN_COUNT 256

/**
*   Function used to save curent process graph inside a Presset file.
*   Preset is saved with format :
*
*   NODE_ROOT_PRESET "program_version" "preset_name" "modules's_count"
*   NODE_MODULE "mode_type_code" "number_of_params" "Module's volume"
*       NODE_PARAM "number_of_params" "list_of_params"
*       ----------
*       NODE_ACCESSOR "module_target" "accesor's_params"
*       -------------
*   -----------
*        ----------
*   NODE_CONNECTION "source" "port" "target" "port"
*   ---------------
*   NODE_POTENTIOMETER "potentiometer_name" "value_range"
*   ------------------
*   NODE_BUTTON "button's_params"
*   -----------
*
*   @param name file name
*   @param version curent program version
*   @param list curent process graph
*   @param pot list of potentiometers
*   @return 0 on success
*/
int save_preset(    string const name,
                    IO_Potentiometer pot[SPI_POTAR_COUNT]
                    );
/**
*   Function used to load a preset file.
*   First verify that file is a preset file by reading it first node
*   Then verify that file's version correspond to curent program's
*   If Root node or program version do not match it abort preset load
*   If file is valid, it clear current graph and reload it
*   @see save_preset(string, string, Module_Node_List)
*
*   @param name file name
*   @param version curent program version
*   @param list pointer to main process graph
*   @return 0 on success
*
*   @warning if presset loaded successful it will cause a brief sound's interuption
*/
int load_preset(string const name, IO_Potentiometer pot[SPI_POTAR_COUNT] );

/**
*   Function used for create new empty presset file
*
*   @param name file name
*   @param version current program version
*   @return 0 on success
*/
int new_preset(string const name);

/**
*   Function used to backup a single module's params and banks.
*   NODE_ROOT_MODULE "program_version" "module_type"
*       NODE_PARAM "number_of_params" "list_of_params"
*       ----------
*
*   @param name file name
*   @param version current program version
*   @param mod module to save
*   @return 0 on success
*/
int save_module(string const name, Module* mod);

/**
*   Function used to load a single module's banks
*   It verify that file passed is a module file
*   Then verify program version and if given module is the same type as file's one
*   If file is valid, delete all module's banks, and load new ones
*
*   @param name file name
*   @param version current program version
*   @param mod module to save
*   @param del delete file after successful load if true
*   @return 0 on success
*/
int load_module(string const name, Module* mod, bool del);

/**
*   Function used to list all avaiable files in givn directory
*
*   @param dir directory path
*   @param ext file extention
*   @param list vector that store file names
*
*   @return 0 if returned non empty list
*/
int list_files(string const dir, vector<string> & list, bool log = false);

#endif
