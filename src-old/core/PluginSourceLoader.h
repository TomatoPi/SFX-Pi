/**********************************************************************
 * @file PluginSourceLoader.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load a plugin from a source file
 **********************************************************************/
#ifndef DEF_PLUGIN_SOURCE_LOADER_H
#define DEF_PLUGIN_SOURCE_LOADER_H

#include <dlfcn.h>
#include <iostream>
#include <string>

#include "lib/plugin/Plugin.h"

namespace PluginSourceLoader {

    const std::string SourceFile = "source.so";

    /**
     * Function to load plugins functions from a dynamic library file
     *  return true if an error occured during loading
     **/
    bool loadFunctions(Plugin* plugin, std::string path);
};

#endif
