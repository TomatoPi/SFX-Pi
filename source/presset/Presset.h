#ifndef DEF_PRESSET_H
#define DEF_PRESSET_H

#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <fstream>

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

static const string NODE_MODULE("Module");
static const string NODE_PARAM("Params");
static const string NODE_ACCESSOR("Accessor");
static const string NODE_CONNECTION("Connection");
static const string NODE_POTENTIOMETER("Potentiometer");
static const string NODE_BUTTON("Button");

/**
*   Function used to save curent process graph inside a Presset file.
*   Preset is saved with format :
*
*   NODE_ROOT_PRESET "program_version" "preset_name" "modules's_count"
*   NODE_MODULE "mode_type_code" "number_of_params"
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
int save_preset(string name, string version, Module_Node_List list);
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
int load_preset(string name, string version, Module_Node_List *list);


#endif