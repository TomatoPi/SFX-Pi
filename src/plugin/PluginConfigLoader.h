/**********************************************************************
 * @file PluginConfigLoader.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load a plugin from an xml file
 **********************************************************************/
#ifndef DEF_PLUGIN_CONFIG_LOADER_H
#define DEF_PLUGIN_CONFIG_LOADER_H

#include <tinyxml2.h>
#include <iostream>
#include <string>

#include "plugin/Plugin.h"
#include "types.h"

namespace PluginConfigLoader{
    
    using namespace tinyxml2;

    const std::string ConfigFile = "config.xml";

    /**
     * Function to load a plugin config from an XML file
     *  return true if an error occured during loading
     **/
    bool loadConfig(Plugin* plugin, std::string path);

    /**
     * Function to parse Plugin Description Node
     *  return true if an error occured during parsing
     **/
    bool parseDescription(Plugin* plugin, XMLElement* desc);

    /**
     * Function to parse Plugin Parameter List Node
     *  return true if an error occured during parsing
     **/
    bool parseParamList(Plugin* plugin, XMLElement* params);

    /**
     * Function to parse Plugin Port List Node
     *  return true if an error occured during parsing
     **/
    bool parsePortList(Plugin* plugin, XMLElement* ports);

    /**
     * Function to get a param name from a param Node
     *  return an empty string if failed
     **/
    std::string parseParamNode(XMLElement* param);

    /**
     * Function to get a port from a port Node
     *  return a pair<type_of_port,name_of_port>, empty name if failed
     **/
    std::pair<SFXP::PortType,std::string> parsePortNode(XMLElement* port);
};

#endif
