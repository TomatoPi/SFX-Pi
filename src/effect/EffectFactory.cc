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

using namespace std;

namespace PluginLoader{

/**
 * Main function that will load a plugin from the directory path
 *  return a pointer to loaded plugin, NULL if load failed
 **/
JackPlugin* loadPlugin(string path){

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginLoader : Load Plugin : " << path << endl;
    #endif

    // Create the plugin
    JackPlugin* plugin = new JackPlugin();

    try{
        
        // Try Load his config
        if (PluginConfigLoader::loadConfig((Plugin*)plugin, path)) {

            throw string("Failed Load Config File");
        }

        // Try Load his functions
        if (PluginSourceLoader::loadFunctions(plugin, path)) {

            throw string("Failed Load Source File");
        }

        if (plugin->status()) {

            throw string("Catched Internal Load Error");
        }
    }
    catch (string & e) {

        cout << "PluginLoader : Error : " << e << endl;
        cout << "PluginLoader : Error : Plugin Status : " << plugin->status() << endl;
        delete plugin;
        
        return NULL;
    }

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginLoader : Plugin Successfully Loaded : " << plugin->name() << endl;
    #endif

    return plugin;
}

/**
 * Function to unload a plugin previously loaded with loadPlugin
 *  function
 *  Unload Libraries and destroy plugin
 * @warning Effects builded from this plugin must be destroyed before
 *  unloading the plugin
 **/
void unloadPlugin(JackPlugin* plugin){

    if (plugin == NULL) {

        cout << "PluginConfigLoader : Warning : Tried to unload a null Plugin" << endl;
        return;
    }

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginConfigLoader : Unload Plugin : " << plugin->name() << endl;
    #endif
    
    // Unlink handle
    dlclose(plugin->handle());

    // Destroy plugin
    delete plugin;
}
}

/**
 * Map of TypeCode/Plugin pairs
 **/
EffectFactory::EffectFactoryRegistry EffectFactory::m_reg = EffectFactory::EffectFactoryRegistry();

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
        cout << "EffectFactory : Build Plugin Registry" << endl;
    #endif

    // First load TypeCodeTable
    if (TCTLoader::loadTypeCodeTable(SFXP::DIR_CONF)) return true;

    // If TypeCodeTable is ok load plugin list
    for (auto & dir : TCTLoader::typeCodeTable()) {

        JackPlugin* plugin = PluginLoader::loadPlugin(SFXP::DIR_PLUG+dir.second);

        if (plugin) {

            m_reg[dir.first] = plugin;
        
            cout << "EffectFactory : Registered Plugin : " << plugin->name()
                << " with TypeCode : " << dir.first << endl;
        }
        else{

            cout << "EffectFactory : Warning : Failed load plugin : "
                << dir.second << endl;
        }
    }

    return false;
}

/**
 * Function to get a plugin previously loaded from it TypeCode
 * Return NULL if given TC is invalid
 **/
JackPlugin* EffectFactory::getPlugin(SFXP::tc_t tc){

    if (m_reg.find(tc) != m_reg.end()) {

        return m_reg[tc];
    }
    return NULL;
}

/**
 * Function to print list of registered Effects
 **/
std::ostream& EffectFactory::print(std::ostream& os){

    os << "EffectRegistry :" << endl;

    for (auto & p : m_reg) {

        os << "    - ";
        os.width(15); os << left << p.second->name();
        os << " : ";
        os.width(3); os << left << p.first;
        os << endl;
    }
}
