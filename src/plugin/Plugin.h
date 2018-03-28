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

#include "effect/AbstractEffectUnit.h"
#include "types.h"
#include "log.h"

/**
 * @brief Base Abstract class for effect plugins
 * Contains a set of functions that must be redefined by all plugins
 *  that return plugin's specific informations
 **/
class Plugin{

    public :

        /**
         * @brief Prototype for function that will build an instance of
         *  the plugin.
         * This function has to be dynamically loaded from the plugin's
         *  .so file using dlopen lib
         **/
        typedef Plugin* (*builder_f)(void);

        /**
         * @brief Prototype for function that will destroy an instance of
         *  the plugin.
         * This function has to be dynamically loaded from the plugin's
         *  .so file using dlopen lib
         **/
        typedef void (*destructor_f)(Plugin* plugin);

        /**
         * @brief The ParamConfig struct, Store informations about a parameter
         */
        struct ParamConfig{

            /**
             * @brief The Type enum, List of available params types
             */
            enum Type {
                Bool,   /**< Param that only accept Two values */
                Bounded,/**< Param that accept values within a range */
                Free,   /**< Param that accept every float value */
                Enum    /**< Param that accept a list of values */
            };

            /**
             * @brief ParamConfig Constructor to make Free Param
             * @param g Param's group
             * @param n Param's name
             */
            ParamConfig(std::string g, std::string n):
                type(Type::Free),group(g),name(n)
            {
            }

            /**
             * @brief ParamConfig Constructor to make Bool Param
             * @param g Param's group
             * @param n Param's name
             * @param on String for on value
             * @param off String for off value
             */
            ParamConfig(std::string g, std::string n, std::string on, std::string off):
                type(Type::Bool), group(g), name(n), on(on), off(off)
            {
            }

            /**
             * @brief ParamConfig Constructor to make Bounded Param
             * @param g Param's group
             * @param n Param's name
             * @param min Minimal allowed value
             * @param max Maximal allowed value
             */
            ParamConfig(std::string g, std::string n, float min, float max):
                type(Type::Bounded), group(g), name(n), min(min), max(max)
            {
            }

            /**
             * @brief ParamConfig Constructor to make Enum Param
             * @param g Param's group
             * @param n Param's name
             * @param v List of available values
             */
            ParamConfig(std::string g, std::string n, std::vector<std::string> v):
                type(Type::Enum), group(g), name(n), values(v)
            {
            }

            Type type;                  /**< Param's Type */
            std::string group;          /**< Used to store related params together */
            std::string name;           /**< Param's Name */
            std::string on, off;        /**< On and Off values for Bools */
            float min, max;             /**< Boundaries for Bounded Boundeds */
            std::vector<std::string> values;/**< List of values for Enums */
    };
        
        /**
         * @brief Plugin Constructor
         **/
        virtual ~Plugin(){};

        /* ********************************************* */
        /* ****************** GETTERS ****************** */
        /* ********************************************* */

        /* *** Status *** */

        /**
         * @brief name, return internal name of the plugin
         */
        virtual const std::string name() const = 0;

        /**
         * @brief pool, return the unmber of parameters inside the effect's
         *  process pool
         */
        virtual const SFXP::usize_t pool() const = 0;

        /* *** Params *** */
        
        /**
         * @brief paramCount, return number of plugin's parameters
         */
        virtual const SFXP::usize_t paramCount() const = 0;

        /**
         * @brief paramList, return array of plugin's parameters
         */
        virtual const Plugin::ParamConfig* paramList() const = 0;

        /**
         * @brief paramName, return name of given param, empty string if invalid
         *  index.
         * @param idx index of wanted param
         */
        virtual const std::string paramName(SFXP::usize_t idx) const = 0;

        /* *** Ports *** */
        
        /**
         * @brief portCount, return list of number of ports for each type
         * @return usize_t[4] array storing count of ports for each type
         */
        virtual const SFXP::usize_t* portCount() const = 0;
        /**
         * @brief portName, return list of names of ports for each type
         * @return string[4][] array storing names of ports for each type
         * @warning the returned array is dynamically instanciated so the
         *  user has to free it after calls of this function
         */
        virtual const std::string** portName() const = 0;

        /**
         * @brief buildNonJack, function to build a non Jack Version of the
         *  effect provided by this Plugin
         * @param t TypeCode of the ParamSet
         * @param id Id of the ParamSet
         * @return Pointer to the created param set
         */
        virtual ParamSet* buildNonJack(SFXP::tc_t t, SFXP::id1_t id) = 0;

        /**
         * @brief build, function to create a Jack Version of the effect provided
         *  by this Plugin
         * @param typeCode TypeCode of the EffectUnit
         * @param id Id of the EffectUnit
         * @return Pointer to the created EffectUnit
         */
        virtual AbstractEffectUnit* build(SFXP::tc_t typeCode, SFXP::id1_t id) = 0;

        /**
         * @brief destroy, function to destroy an Effect loaded from a plugin
         * @param unit Effect to destroy
         */
        virtual void destroy(AbstractEffectUnit* unit) = 0;
};

#define PLUGIN_INTERFACE_METHODS \
\
virtual const std::string name() const; \
virtual const SFXP::usize_t pool() const; \
\
virtual const SFXP::usize_t paramCount() const; \
virtual const Plugin::ParamConfig* paramList() const; \
virtual const std::string paramName(SFXP::usize_t idx) const; \
\
virtual const SFXP::usize_t* portCount() const; \
virtual const std::string** portName() const; \
\
virtual ParamSet* buildNonJack(SFXP::tc_t t, SFXP::id1_t id); \
virtual AbstractEffectUnit* build(SFXP::tc_t typeCode, SFXP::id1_t id); \
virtual void destroy(AbstractEffectUnit* unit);

#define PLUGIN_INTERFACE_DECLARATIONS(pluginClassName, pluginEffectName) \
\
extern "C" Plugin* func_builder() { return new pluginClassName ## _Class(); } \
extern "C" void func_destructor(Plugin* plg) { delete plg; } \
 \
const std::string pluginClassName ## _Class::name() const { return #pluginEffectName; } \
const SFXP::usize_t pluginClassName ## _Class::pool() const { return POOL_SIZE; } \
 \
const SFXP::usize_t pluginClassName ## _Class::paramCount() const { return PARAM_COUNT; } \
const Plugin::ParamConfig* pluginClassName ## _Class::paramList() const { return PARAMS; } \
const std::string pluginClassName ## _Class::paramName(SFXP::usize_t idx) const { if (idx < PARAM_COUNT) return PARAMS[idx].name; return ""; } \
 \
const SFXP::usize_t* pluginClassName ## _Class::portCount() const { return PORTCOUNT; } \
const std::string** pluginClassName ## _Class::portName() const { return (const std::string**)PORTS; } \
 \
ParamSet* pluginClassName ## _Class::buildNonJack(SFXP::tc_t t, SFXP::id1_t id) { return new ParamSet(t, id, PARAM_COUNT); } \
AbstractEffectUnit* pluginClassName ## _Class::build(SFXP::tc_t typeCode, SFXP::id1_t id) { return (AbstractEffectUnit*)(new pluginEffectName ## _Class(typeCode, id, (Plugin*)this)); } \
void pluginClassName ## _Class::destroy(AbstractEffectUnit* unit) { unit->deactivate(); delete unit; }

#define PLUGIN_DELETE_PORT_NAME_ARRAY(array) \
for (int z = 0; z < 4; z++) delete[] array[z]; \
delete[] array;

#endif
