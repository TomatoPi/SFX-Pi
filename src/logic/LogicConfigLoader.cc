/**********************************************************************
 * @file LogicConfigLoader.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load logical io configuration from an
 *  xml file
 **********************************************************************/
#include "LogicConfigLoader.h"

#define NAME ("LogicConfLoader")
namespace {

    static const std::string ConfigFile = "LogicConfig.xml";
};

using namespace tinyxml2;
using namespace SFXP;

LogicConfigLoader::LogicConfigLoader(std::string path):
_status(true),
_registers(),
_inputs(),
_outputs()
{

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAME) << "Load Configuration File : " << path + ConfigFile << endl;
    #endif

    // Open the XMLDocument
    XMLDocument config;
    if (config.LoadFile((path+ConfigFile).c_str())) {

        SFXPlog::err(NAME) << "Failed open Configuration File" << endl;
        SFXPlog::err(NAME) << "#" << config.ErrorID() << " : " <<
        #ifdef __ARCH_LINUX__
            config.ErrorStr()
        #elif defined(__LUBUNTU__)
            config.Error()
        #else
            #error
        #endif
        << endl;

        _status = true;
        return;
    }

    // Get root Node
    XMLNode *root = config.FirstChildElement("config");
    if (root == NULL) {

        SFXPlog::err(NAME) << "Failed get root Node" << endl;
        
        _status = true;
        return;
    }

    try {
        // Browse each Node
        XMLNode *node = root->FirstChild();
        while (node != NULL){

            XMLElement *elem = node->ToElement();

            if (elem != NULL) {

                string name = elem->Name();
                
                if (name == "MCP23017") {

                    RegisterConfig reg;

                    if (parseRegisterConfig(elem, reg))
                        throw string("Failed Parse MCP23017 Config");

                    _registers.push_back(reg);
                }
                else if (name == "Footswitch") {

                    FootConfig foot;

                    if (parseFootConfig(elem, foot))
                        throw string("Failed Parse Footswitch Config");
                        
                    _inputs.push_back(foot);
                }
                else if (name == "Led") {

                    LedConfig led;

                    if (parseLedConfig(elem, led))
                        throw string("Failed Parse Led Config");
                        
                    _outputs.push_back(led);
                }
                else {

                    SFXPlog::wrn(NAME) << "Unknown XMLElement : " << name << endl;
                }
            }

            node = node->NextSibling();
        }

        // Load Is Successfull
        _status = false;
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << e << endl;
        
        _registers.clear();
        _inputs.clear();
        _outputs.clear();
        _status = true;
    }
}
LogicConfigLoader::~LogicConfigLoader() {

    _registers.clear();
    _inputs.clear();
    _outputs.clear();
}

/**
 * Return true if Loader failed parse XML file
 **/
bool LogicConfigLoader::status() const {

    return _status;
}

std::vector<LogicConfigLoader::RegisterConfig> LogicConfigLoader::registers() const {

    return _registers;
}
std::vector<LogicConfigLoader::FootConfig> LogicConfigLoader::inputs() const {

    return _inputs;
}
std::vector<LogicConfigLoader::LedConfig> LogicConfigLoader::outputs() const {

    return _outputs;
}

/**
 * Functions to parse XMLElements
 * return non-zero if an error occured during parsing
 **/
int LogicConfigLoader::parseAdressConfig(tinyxml2::XMLElement* elem, AdressConfig& adr) {

    unsigned int adress;
    unsigned int index;
    bool gpio;

    try {

        if(elem->QueryUnsignedAttribute("Adress", &adress))
            throw string("Missing Adress");

        if(elem->QueryUnsignedAttribute("Index", &index))
            throw string("Missing Index");

        if(elem->QueryBoolAttribute("GPIO", &gpio))
            throw string("Missing GPIO");
/*
        SFXPlog::log(NAME) << "Parsed Adress : ADR:" << adress
            << " IDX:" << index << " GPIO:" << gpio << endl;
*/            
        adr = AdressConfig((hex_t)adress, (usize_t)(index), gpio);
        return 0;
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << "Adress : " << e << endl;
        return 1;
    }
}
int LogicConfigLoader::parseFootConfig(tinyxml2::XMLElement* foot, FootConfig& f) {

    id2_t id;
    string name;
    AdressConfig adr;
    LedConfig led;

    try {
        
        XMLElement* idNode = foot->FirstChildElement("ID");
        if (!idNode) throw string("Missing ID Node");
        id = idNode->UnsignedText();

        XMLElement* nameNode = foot->FirstChildElement("Name");
        if (!nameNode) throw string("Missing Name Node");
        name = nameNode->GetText();

        XMLElement* adrNode = foot->FirstChildElement("Adress");
        if (!adrNode) throw string("Missing Adress Node");
        if(parseAdressConfig(adrNode, adr))
            throw string("Broken Adress Node");

        XMLElement* ledNode = foot->FirstChildElement("Led");
        if (!ledNode) {

            f = FootConfig(id, name, adr, false, led);
        }
        else {
            
            if(parseLedConfig(ledNode, led))
                throw string("Broken Led Node");

            f = FootConfig(id, name, adr, true, led);
        }
/*
        SFXPlog::log(NAME) << "Parsed Footswitch : ID:" << id << " NAME:"
            << name << endl;
*/            
        return 0;
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << "Footswitch : " << e << endl;
        return 1;
    }
}
int LogicConfigLoader::parseLedConfig(tinyxml2::XMLElement* led, LedConfig& l) {

    id2_t id;
    AdressConfig adr;

    try {
        
        XMLElement* idNode = led->FirstChildElement("ID");
        if (!idNode) throw string("Missing ID Node");
        id = idNode->UnsignedText();

        XMLElement* adrNode = led->FirstChildElement("Adress");
        if (!adrNode) throw string("Missing Adress Node");
        if(parseAdressConfig(adrNode, adr))
            throw string("Broken Adress Node");
/*
        SFXPlog::log(NAME) << "Parsed LED : ID:" << id << endl;
*/
        l = LedConfig(id, adr);
        return 0;
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << "Led : " << e << endl;
        return 1;
    }
}
int LogicConfigLoader::parseRegisterConfig(tinyxml2::XMLElement* reg, RegisterConfig& r) {

    unsigned int adress;
    unsigned int index;

    try {

        if(reg->QueryUnsignedAttribute("Adress", &adress))
            throw string("Missing Adress");

        if(reg->QueryUnsignedAttribute("Index", &index))
            throw string("Missing Index");
/*
        SFXPlog::log(NAME) << "Parsed Register : ADR:" << adress <<
            " IDX:" << index << endl;
*/
        r = RegisterConfig((hex_t)adress, (usize_t)(index));
        return 0;
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << "Register : " << e << endl;
        return 1;
    }
}
