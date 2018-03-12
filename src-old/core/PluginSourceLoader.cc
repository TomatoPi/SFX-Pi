/**********************************************************************
 * @file PluginSourceLoader.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load a plugin from a source file
 **********************************************************************/
#include "PluginSourceLoader.h"

using namespace std;

namespace PluginSourceLoader {

/**
 * Function to load plugins functions from a dynamic library file
 *  return true if an error occured during loading
 **/
bool loadFunctions(Plugin* plugin, std::string path){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginFunctionLoader : Load Source File : " << path << endl;

    // Open the library
    void* handle = dlopen((path+SourceFile).c_str(), RTLD_LAZY);
    if (!handle) {

        cout << "PluginFunctionLoader : Error : Failed Open Library : "
            << dlerror() << endl;
        return true;
    }

    dlerror();
    bool success = true;

    // Load builder function
    Plugin::builder_f builder = (Plugin::builder_f) dlsym(handle, "func_builder");
    const char* dlsym_error = dlerror();
    if (dlsym_error){
        
        cout << "PluginFunctionLoader : Error : Failed Load Symbol : " << dlsym_error << endl;
        success = false;
        builder = NULL;
    }

    // Load destructor function
    Plugin::destructor_f destructor = (Plugin::destructor_f) dlsym(handle, "func_destructor");
    dlsym_error = dlerror();
    if (dlsym_error){
        
        cout << "PluginFunctionLoader : Error : Failed Load Symbol : " << dlsym_error << endl;
        success = false;
        destructor = NULL;
    }

    // Register loaded functions
    success &= !plugin->registerBuilder(builder);
    success &= !plugin->registerDestructor(destructor);
    success &= !plugin->registerHandle(handle);
    
    return !success;
}
}
