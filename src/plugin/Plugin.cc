/**********************************************************************
 * @file Plugin.h
 * @author TomatoPi
 * @version 1.1
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
    m_portNames(NULL)
{
    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled){

        cout << "Start Plugin's Construction" << endl;
    }
    #endif
}
Plugin::~Plugin(){

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled){

        cout << "Destroy Plugin : " << m_name << endl;
    }
    #endif

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
        
        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Name \"" << m_name << "\"" << endl;
        #endif
        
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
        
        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : Pool " << pool << endl;
        #endif

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

        #ifdef __DEBUG__
        if (SFXP::GlobalIsDebugEnabled)
            cout << "PluginBuilding : Registered : " << m_paramCount << " parameters" << endl;
        #endif

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

            #ifdef __DEBUG__
            if (SFXP::GlobalIsDebugEnabled)
                cout << "PluginBuilding : Registered : " << m_portCount[i] << " ports of type " << i << endl;
            #endif
        }

        return false;
    }
    else{

        cout << "PluginBuiding : Warning : Ports already registered" << endl;
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
usize_t Plugin::portCount(PortType type) const{

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
const string* Plugin::portNameList(PortType type) const{

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
string Plugin::portName(PortType type, usize_t idx) const{

    if (m_status) return "";
    if (idx >= m_portCount[static_cast<usize_t>(type)]) return "";
    return m_portNames[static_cast<usize_t>(type)][idx];
}

/**
 * Create a non Jack Version of the effect Provided by this plugin
 **/
ParamSet* Plugin::buildNonJack(SFXP::tc_t typeCode, SFXP::id1_t id){

    // If plugin's Config file is completly loaded
    if (!(m_status & ConfigMask))
    {
        return new ParamSet(id, typeCode, m_paramCount);
    }
    return NULL;
}
