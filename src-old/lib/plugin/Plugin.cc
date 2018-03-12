/**********************************************************************
 * @file Plugin.h
 * @author TomatoPi
 * @version 1.0
 *
 * Base file for plugin system
 **********************************************************************/
#include "Plugin.h"

using namespace SFXP;

/**
 * Plugin constructor, init it state at NotBuilt
 **/
Plugin::Plugin():
    m_name(""),
    m_status(NotBuilt),
    m_poolSize(0),
    m_paramCount(0),
    m_paramNames(NULL),

    m_portCount(NULL),
    m_portNames(NULL),

    m_builder(NULL),
    m_destructor(NULL),

    m_libHandle(NULL)
{
    if (SFXP::GlobalIsDebugEnabled){

        cout << "Start Plugin's Construction" << endl;
    }
}
Plugin::~Plugin(){

    if (SFXP::GlobalIsDebugEnabled){

        cout << "Destroy Plugin : " << m_name << endl;
    }

    if (!(m_status&MissParam)){

        delete[] m_paramNames;
    }

    if (!(m_status&MissPorts)){

        for (usize_t i = 0; i < 4; i++ )
            delete[] m_portNames[i];
            
        delete[] m_portNames;
        delete[] m_portCount;
    }
}

/* ****************************************************************** */
/* ******************* Plugin's building functions ****************** */
/* ****************************************************************** */

/**
 * Function to register plugin's name, given string must have
 *  non zero length
 *  return true if an error occured
 **/
bool Plugin::registerName(string name){

    if (m_status & MissName && name.length() != 0){

        m_name = name;
        m_status = m_status&(~MissName);
        
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Name \"" << m_name << "\"" << endl;

        return false;
    }
    else{

        cout << "PluginBuiding : Error : Already registered or Invalid Name Given : " << m_name << " : " << name << endl;
        return true;
    }
}

/**
 * Function used to register plugin's pool size
 *  return true if an error occured
 **/
bool Plugin::registerPool(SFXP::usize_t pool){

    if (m_status & MissPool){

        m_poolSize = pool;
        m_status = m_status&(~MissPool);
        
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Pool " << pool << endl;

        return false;
        
    }
    else{

        cout << "PluginBuiding : Warning : Pool already registered" << endl;
        return true;
    }
}
        
/**
 * Function to register parameters list
 *  return true if an error occured
 **/
bool Plugin::registerParams(vector<string> params){

    if (m_status & MissParam){

        m_paramCount = params.size();
        m_paramNames = new string[m_paramCount];

        for (usize_t i = 0; i < m_paramCount; i++ )
            m_paramNames[i] = params[i];

        m_status = m_status&(~MissParam);

        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : " << m_paramCount << " parameters" << endl;

        return false;
    }
    else{

        cout << "PluginBuiding : Warning : Params already registered" << endl;
        return true;
    }
}

/**
 * Function to register ports list
 *  return true if an error occured
 **/
bool Plugin::registerPorts(vector<string>* ports){

    if (m_status & MissPorts){

        m_portCount = new usize_t[4];
        m_portNames = new string*[4];

        m_status = m_status&(~MissPorts);

        for (usize_t i = 0; i < 4; i++ ){

            m_portCount[i] = ports[i].size();
            m_portNames[i] = new string[m_portCount[i]];

            for (usize_t k = 0; k < m_portCount[i]; k++)
                m_portNames[i][k] = ports[i][k];

            if (SFXP::GlobalIsDebugEnabled)
                cout << "PluginBuilding : Registered : " << m_portCount[i] << " ports of type " << i << endl;
        }

        return false;
    }
    else{

        cout << "PluginBuiding : Warning : Ports already registered" << endl;
        return true;
    }
}

/**
 * Function to register builder
 *  return true if an error occured
 **/
