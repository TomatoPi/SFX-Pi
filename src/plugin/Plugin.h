/**********************************************************************
 * @file Plugin.h
 * @author TomatoPi
 * @version 1.1
 *
 * Base file for plugin system
 **********************************************************************/
#ifndef DEF_PLUGIN_H
#define DEF_PLUGIN_H

#include <vector>
#include <string>
#include <iostream>

#include "effect/EffectParamSet.h"
#include "types.h"

/**
 * Base calss for effect plugins
 *  Contain configuration of a plugin, his name, number and parameters
 *   names number and parameters ports
 *  A plugin must be builded calling each registering functions, effects
 *   can't be created if plugin building is incomplete
 **/
class Plugin{

    public :

        /**
         * Enum of available states for a plugin
         *  Used to construct plugin in several steps
         **/
        enum PluginStatus{
            Ready           = 0x00,
            MissName        = 0x01,
            MissPool        = 0x02,
            MissParam       = 0x04,
            MissPorts       = 0x08,
            MissBuilder     = 0x10,
            MissDestructor  = 0x20,
            MissLibrary     = 0x40,
            NotBuilt = MissName|MissPool|MissParam|MissPorts|MissBuilder|MissDestructor|MissLibrary,
            ConfigMask = MissName|MissPool|MissParam|MissPorts,
            SourceMask = MissBuilder|MissDestructor|MissLibrary
        };
        
        /**
         * Plugin constructor, init it state at NotBuilt
         **/
        Plugin();
        virtual ~Plugin();

        /* ********************************************* */
        /* ******** Plugin's building functions ******** */
        /* ********************************************* */
        
        /**
         * Function to register plugin's name, given string must have
         *  non zero length
         *  return true if an error occured
         **/
        bool registerName(std::string name);

        /**
         * Function used to register plugin's pool size
         *  return true if an error occured
         **/
        bool registerPool(SFXP::usize_t pool);

        /**
         * Function to register parameters list
         *  return true if an error occured
         **/
        bool registerParams(std::vector<std::string> params);

        /**
         * Function to register ports list
         *  return true if an error occured
         **/
        bool registerPorts(std::vector<std::string>* ports);

        /* ********************************************* */
        /* ****************** GETTERS ****************** */
        /* ********************************************* */

        /* *** Status *** */
        
        /**
         * Return current plugin's status
         **/
        unsigned int status() const;

        /**
         * Return plugin's name
         **/
        std::string name() const;

        /**
         * Return plugin's pool size
         **/
        SFXP::usize_t pool() const;

        /* *** Params *** */
        
        /**
         * Return plugin's param count
         **/
        SFXP::usize_t paramCount() const;

        /**
         * Return plugin's param list
         **/
        std::string* paramList() const;

        /**
         * Return name of given param, empty string if index is invalid
         **/
        std::string paramName(SFXP::usize_t idx) const;

        /* *** Ports *** */
        
        /**
         * Return total port count
         **/
        SFXP::usize_t portCount() const;

        /**
         * Return port count of given type
         **/
        SFXP::usize_t portCount(SFXP::PortType type) const;

        /**
         * Return list of ports count
         **/
        const SFXP::usize_t* portCountList() const;

        /**
         * Return list of ports names of given type
         **/
        const std::string* portNameList(SFXP::PortType type) const;

        /**
         * Return list of list of ports names
         **/
        const std::string** portNameListList() const;

        /**
         * Return name of given port, empty string if port doesn't exist
         **/
        std::string portName(SFXP::PortType type, SFXP::usize_t idx) const;

        /**
         * Create a non Jack Version of the effect Provided by this plugin
         **/
        ParamSet* buildNonJack(SFXP::tc_t typeCode, SFXP::id1_t id);

    protected :

        std::string     m_name;     /**< Plugin's name */
        unsigned int    m_status;   /**< Current plugin's status */

        SFXP::usize_t m_poolSize;   /**< Number of processing params */
        SFXP::usize_t m_paramCount; /**< Number of parameters */
        std::string* m_paramNames;  /**< List of param names */

        SFXP::usize_t* m_portCount;  /**< List of ports count by type */
        std::string** m_portNames;   /**< List of ports names by type */
};

#endif
