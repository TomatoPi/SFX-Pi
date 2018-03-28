/**********************************************************************
 * @file EffectHandler.cc
 * @author TomatoPi
 * @version 1.1
 *
 * File providing main program's handler that will store all effect units
 **********************************************************************/
#include "EffectHandler.h"

using namespace SFXP;

namespace {

    static const char* portTypeString(PortType t) {

        if (t == PortType::AudioIn || t == PortType::AudioOut)
            return JACK_DEFAULT_AUDIO_TYPE;
        return JACK_DEFAULT_MIDI_TYPE;
    }
}

EffectHandler::EffectHandler(bool nojackRun):AbstractHandler("EffectHandler"),
_nojack(nojackRun),
_graph(EffectGraph()),
_client(nullptr),
_preset(nullptr)
{
    try{

        if (!_nojack) {
            // Create Handler's Jack Client
            jack_status_t status;

            _client = jack_client_open( "SpaceFX-Software", JackNullOption, &status, SFXP::JACK_SERVER );
            if ( _client == NULL ){

                throw string("Unable to connect JACK server");
            }
            if ( status & JackNameNotUnique ){

                cout << "Unique Name " << jack_get_client_name(_client) << " Assigned\n";
            }

            cout << "ProcessGraph : Registered Graph Client : " << jack_get_client_name(_client) << endl;
        }
        _status = HANDLER_OK;
        SFXPlog::log(_name) << "Builded Handler" << endl;
    }
    catch (string const& e){

        cout << "ProcessGraph : Error : " << e << endl;
        _status = ERRORED;
    }
}
EffectHandler::~EffectHandler() {

    this->clearGraph();
    if (!_nojack) {
        jack_client_close(_client);
    }
}

/**
 * Function to know if the handler contruction is successfull
 * @return non-zero error code if not
 **/
