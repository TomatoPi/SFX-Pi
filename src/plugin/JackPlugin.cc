/**********************************************************************
 * @file Plugin.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Base file for plugins used by SFXPi
 **********************************************************************/
#include "JackPlugin.h"

using namespace SFXP;

JackPlugin::JackPlugin():Plugin(),
    m_builder(NULL),
    m_destructor(NULL),

    m_libHandle(NULL)
{
}
JackPlugin::~JackPlugin(){
}

/* ****************************************************************** */
/* ******************* Plugin's building functions ****************** */
/* ****************************************************************** */

/**
 * Function to register builder
 *  return true if an error occured
 **/
bool JackPlugin::registerBuilder(builder_f builder){

    if (m_status & MissBuilder){

        if (builder == NULL){

            cout << "PluginBuilding : Error : Null Builder Function Passed" << endl;
            return true;
        }

        m_builder = builder;
        m_status = m_status&(~MissBuilder);

        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Builder function" << endl;
        #endif

        return false;
    }
    else{

        cout << "PluginBuiding : Warning : Builder already registered" << endl;
        return true;
    }
}

/**
 * Function to register destructor
 *  return true if an error occured
 **/
bool JackPlugin::registerDestructor(destructor_f destructor){

    if (m_status & MissDestructor){

        if (destructor == NULL){

            cout << "PluginBuilding : Error : Null Destructor Function Passed" << endl;
            return true;
        }

        m_destructor = destructor;
        m_status = m_status&(~MissDestructor);

        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Destructor function" << endl;
        #endif

        return false;
    }
    else{

        cout << "PluginBuiding : Warning : Destructor already registered" << endl;
        return true;
    }
}

/**
 * Function to save pointer to plugin's dynamic library handle
 *  return true if an error occured
 **/
bool JackPlugin::registerHandle(void* handle){

    if (m_status & MissLibrary) {

        if (handle == NULL) {

            cout << "PluginBuilding : Error : Null Library Handle Passed" << endl;
            return true;
        }

        m_libHandle = handle;
        m_status = m_status&(~MissLibrary);

        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Library Handle" << endl;
        #endif

        return false;
    }
    else {

        cout << "PluginBuiding : Warning : Library Handle already registered" << endl;
        return true;
    }
}

/* ****************************************************************** */
/* ***************************** GETTERS **************************** */
/* ****************************************************************** */

/**
 * Return library handle pointer
 **/
void* JackPlugin::handle() const{

    return m_libHandle;
}

/**
 * Calls builder function
 *  return null if plugin is incomplete
 **/
AbstractEffectUnit* JackPlugin::build(tc_t typeCode, id1_t id){

    if (m_status) {

        cout << "Error Unifinshed plugin : " << hex << m_status << endl;
        return NULL;
    }

    return (*m_builder)(typeCode, id, this);
}

/**
 * Calls destructor function
 *  return null if plugin is incomplete
 **/
void JackPlugin::destroy(AbstractEffectUnit* effect){

    if (!m_status){

        // Call Predestructor to prevent segmentation fault
        effect->deactivate();
        
        // Call destructor function
        (*m_destructor)(effect);
    }
}
