/**********************************************************************
 * @file PluginConfigLoader.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load a plugin from an xml file
 **********************************************************************/
#include "PluginConfigLoader.h"

using namespace SFXP;

namespace PluginConfigLoader{
/**
 * Function to load a plugin config from an XML file
 *  return true if an error occured during loading
 **/
bool loadConfig(Plugin* plugin, string path){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginConfigLoader : Load Configuration File : " << path << endl;

    // Open the XMLDocument
    XMLDocument config;
    if (config.LoadFile((path+ConfigFile).c_str())) {

        cout << "PluginConfigLoader : Error : Failed open Configuration File" << endl;
        cout << "PluginConfigLoader : Error : #" << config.ErrorID() << " : " << config.ErrorStr() << endl;

        return true;
    }

    // Get root Node
    XMLNode *root = config.FirstChildElement("config");
    if (root == NULL) {

        cout << "PluginConfigLoader : Error : Failed get root Node" << endl;
        return true;
    }

    bool failed = false;

    // Browse each Node
    XMLNode *node = root->FirstChild();
    while (node != NULL && !failed){

        XMLElement *elem = node->ToElement();

        if (elem != NULL) {

            string name = elem->Name();
            
            if (name == "description") {

                failed |= parseDescription(plugin, elem);
            }
            else if (name == "params") {

                failed |= parseParamList(plugin, elem);
            }
            else if (name == "ports") {

                failed |= parsePortList(plugin, elem);
            }
        }

        node = node->NextSibling();
    }

    return failed;
}

/**
 * Function to parse Plugin Description Node
 *  return true if an error occured during parsing
 **/
bool parseDescription(Plugin* plugin, XMLElement* desc){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginConfigLoader : Load Description" << endl;

    bool failed = false;

    XMLElement* nameNode = desc->FirstChildElement("name");
    if (nameNode != NULL) {

        string name = nameNode->GetText();
        failed |= plugin->registerName(name);
    }
    else {

        cout << "PluginConfigLoader : Error : Failed get Name Node" << endl;
        return true;
    }

    XMLElement* poolNode = desc->FirstChildElement("pool");
    if (poolNode != NULL) {

        unsigned int pool = poolNode->UnsignedText();
        failed |= plugin->registerPool(pool);
    }
    else {

        cout << "PluginConfigLoader : Error : Failed get Pool Node" << endl;
        return true;
    }

    return failed;
}

/**
 * Function to parse Plugin Parameter List Node
 *  return true if an error occured during parsing
 **/
bool parseParamList(Plugin* plugin, XMLElement* params){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginConfigLoader : Load Param List" << endl;

    // Get Expected param count
    unsigned int paramCount;
    if (params->QueryUnsignedAttribute("count", &paramCount)) {

        cout << "PluginConfigLoader : Error : Missing Param Count Attribute" << endl;
        return true;
    }

    // Get param List
    vector<string> paramList;
    
    XMLNode *node = params->FirstChild();
    bool inGroup = false;
    while (node != NULL){

        XMLElement *elem = node->ToElement();

        if (elem != NULL) {

            if (string(elem->Name()) == "group") {

                inGroup = true;
                node = node->FirstChild();
                continue;
            }
            else if (string(elem->Name()) == "param") {

                string param = parseParamNode(elem);
                
                if (param.length() != 0) {

                    paramList.push_back(param);
                }
            }
        }
        
        XMLNode* bak = node;
        node = node->NextSibling();
        
        if (inGroup && node == NULL){
            
            inGroup = false;
            node = bak->Parent()->NextSibling();
        }
    }

    // Verify that parsing is ok
    if (paramList.size() == paramCount) {

        return plugin->registerParams(paramList);
    }
    else {

        cout << "PluginConfigLoader : Error : Bad Param Count : Expected "
            << paramCount << " : Parsed " << paramList.size() << endl;
        return true;
    }
}

/**
 * Function to parse Plugin Port List Node
 *  return true if an error occured during parsing
 **/
bool parsePortList(Plugin* plugin, XMLElement* ports){

    
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PluginConfigLoader : Load Port List" << endl;

    // Get Expected param count
    unsigned int portCount[4];
    if (ports->QueryUnsignedAttribute("AudioIn", portCount)) {

        cout << "PluginConfigLoader : Error : Missing AudioIn Attribute" << endl;
        return true;
    }
    if (ports->QueryUnsignedAttribute("AudioOut", portCount+1)) {

        cout << "PluginConfigLoader : Error : Missing AudioOut Attribute" << endl;
        return true;
    }
    if (ports->QueryUnsignedAttribute("MidiIn", portCount+2)) {

        cout << "PluginConfigLoader : Error : Missing MidiIn Attribute" << endl;
        return true;
    }
    if (ports->QueryUnsignedAttribute("MidiOut", portCount+3)) {

        cout << "PluginConfigLoader : Error : Missing MidiOut Attribute" << endl;
        return true;
    }

    // Get Port List
    vector<string> portList[4];
    
    XMLNode *node = ports->FirstChild();
    while (node != NULL){

        XMLElement *elem = node->ToElement();

        if (elem != NULL) {

            if (string(elem->Name()) == "port") {

                pair<JACKUnit::PortType,string> port = parsePortNode(elem);
                
                if (port.second.length() != 0) {

                    portList[port.first].push_back(port.second);
                }
            }
        }
        node = node->NextSibling();
    }

    // Verify that parsing successed
    for (usize_t i = 0; i < 4; i++) {

        if (portCount[i] != portList[i].size()){

            cout << "PluginConfigLoader : Error : Bad Port Count : Type "
                << i << " : Expected " << portCount[i] << " : Parsed "
                << portList[i].size() << endl;
            return true;
        }
    }

    return plugin->registerPorts(portList);
}

/**
 * Function to get a param name from a param Node
 *  return an empty string if failed
 **/
string parseParamNode(XMLElement* param){

    XMLElement* nameNode = param->FirstChildElement("name");
    if (nameNode != NULL) {

        return nameNode->GetText();
    }
    return "";
}

/**
 * Function to get a port from a port Node
 *  return a pair<type_of_port,name_of_port>, empty name if failed
 **/
pair<JACKUnit::PortType,string> parsePortNode(XMLElement* port){

    bool success = true;

    JACKUnit::PortType  type;
    string              name = "";
    
    string t = port->Attribute("type");
    if (t == "AudioIn") type = JACKUnit::PortType::AudioIn;
    else if (t == "AudioOut") type = JACKUnit::PortType::AudioOut;
    else if (t == "MidiIn")   type = JACKUnit::PortType::MidiIn;
    else if (t == "MidiOut")  type = JACKUnit::PortType::MidiOut;
    else success = false;

    XMLElement* nameNode = port->FirstChildElement("name");
    if (nameNode != NULL && success) {

        name = nameNode->GetText();
    }

    return make_pair(type, name);
}
};