int EffectHandler::errored() {

    return _status;
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void EffectHandler::pushEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_PresetChanged) {

        this->eventPresetChanged(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (!_preset) {

            SFXPlog::err(_name) << "No Preset Given to the Handler" << endl;
        }
        else {
                
            SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
        }
    }
    else if (_preset) {
        
        switch(event->_type) {

            case SFXPEvent::Type::Event_EffectAddBank :

                this->eventAddBank(event);
                break;

            case SFXPEvent::Type::Event_EffectRemoveBank :

                this->eventRemoveBank(event);
                break;

            case SFXPEvent::Type::Event_EffectChangeBank :

                this->eventChangeBank(event);
                break;

            case SFXPEvent::Type::Event_EffectEditParam :

                this->eventEditParam(event);
                break;

            case SFXPEvent::Type::Event_GraphConnect :

                this->eventConnect(event);
                break;

            case SFXPEvent::Type::Event_GraphDisconnect :

                this->eventDisconnect(event);
                break;

            case SFXPEvent::Type::Event_AddEffect :

                this->eventAddEffect(event);
                break;

            case SFXPEvent::Type::Event_RemoveEffect :

                this->eventRemoveEffect(event);
                break;

            case SFXPEvent::Type::Event_ClearGraph :

                this->clearGraph();
                break;

            case SFXPEvent::Type::Event_ShowGraph :

                this->printGraph();
                break;

            case SFXPEvent::Type::Event_ListAvailable :

                this->eventListAvailable();
                break;

            case SFXPEvent::Type::Event_EffectShow :

                this->eventShowEffect(event);
                break;

            case SFXPEvent::Type::Event_EffectShowPool :

                this->eventShowEffectPool(event);
                break;

            default :

                SFXPlog::err(_name) << "Unhandled Event : " << (*event) << endl;
        }
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void EffectHandler::run() {

}

void EffectHandler::clearGraph() {

    if (!_nojack) {
        for (auto& e : _graph) {

            // Call Plugin's destructor to properly delete the effect
            EffectFactory::getPlugin(e.second->type())->destroy(e.second);
        }
    }
    _graph.clear();

    if (_preset) {

        _preset->_connections.clear();
        for (auto& p : _preset->_paramSets) delete p.second;
        _preset->_paramSets.clear();
    }
}

void EffectHandler::printGraph() {

    if (_preset) {

        if (!_nojack) {
            SFXPlog::log(_name) << "Current Loaded Effects :" << endl;
            for (auto& e : _graph) {

                cout << "   -> ID : " << e.first << " : TYPE : " << e.second->type() << endl;
            }
        }
        else {
            SFXPlog::log(_name) << "NoJack : Current Loaded Effects :" << endl;
            for (auto& e : _preset->_paramSets) {

                cout << "   -> ID : " << e.first << " : TYPE : " << e.second->type() << endl;
            }
        }
        
        SFXPlog::log(_name) << "Current Made Connections :" << endl;
        for (auto& c : _preset->_connections) {

            cout << "   -> " << c << endl;
        }
    }
    else {

        SFXPlog::err(_name) << "No Preset Loaded" << endl;
    }
}

void EffectHandler::connect(Connection c) {

    if (_preset) {

        // Verify that connection doesn't exist
        if (_preset->_connections.find(c) == _preset->_connections.end()) {

            SFXP::PortType sourceType = (c.m_midi)?SFXP::PortType::MidiOut:SFXP::PortType::AudioOut;
            SFXP::PortType targetType = (c.m_midi)?SFXP::PortType::MidiIn :SFXP::PortType::AudioIn;

            try{
                
                string source = buildPortName(sourceType,c.m_si,c.m_sp);
                string target = buildPortName(targetType,c.m_ti,c.m_tp);

                if (source == "" || target == "")
                    throw string("Invalid Source or Target Port" + source + " : " + target);

                if (!_nojack) {
                    
                    if (c.m_si == TCTLoader::physicalInput()) {
                            
                        _graph[c.m_ti]->getOwner(targetType, c.m_tp)->connect(source, target);
                    }
                    else {

                        _graph[c.m_si]->getOwner(sourceType, c.m_sp)->connect(source, target);
                    }
                }
                else {

                    SFXPlog::wrn(_name) << "NoJackRun : Cannot verify that connection is valid" << endl;
                }
            
                _preset->_connections.insert(c);

                return;
            }
            catch (string const& e) {

                cout << "ProcessGraph : Error : " << e << endl;
            }
            catch (...) {

                cout << "ProcessGraph : Error : Unknown Error" << endl;
            }
            return;
        }
        cout << "ProcessGraph : Error : Connection : " << c << " Already Exist" << endl;
        return;
    }
    else {

        SFXPlog::err(_name) << "Preset is Null" << endl;
    }
}
void EffectHandler::disconnect(Connection c) {

    if (_preset) {

        // Verify that connection exist
        if (_preset->_connections.find(c) != _preset->_connections.end()) {

            SFXP::PortType sourceType = (c.m_midi)?SFXP::PortType::MidiOut:SFXP::PortType::AudioOut;
            SFXP::PortType targetType = (c.m_midi)?SFXP::PortType::MidiIn :SFXP::PortType::AudioIn;

            try{
                
                string source = buildPortName(sourceType,c.m_si,c.m_sp);
                string target = buildPortName(targetType,c.m_ti,c.m_tp);

                if (source == "" || target == "")
                    throw string("Invalid Source or Target Port" + source + " : " + target);

                if (!_nojack) {
                
                    if (c.m_si == TCTLoader::physicalInput()) {
                            
                        _graph[c.m_ti]->getOwner(targetType, c.m_tp)->disconnect(source, target);
                    }
                    else {

                        _graph[c.m_si]->getOwner(sourceType, c.m_sp)->disconnect(source, target);
                    }
                }
                
                _preset->_connections.erase(c);

                return;
            }
            catch (string const& e) {

                cout << "ProcessGraph : Error : " << e << endl;
                return;
            }
            catch (...) {

                cout << "ProcessGraph : Error : Unknown Error" << endl;
                return;
            }
        }

        cout << "ProcessGraph : Error : Connection : " << c << " Doesn't Exist" << endl;
        return;
    }
    else {

        SFXPlog::err(_name) << "Preset is Null" << endl;
    }
}

void EffectHandler::eventPresetChanged(SFXPEvent* event) {

    if (Preset* np = (Preset*)event->_preset._preset) {

        this->clearGraph();

        // First create effects from param sets
        if (!_nojack) {
        for (auto& p : np->_paramSets) {

            Plugin* plugin = EffectFactory::getPlugin(p.second->type());
            if (plugin) {

                // Create effect
                AbstractEffectUnit* unit = plugin->build(p.second->type(), p.second->id());
                if (unit) {

                    // Link effect
                    unit->attachParamSet(p.second);
                    // Add Effect
                    _graph[p.second->id()] = unit;

                    // Update it
                    SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_EffectUpdateAll);
                    unit->update(&e);
                }
            }
        }
        }

        // Connect effects together
        for (auto& c : np->_connections) {

            this->connect(c);
        }

        _preset = np;
    }
}

void EffectHandler::eventAddBank(SFXPEvent* event) {

    id1_t id = event->_effect._id;
    id1_t idx = (id1_t) event->_effect._idx;

    // Verify that ParamSet exist
    if (_preset->_paramSets.find(id) != _preset->_paramSets.end()) {

        // Add a new Bank
        _preset->_paramSets[id]->addBank(idx);
    }
    else {

        SFXPlog::err(_name) << "Param Set Doesn't exist : "
            << id << endl;
    }
}
void EffectHandler::eventRemoveBank(SFXPEvent* event) {

    id1_t id = event->_effect._id;
    id1_t idx = (id1_t) event->_effect._idx;

    // Verify that ParamSet exist
    if (_preset->_paramSets.find(id) != _preset->_paramSets.end()
        && (_nojack || (!_nojack && _graph.find(id) != _graph.end()) )
        )
    {
        // Remove the Bank
        _preset->_paramSets[id]->removeBank(idx);

        // Update linked effect
        if (!_nojack) {
            _graph[id]->attachParamSet(_preset->_paramSets[id]);
        }
    }
    else {

        SFXPlog::err(_name) << "Param Set Doesn't exist : "
            << id << endl;
    }
}
void EffectHandler::eventChangeBank(SFXPEvent* event) {

    if (!_nojack) {
        
        // Verify that targeted effect exist
        id1_t id = event->_effect._id;
        id1_t idx = (id1_t) event->_effect._idx;

        if (_graph.find(id) != _graph.end()) {

            _graph[id]->setBank(idx);
        }
    }
}
void EffectHandler::eventEditParam(SFXPEvent* event) {

    // Update effect param set
    id1_t id = event->_effect._id;
    id1_t idx = (id1_t) event->_effect._idx;
    float v = event->_effect._value;

    if (_preset->_paramSets.find(id) != _preset->_paramSets.end()
        && (_nojack || (!_nojack && _graph.find(id) != _graph.end()) )
        )
    {
        // Edit param set
        _preset->_paramSets[id]->editBank(id, idx, v);

        if (!_nojack) {
            
            // Update linked effect
            _graph[id]->update(event);
        }
    }
}

void EffectHandler::eventConnect(SFXPEvent* event) {

    this->connect(
        Connection( event->_graph._source,
                    event->_graph._sport,
                    event->_graph._target,
                    event->_graph._tport,
                    event->_graph._midi)
                );
}
void EffectHandler::eventDisconnect(SFXPEvent* event) {

    this->disconnect(
        Connection( event->_graph._source,
                    event->_graph._sport,
                    event->_graph._target,
                    event->_graph._tport,
                    event->_graph._midi)
                );
}

void EffectHandler::eventAddEffect(SFXPEvent* event) {

    id1_t id(event->_edit._id);
    tc_t type(event->_edit._type);

    try {

        // Verify that id is not already used
        if (_preset->_paramSets.find(id) != _preset->_paramSets.end()
            && (_nojack || (!_nojack && _graph.find(id) != _graph.end()) )
            )
        {
            throw std::string("Id Already Used : ") + std::to_string(id);
        }

        // Get the correct Plugin
        Plugin* plugin = EffectFactory::getPlugin(type);

        if (!plugin) throw std::string("TypeCode Not Found ") +
            std::to_string(type);

        // Create a param set
        _preset->_paramSets[id] = new ParamSet(id, type, plugin->paramCount());

        if (!_nojack) {
            
            // Build the EffectUnit
            AbstractEffectUnit* unit = plugin->build(type, id);

            if (!unit) throw std::string("Failed Build EffectUnit");
            
            // Attach the unit
            unit->attachParamSet(_preset->_paramSets[id]);

            // Add the unit to the graph
            _graph[id] = unit;
        }

        SFXPlog::log(_name) << "Added Effect : ID:" << id << " TYPE:" << type << endl;
    }
    catch (const std::string& e) {

        SFXPlog::err(_name) << e << '\n';
    }
}
void EffectHandler::eventRemoveEffect(SFXPEvent* event) {

    id1_t id(event->_edit._id);

    if (_preset->_paramSets.find(id) != _preset->_paramSets.end()
            && (_nojack || (!_nojack && _graph.find(id) != _graph.end()) )
        )
    {
        if (!_nojack) {
                
            // Destroy the effect and remove it from the graph
            EffectFactory::getPlugin(_graph[id]->type())->destroy(_graph[id]);
            _graph.erase(id);
        }

        // Remove the effect from the preset
        delete _preset->_paramSets[id];
        _preset->_paramSets.erase(id);
    }
}

void EffectHandler::eventListAvailable() {

    SFXPlog::log(_name) << "Loaded Plugins : " << endl;
    EffectFactory::print();
}
void EffectHandler::eventShowEffect(SFXPEvent* event) {

    id1_t id(event->_effect._id);

    if (!_nojack) {
        
        if (_graph.find(id) != _graph.end()) {

            _graph[id]->printEffect();
        }
        else {

            SFXPlog::err(_name) << "Effect not Found : " << id << endl;
        }
    }
}
void EffectHandler::eventShowEffectPool(SFXPEvent* event) {

    id1_t id(event->_effect._id);

    if (!_nojack) {
            
        if (_graph.find(id) != _graph.end()) {

            _graph[id]->printPool();
        }
        else {

            SFXPlog::err(_name) << "Effect not Found : " << id << endl;
        }
    }
}
/**
 * Function to get name of a given port
 *  return empty string if port not found
 **/
std::string EffectHandler::buildPortName(SFXP::PortType pt, SFXP::id1_t id, SFXP::usize_t idx) {

    unsigned long flag = 0;

    // If requested id is Physical IO, set correct flag
    if (id == TCTLoader::physicalInput()) {

        flag = JackPortIsPhysical|JackPortIsOutput;
    }
    else if (id == TCTLoader::physicalOutput()) {

        flag = JackPortIsPhysical|JackPortIsInput;
    }

    // If requested id is Virtual IO
    if (!flag) {

        if (_preset->_paramSets.find(id) != _preset->_paramSets.end()
            && (_nojack || (!_nojack && _graph.find(id) != _graph.end()) )
            )
        {

            Plugin* plugin = EffectFactory::getPlugin
                                (_preset->_paramSets[id]->type());

            if (idx >= plugin->portCount()[pt]) {

                return string("Invalid_Port_Index");
            }

            // return "IdOfEffect:NameOfPort"
            const std::string** ports = plugin->portName();
            std::string rtn = to_string(id) + ":" + ports[pt][idx];
            PLUGIN_DELETE_PORT_NAME_ARRAY(ports);
            
            return rtn;
        }
    }
    // Else return Physical Port's name
    else{

        if (_nojack) return "PhysicalIO";

        const char** port = jack_get_ports(_client, NULL, portTypeString(pt), flag);

        if (port == NULL) return "";
        
        string rtn = port[idx];
        free(port);
        
        return rtn;
    }

    return "";
}
