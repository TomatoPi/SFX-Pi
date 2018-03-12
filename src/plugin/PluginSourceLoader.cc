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
bool loadFunctions(JackPlugin* plugin, std::string path){

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginFunctionLoader : Load Source File : " << path << endl;
    #endif

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
    JackPlugin::builder_f builder = (JackPlugin::builder_f) dlsym(handle, "func_builder");
    const char* dlsym_error = dlerror();
    if (dlsym_error){
        
        cout << "PluginFunctionLoader : Error : Failed Load Symbol : " << dlsym_error << endl;
        success = false;
        builder = NULL;
    }

    // Load destructor function
    JackPlugin::destructor_f destructor = (JackPlugin::destructor_f) dlsym(handle, "func_destructor");
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
