/**********************************************************************
 * @file Plugin.h
 * @author TomatoPi
 * @version 1.0
 *
 * Base file for plugin system
 **********************************************************************/
#ifndef DEF_PLUGIN_H
#define DEF_PLUGIN_H

#include <vector>
#include <string>

#include "lib/unit/AbstractEffectUnit.h"
#include "Const.h"

/**
 * Base calss for effect plugins
 *  Contain configuration of a plugin, his name, number and parameters
 *   names number and parameters ports
 *  Provide builder and destructor functions for manage plugin's effects
 *   instances
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
         * Builder's function prototype
         **/
        typedef AbstractEffectUnit* (*builder_f)(SFXP::tc_t typeCode, SFXP::id1_t id, Plugin* plugin);

        /**
         * Destructor's function prototype
         **/
        typedef void (*destructor_f)(AbstractEffectUnit* effect);

        /**
         * Plugin constructor, init it state at NotBuilt
         **/
        Plugin();
        ~Plugin();

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

        /**
         * Function to register builder
         *  return true if an error occured
         **/
        bool registerBuilder(builder_f builder);

        /**
         * Function to register destructor
         *  return true if an error occured
         **/
        bool registerDestructor(destructor_f destructor);

        /**
         * Function to save pointer to plugin's dynamic library handle
         *  return true if an error occured
         **/
        bool registerHandle(void* handle);

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
        SFXP::usize_t portCount(JACKUnit::PortType type) const;

        /**
         * Return list of ports count
         **/
        const SFXP::usize_t* portCountList() const;

        /**
         * Return list of ports names of given type
         **/
        const std::string* portNameList(JACKUnit::PortType type) const;

        /**
         * Return list of list of ports names
         **/
        const std::string** portNameListList() const;

        /**
         * Return name of given port, empty string if port doesn't exist
         **/
        std::string portName(JACKUnit::PortType type, SFXP::usize_t idx) const;

        /**
         * Return library handle pointer
         **/
        void* handle() const;

        /**
         * Calls builder function
         *  return null if plugin is incomplete
         **/
        AbstractEffectUnit* build(SFXP::tc_t typeCode, SFXP::id1_t id);

        /**
         * Calls destructor function
         *  return null if plugin is incomplete
         **/
        void destroy(AbstractEffectUnit* effect);

        /**
         * Create a non Jack Version of the effect Provided by this plugin
         **/
        EffectPrototype* buildNonJack(SFXP::tc_t typeCode, SFXP::id1_t id);

    private :

        std::string     m_name;     /**< Plugin's name */
        unsigned int    m_status;   /**< Current plugin's status */

        SFXP::usize_t m_poolSize;   /**< Number of processing params */
        SFXP::usize_t m_paramCount; /**< Number of parameters */
        std::string* m_paramNames;  /**< List of param names */

        SFXP::usize_t* m_portCount;  /**< List of ports count by type */
        std::string** m_portNames;   /**< List of ports names by type */

        builder_f m_builder;        /**< Pointer to builder function */
        destructor_f m_destructor;  /**< Pointer to destructor function */

        void* m_libHandle;          /**< Pointer to library handle */
};

#endif
