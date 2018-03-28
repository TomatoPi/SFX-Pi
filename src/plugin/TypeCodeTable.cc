/**********************************************************************
 * @file TypeCodeTable.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load plugin list and table that give each
 *  plugin an unique code that will identify it
 **********************************************************************/
#include "TypeCodeTable.h"

#define NAME ("TCTLoader")

using namespace std;
using namespace tinyxml2;

/**
 * Path to Typecode File
 **/
std::string TCTLoader::TypeCodeFile = "TypeCodeTable.xml";

/**
 * TypeCode of Physical Input Ports
 **/
SFXP::id1_t TCTLoader::m_physicalInput = 0;

/**
 *  TypeCode of Physical Output Ports
 **/
SFXP::id1_t TCTLoader::m_physicalOutput = 0;

/**
 * Map of TypeCode with Plugin's directory
 **/
TCTLoader::TypeCodeTable TCTLoader::m_tctable = TCTLoader::TypeCodeTable();

/**
 * Function that load tctable.xml file
 *  if physical io are missing program must abort because audio
 *  chain will be broken
 * Duplicated type codes will be ignored and warnings will be
 *  raised on log
 * return true if physical input or output are missing or critical error
 *  happened
 **/
bool TCTLoader::loadTypeCodeTable(std::string path){

    SFXPlog::log(NAME) << "Load TypeCode Table Config File" << endl;

    try {
        
        // Open the XMLDocument
        XMLDocument config;
        if (config.LoadFile((path+TypeCodeFile).c_str())) {

            SFXPlog::err(NAME) << "#" << config.ErrorID() << " : "
                <<
                #ifdef __ARCH_LINUX__
                    config.ErrorStr()
                #elif defined(__LUBUNTU__)
                    config.Error()
                #else
                    #error
                #endif
                 << endl;
            
            throw string("Failed open Configuration File : " + path + TypeCodeFile);
        }
        
        // Get root Node
        XMLNode* root = config.FirstChildElement("tctable");
        if (root == NULL) {

            throw string("Failed get root Node");
        }

        // Load PhysicalInput TypeCode
        XMLElement* phyi = root->FirstChildElement("PhysicalInput");
        if (phyi == NULL) {

            throw string("Failed get Physical Input Config");
        }
        unsigned int buff;
        if (phyi->QueryUnsignedAttribute("TypeCode", &buff)) {

            throw string("Failed get Physical Input TypeCode");
        }
        m_physicalInput = buff;

        // Load PhysicalOut TypeCode
        XMLElement* phyo = root->FirstChildElement("PhysicalOutput");
        if (phyo == NULL) {

            throw string("Failed get Physical Output Config");
        }
        if (phyo->QueryUnsignedAttribute("TypeCode", &buff)) {

            throw string("Failed get Physical Output TypeCode");
        }
        m_physicalOutput = buff;

        SFXPlog::log(NAME) << "Registered Physical IO TypeCodes : Input( "
            << m_physicalInput << " ) Output( " << m_physicalOutput
            << " )" << endl;

        // Parse Document
        
        XMLNode* node = root->FirstChild();
        while(node){
    
            XMLElement* elem = node->ToElement();
            if (elem) {

                if (string(elem->Name()) == "Plugin") {

                    unsigned int tc;
                    string dir;
                    if (elem->QueryUnsignedAttribute("TypeCode", &tc)
                        || (dir = elem->Attribute("Dir")) == "") {

                        SFXPlog::wrn(NAME) << "Failed get Plugin"
                            << " Attribute" << endl;
                        
                        node = node->NextSibling();
                        continue;
                    }

                    if (m_tctable.find(tc) == m_tctable.end()) {

                        m_tctable[tc] = dir;
                        
                        #ifdef __DEBUG__
                        if (SFXP::GlobalIsDebugEnabled)
                            SFXPlog::debug(NAME) << "Parsed Plugin : "
                                << dir << " on TC : " << tc << endl;
                        #endif
                    }
                    else{

                        SFXPlog::wrn(NAME) << "Duplicated TC : "
                        << tc << " Already used by : " << m_tctable[tc]
                        << " ... Can't load : " << dir << endl;
                    }
                }
            }
        
            node = node->NextSibling();
        }
    }
    catch (string & e) {

        SFXPlog::err(NAME) << e << endl;
        return true;
    }
    return false;
}

/**
 * Function to get TypeCode of Physical Input Ports
 **/
SFXP::id1_t TCTLoader::physicalInput(){

    return m_physicalInput;
}

/**
 * Function to get TypeCode of Physical Output Ports
 **/
SFXP::id1_t TCTLoader::physicalOutput(){

    return m_physicalOutput;
}

/**
 * Function to get map of (TypeCode, Plugin directory) pairs
 **/
TCTLoader::TypeCodeTable TCTLoader::typeCodeTable(){

    return m_tctable;
}
