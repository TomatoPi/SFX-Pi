/**********************************************************************
 * @file EffectFactory.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing functions to create an effect from a TypeCode
 *  To create an effect the plugin registery has to be created from the
 *  loaded TypeCodeTable
 **********************************************************************/
#ifndef DEF_EFFECT_FACTORY_H
#define DEF_EFFECT_FACTORY_H

#include <dlfcn.h>
#include <string>
#include <map>

#include "types.h"
#include "log.h"

#include "plugin/Plugin.h"
#include "plugin/TypeCodeTable.h"

namespace PluginLoader {

    struct PluginInfos {

        Plugin::builder_f _builder;/**< Pointer to builder function */
        Plugin::destructor_f _destructor;
                                    /**< Pointer to destructor function */

        void* _libHandle;          /**< Pointer to library handle */
        Plugin* _plugin;           /**< Pointer to the loaded Plugin */
    };
    
    /**
     * Main function that will load a plugin from the dirrectory path
     *  return a pointer to loaded plugin, NULL if load failed
     **/
    PluginInfos* loadPlugin(std::string path);

    /**
     * Function to unload a plugin previously loaded with loadPlugin
     *  function
     *  Unload Libraries and destroy plugin
     * @warning Effects builded from this plugin must be destroyed before
     *  unloading the plugin
     **/
    void unloadPlugin(PluginInfos* plugin);
}

class EffectFactory{

    public :

        typedef std::map<SFXP::tc_t,PluginLoader::PluginInfos*> EffectFactoryRegistry;

        /**
         * Function that Build EffectFactoryRegistry.
         * First load TypeCodeTable.xml file to get list of plugins to
         *  load
         * Then load each plugin and add it to it registry, then plugins
         *  can be used with it's asigned TypeCode
         * Return true if building has criticaly failed
         **/
        static bool buildRegistry();

        /**
         * Function to get a plugin previously loaded from it TypeCode
         * Return NULL if given TC is invalid
         **/
        static Plugin* getPlugin(SFXP::tc_t tc);

        /**
         * Function to print list of registered Effects
         **/
        static void print();

    private :
    
        static EffectFactoryRegistry m_reg; /**< Map of TypeCode/Plugin pairs */
};

#endif
