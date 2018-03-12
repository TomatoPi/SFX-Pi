/**********************************************************************
 * @file Plugin.h
 * @author TomatoPi
 * @version 1.0
 *
 * Base file for plugins used by SFXPi
 **********************************************************************/
#ifndef DEF_JACK_PLUGIN_H
#define DEF_JACK_PLUGIN_H

#include "plugin/Plugin.h"
#include "effect/AbstractEffectUnit.h"

/**
 * Base class for SFX-Pi Plugins
 * 
 *  Provide builder and destructor functions for manage plugin's effects
 *   instances
 **/
class JackPlugin : public Plugin {

    public :

        /**
         * Builder's function prototype
         **/
        typedef AbstractEffectUnit* (*builder_f)(SFXP::tc_t typeCode, SFXP::id1_t id, Plugin* plugin);

        /**
         * Destructor's function prototype
         **/
        typedef void (*destructor_f)(AbstractEffectUnit* effect);

        JackPlugin();
        virtual ~JackPlugin();

        /* ********************************************* */
        /* ******** Plugin's building functions ******** */
        /* ********************************************* */

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

    private :

        builder_f m_builder;        /**< Pointer to builder function */
        destructor_f m_destructor;  /**< Pointer to destructor function */

        void* m_libHandle;          /**< Pointer to library handle */
};

#endif