bool Plugin::registerBuilder(builder_f builder){

    if (m_status & MissBuilder){

        if (builder == NULL){

            cout << "PluginBuilding : Error : Null Builder Function Passed" << endl;
            return true;
        }

        m_builder = builder;
        m_status = m_status&(~MissBuilder);

        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Builder function" << endl;

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
bool Plugin::registerDestructor(destructor_f destructor){

    if (m_status & MissDestructor){

        if (destructor == NULL){

            cout << "PluginBuilding : Error : Null Destructor Function Passed" << endl;
            return true;
        }

        m_destructor = destructor;
        m_status = m_status&(~MissDestructor);

        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Destructor function" << endl;

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
bool Plugin::registerHandle(void* handle){

    if (m_status & MissLibrary) {

        if (handle == NULL) {

            cout << "PluginBuilding : Error : Null Library Handle Passed" << endl;
            return true;
        }

        m_libHandle = handle;
        m_status = m_status&(~MissLibrary);

        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Library Handle" << endl;

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

/* ***************************** Status ***************************** */

/**
 * Return current plugin's status
 **/
unsigned int Plugin::status() const{

    return m_status;
}

/**
 * Return plugin's name
 **/
string Plugin::name() const{

    return m_name;
}

/**
 * Return plugin's pool size
 **/
usize_t Plugin::pool() const{

    return m_poolSize;
}
        
/* ****************************** Params **************************** */

/**
 * Return plugin's param count
 **/
usize_t Plugin::paramCount() const{

    return m_paramCount;
}

/**
 * Return plugin's param list
 **/
string* Plugin::paramList() const{

    if (m_status) return NULL;
    return m_paramNames;
}

/**
 * Return name of given param, empty string if index is invalid
 **/
string Plugin::paramName(usize_t idx) const{

    if (m_status || idx >= m_paramCount) return "";
    return m_paramNames[idx];
}

/* ****************************** Ports ***************************** */

/**
 * Return total port count
 **/
usize_t Plugin::portCount() const{

    if (m_status) return 0;
    usize_t s = 0;
    for (usize_t i = 0; i < 4; i++ )
        s += m_portCount[i];

    return s;
}

/**
 * Return port count of given type
 **/
usize_t Plugin::portCount(JACKUnit::PortType type) const{

    if (m_status) return 0;
    return m_portCount[static_cast<usize_t>(type)];
}

/**
 * Return list of ports count
 **/
const SFXP::usize_t* Plugin::portCountList() const{

    return m_portCount;
}

/**
 * Return list of ports names of given type
 **/
const string* Plugin::portNameList(JACKUnit::PortType type) const{

    if (m_status) return NULL;
    return m_portNames[static_cast<usize_t>(type)];
}

/**
 * Return list of list of ports names
 **/
const std::string** Plugin::portNameListList() const{

    return (const std::string**)m_portNames;
}

/**
 * Return name of given port, empty string if port doesn't exist
 **/
string Plugin::portName(JACKUnit::PortType type, usize_t idx) const{

    if (m_status) return "";
    if (idx >= m_portCount[static_cast<usize_t>(type)]) return "";
    return m_portNames[static_cast<usize_t>(type)][idx];
}

/**
 * Return library handle pointer
 **/
void* Plugin::handle() const{

    return m_libHandle;
}

/**
 * Calls builder function
 *  return null if plugin is incomplete
 **/
AbstractEffectUnit* Plugin::build(tc_t typeCode, id1_t id){

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
void Plugin::destroy(AbstractEffectUnit* effect){

    if (!m_status){

        // Call Predestructor to prevent segmentation fault
        effect->deactivate();
        
        // Call destructor function
        (*m_destructor)(effect);
    }
}

/**
 * Create a non Jack Version of the effect Provided by this plugin
 **/
EffectPrototype* Plugin::buildNonJack(SFXP::tc_t typeCode, SFXP::id1_t id){

    // If plugin's Config file is completly loaded
    if (!(m_status & ConfigMask))
    {
        return new EffectPrototype(id, typeCode, m_paramCount);
    }
    return NULL;
}
