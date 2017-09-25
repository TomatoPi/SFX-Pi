#ifndef DEF_PRESSET_H
#define DEF_PRESSET_H

#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <dirent.h>

#include "../core/Modules.h"
#include "../modules/ModuleList.h"

using namespace std;
/*
*   ---------------------------------------------------------------------------
*	---------------------------------------------------------------------------
*	                        Preset Managing stuff
*	---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*/

/*
*   ----------------------File identification keys-----------------------------
*/

/** Preset File @see save_preset(string, string, Module_Node_List) */
static const string NODE_ROOT_PRESET("Preset");
static const string NRP_NAME("Name");
static const string NRP_VERSION("Version");

static const string NODE_ROOT_MODULE("ModuleBak");

static const string NODE_MODULE("Module");
static const string NMOD_TYPE("Type");
static const string NMOD_BANK("Bank");
static const string NMOD_SIZE("Size");
static const string NMOD_DATA("Data");

static const string NODE_CONNECTION("Connection");

static const string NODE_POTENTIOMETER("Potentiometer");
static const string NPOT_IDX("Index");
static const string NPOT_TARGET("Target");

static const string NPOT_CURVE("Curve");
static const string NPOT_TARGETN("TargetM");
static const string NPOT_TARGETP("TargetP");
static const string NPOT_MIN("Min");
static const string NPOT_MAX("Max");
static const string NPOT_DB("IsdB");
static const string NPOT_RLT("IsRlt");


static const string NODE_PARAM("Params");
static const string NODE_BUTTON("Button");

static const string TAB("    ");

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
*   @return 0 on success
*/
int save_preset(string const name, string const version, Module_Node_List *list);
/**
*   Function used to load a preset file.
*   First verify that file is a preset file by reading it first node
*   Then verify that file's version correspond to curent program's
*   @see save_preset(string, string, Module_Node_List)
*
*   @param name file name
*   @param version curent program version
*   @param list pointer to main process graph
*   @return 0 on success
*
*   @warning if presset loaded successful it will cause a brief sound's interuption
*/
int load_preset(string const name, string const version, Module_Node_List*& list);

/**
*   Function used for create new empty presset file
*   
*   @param name file name
*   @param version current program version
*   @return 0 on success
*/
int new_preset(string const name, string const version);

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
int save_module(string const name, string const version, Module* mod);

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
int load_module(string const name, string const version, Module* mod, bool del);

/**
*   Function used to list all avaiable files in givn directory
*
*   @param dir directory path
*   @param ext file extention
*   @param list vector that store file names
*
*   @return 0 if returned non empty list
*/
int list_files(string const dir, vector<string> *list);

#endif