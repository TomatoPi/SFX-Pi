/**********************************************************************
 * @file EffectFactory.cc
 * @author TomatoPi
 * @version 1.1
 *
 * File providing functions to create an effect from a TypeCode
 *  To create an effect the plugin registery has to be created from the
 *  loaded TypeCodeTable
 **********************************************************************/
#include "EffectFactory.h"

#define NAMEP ("PluginLoader")
#define NAME ("EffectFactory")

using namespace std;

namespace PluginLoader{

/**
 * Main function that will load a plugin from the directory path
 *  return a pointer to loaded plugin, NULL if load failed
 **/
PluginInfos* loadPlugin(string path){

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAMEP) << "Load Plugin : " << path << endl;
    #endif

    try{

        // Open the library
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {

            throw string("Failed Open Library : ")+string(dlerror());
        }
        
        dlerror();

        // Create plugin infos object
        PluginInfos* infos = new PluginInfos();
        infos->_libHandle = handle;

        // Load builder function
        infos->_builder = (Plugin::builder_f) dlsym(handle, "func_builder");
        const char* dlsym_error = dlerror();
        if (dlsym_error){
            
            throw string("Failed Load Symbol : ")+string(dlsym_error);
        }

        // Load destructor function
        infos->_destructor = (Plugin::destructor_f) dlsym(handle, "func_destructor");
        dlsym_error = dlerror();
        if (dlsym_error){
            
            throw string("Failed Load Symbol : ")+string(dlsym_error);
        }

        // Build the plugin object
        infos->_plugin = (*(infos->_builder))();
        if (!infos->_plugin) {

            throw string("Failed Build Plugin");
        }

        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            SFXPlog::debug(NAMEP) << "Plugin Successfully Loaded : "
                << infos->_plugin->name() << endl;
        #endif
        
        return infos;
    }
    catch (string & e) {

        SFXPlog::err(NAMEP) << e << endl;
        
        return nullptr;
    }
}

/**
 * Function to unload a plugin previously loaded with loadPlugin
 *  function
 *  Unload Libraries and destroy plugin
 * @warning Effects builded from this plugin must be destroyed before
 *  unloading the plugin
 **/
void unloadPlugin(PluginInfos* plugin){

    if (plugin == NULL) {

        cout << "PluginConfigLoader : Warning : Tried to unload a null Plugin"
            << endl;
        return;
    }

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAMEP) << "Unload Plugin : " << plugin->_plugin->name() << endl;
    #endif

    // Destroy plugin
    (*(plugin->_destructor))(plugin->_plugin);
    
    // Unlink handle
    dlclose(plugin->_libHandle);

    // Destroy Plugininfos
    delete plugin;
}
}

/**
 * Map of TypeCode/Plugin pairs
 **/
EffectFactory::EffectFactoryRegistry EffectFactory::m_reg =
        EffectFactory::EffectFactoryRegistry();

/**
 * Function that Build EffectFactoryRegistry.
 * First load TypeCodeTable.xml file to get list of plugins to
 *  load
 * Then load each plugin and add it to it registry, then plugins
 *  can be used with it's asigned TypeCode
 * Return true if building has criticaly failed
 **/
bool EffectFactory::buildRegistry(){

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAME) << "Build Plugin Registry" << endl;
    #endif

    // First load TypeCodeTable
    if (TCTLoader::loadTypeCodeTable(SFXP::DIR_CONF)) return true;

    // If TypeCodeTable is ok load plugin list
    for (auto & dir : TCTLoader::typeCodeTable()) {

        PluginLoader::PluginInfos* plugin =
                PluginLoader::loadPlugin(SFXP::DIR_PLUG+dir.second);

        if (plugin) {

            m_reg[dir.first] = plugin;
        
            SFXPlog::log(NAME) << "Registered Plugin : "
                << plugin->_plugin->name() << " with TypeCode : "
                << dir.first << endl;
        }
        else{

            SFXPlog::wrn(NAME) << "Failed load plugin : "
                << dir.second << endl;
        }
    }

    return false;
}

/**
 * Function to get a plugin previously loaded from it TypeCode
 * Return NULL if given TC is invalid
 **/
Plugin* EffectFactory::getPlugin(SFXP::tc_t tc){

    if (m_reg.find(tc) != m_reg.end()) {

        return m_reg[tc]->_plugin;
    }
    return NULL;
}

/**
 * Function to print list of registered Effects
 **/
void EffectFactory::print(){

    cout << "EffectRegistry :" << endl;

    for (auto & p : m_reg) {

        cout << "    - ";
        cout.width(15); cout << left << p.second->_plugin->name();
        cout << " : ";
        cout.width(3); cout << left << p.first;
        cout << endl;
    }
}
