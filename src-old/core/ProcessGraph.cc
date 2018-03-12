/**********************************************************************
 * @file ProcessGraph.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Program's Core structure
 *  Contains list of all effects instances and connection graph
 **********************************************************************/
#include "ProcessGraph.h"

using namespace std;

const char* ErrString[] = {
            "No_Error",
            "Id_Not_Unique",
            "Invalid_Effect_Type",
            "Id_Not_Found",
            "Invalid_Id",
            "Unknown_Error"};

/** List of all Connections **/
ProcessGraph::ConnectionGraph ProcessGraph::m_connection
    = ProcessGraph::ConnectionGraph();

/** List of all effects instances **/
ProcessGraph::EffectGraph ProcessGraph::m_graph
    = ProcessGraph::EffectGraph();

/** Client representing All programm for JACK **/
jack_client_t* ProcessGraph::m_client = NULL;

/* ************************** Graph Building ************************ */

/**
 * Function to Create and Destroy ProcessGraph
 *  return 0 on success
 **/
int ProcessGraph::buildProcessGraph(){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Build Graph" << endl;

    try{

        jack_status_t status;

        m_client = jack_client_open( "SpaceFX-Software", JackNullOption, &status, SFXP::JACK_SERVER );
        if ( m_client == NULL ){

            throw string("Unable to connect JACK server");
        }
        if ( status & JackNameNotUnique && SFXP::GlobalIsDebugEnabled ){

            cout << "Unique Name " << jack_get_client_name(m_client) << " Assigned\n";
        }

        cout << "ProcessGraph : Registered Graph Client : " << jack_get_client_name(m_client) << endl;

    }
    catch (string const& e){

        cout << "ProcessGraph : Error : " << e << endl;
        return 1;
    }

    return 0;
}
int ProcessGraph::killProcessGraph(){
    
    // Delete all effects
    clearGraph();

    // Close Graph's client
    jack_client_close(m_client);
    
    return 0;
}

/* ************************ Effect Managment ************************ */

/**
 * Function to build add an effect to the graph from it type 
 *  and id
 *  return 0 on success
 **/
int ProcessGraph::addEffect(SFXP::tc_t type, SFXP::id1_t id){
    
    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Create Effect : " << type << " : " << id << endl;
        
    try{
        
        // Verify that provided ID is unique
        if (m_graph.find(id) != m_graph.end())
            throw ErrIdNotUnique;
            
        // Create Effect
        AbstractEffectUnit* effect = EffectFactory::getPlugin(type)->build(type, id);
        if (effect == NULL)
            throw ErrInvalidEffect;
            
        // Add Effect
        m_graph[id] = effect;
        
        cout << "ProcessGraph : Created Effect : " << type << " : " << id << endl;
    }
    catch (int & e){
        
        cout << "ProcessGraph : Error : #" << e << " : " << ErrString[e] << endl;
        return e;
    }
    
    return 0;
}
/**
 * Function to add an effect that already exist to process graph
 *  If effect's id is already used, override it if overide is true
 *  return 0 on success
 **/
int ProcessGraph::addEffect(AbstractEffectUnit* effect, bool overide){
    
    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Copy Effect : " << effect->getID() << endl;
        
    try{
        
        SFXP::id1_t id = effect->getID();
        
        // Verify if provided id is unique
        if (m_graph.find(id) != m_graph.end() && !overide) {
            
            throw ErrIdNotUnique;
        }
        // If override is activated and effect are same type replace effect banks
        else if (overide) {
            
            if (effect->getType() == m_graph[id]->getType()) {
            
                AbstractEffectUnit* old = m_graph[id];

                // clear it banks
                old->clearEffect();

                //get new banks
                EffectPrototype::ModBankArray banks = effect->getAllBanks();
                SFXP::usize_t size = effect->getBankSize();

                for ( auto& cur : banks ){

                    old->addBank( cur.first, size, cur.second );
                }
            }
            // Throw error if effects are different
            else throw ErrInvalidEffect;
        }
        // Else add given effect
        else{
            
            m_graph[id] = effect;
        }
        
        cout << "ProcessGraph : Added Effect : " << effect->getType() 
            << " : " << id << endl;
    }
    catch (int & e){
        
        cout << "ProcessGraph : Error : #" << e << " : " << ErrString[e] << endl;
        return e;
    }
    return 0;
}
/**
 * Function to remove an effect from process Graph
 *  return 0 on success
 **/
int ProcessGraph::removeEffect(SFXP::id1_t id){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Remove Effect : " << (unsigned int)id << endl;

    // Verify that effect Exist
    if (m_graph.find(id) == m_graph.end()) {

        cout << "ProcessGraph : Error : #" << ErrIdNotFound << " : " << ErrString[ErrIdNotFound] << endl;
        return ErrIdNotFound;
    }

    // Remove All connection from or to this effect
    for (auto & c : m_connection){

        if (c.m_si == id || c.m_ti == id) {

            removeConnection(c);
        }
    }

    // Destroy the effect calling the correct destructor function
    EffectFactory::getPlugin(m_graph[id]->getType())->destroy(m_graph[id]);
    m_graph.erase(id);
    
    return 0;
}
/**
 * Function to get an effect from it id
 *  return null if id is not found
 **/
AbstractEffectUnit* ProcessGraph::getEffect(SFXP::id1_t id){
    
    if (m_graph.find(id) == m_graph.end()) return NULL;
    return m_graph[id];
}
/**
 * Function to get list of all used ids
 **/
std::vector<SFXP::id1_t> ProcessGraph::getEffectList(){
    
    std::vector<SFXP::id1_t> list;
    for (auto & effect : m_graph) {
        list.push_back(effect.first);
    }
    
    return list;
}

/* ********************** Connection Managment ********************** */

/**
 * Function to add a connection to the ProcessGraph
 *  If given connection already exist do nothing
 *  return 0 on success
 **/
int ProcessGraph::addConnection(Connection c){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Add Connection : " << c << endl;

    // Verify that connection doesn't exist
    if (m_connection.find(c) == m_connection.end()) {

        JACKUnit::PortType sourceType = (c.m_midi)?JACKUnit::PortType::MidiOut:JACKUnit::PortType::AudioOut;
        JACKUnit::PortType targetType = (c.m_midi)?JACKUnit::PortType::MidiIn :JACKUnit::PortType::AudioIn;

        try{
            
            string source = buildPortName(sourceType,c.m_si,c.m_sp);
            string target = buildPortName(targetType,c.m_ti,c.m_tp);

            if (source == "" || target == "")
                throw string("Invalid Source or Target Port" + source + " : " + target);

            if (c.m_si == TCTLoader::physicalInput()) {
                    
                m_graph[c.m_ti]->getOwner(targetType, c.m_tp)->connect(source, target);
            }
            else {

                m_graph[c.m_si]->getOwner(sourceType, c.m_sp)->connect(source, target);
            }
            
            m_connection.insert(c);

            return 0;
        }
        catch (string const& e) {

            cout << "ProcessGraph : Error : " << e << endl;
            return ErrIdNotFound;
        }
        catch (...) {

            cout << "ProcessGraph : Error : Unknown Error" << endl;
            return ErrUnknownError;
        }
    }

    cout << "ProcessGraph : Error : Connection : " << c << " Already Exist" << endl;
    return ErrIdNotUnique;
}
/**
 * Function to remove a connection from the ProcessGraph
 *  return 0 on success
 **/
int ProcessGraph::removeConnection(Connection c){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "ProcessGraph : Remove Connection : " << c << endl;

    // Verify that connection exist
    if (m_connection.find(c) != m_connection.end()) {

        JACKUnit::PortType sourceType = (c.m_midi)?JACKUnit::PortType::MidiOut:JACKUnit::PortType::AudioOut;
        JACKUnit::PortType targetType = (c.m_midi)?JACKUnit::PortType::MidiIn :JACKUnit::PortType::AudioIn;

        try{
            
            string source = buildPortName(sourceType,c.m_si,c.m_sp);
            string target = buildPortName(targetType,c.m_ti,c.m_tp);

            if (source == "" || target == "")
                throw string("Invalid Source or Target Port" + source + " : " + target);

            if (c.m_si == TCTLoader::physicalInput()) {
                    
                m_graph[c.m_ti]->getOwner(targetType, c.m_tp)->disconnect(source, target);
            }
            else {

                m_graph[c.m_si]->getOwner(sourceType, c.m_sp)->disconnect(source, target);
            }
            
            m_connection.erase(c);

            return 0;
        }
        catch (string const& e) {

            cout << "ProcessGraph : Error : " << e << endl;
            return ErrIdNotFound;
        }
        catch (...) {

            cout << "ProcessGraph : Error : Unknown Error" << endl;
            return ErrUnknownError;
        }
    }

    cout << "ProcessGraph : Error : Connection : " << c << " Doesn't Exist" << endl;
    return ErrIdNotFound;
}
/**
 * Function to get list of all current connections
 **/
ProcessGraph::ConnectionGraph ProcessGraph::getConnectionList(){

    return m_connection;
}

/* ************************* Graph Managment ************************ */

/**
 * Function that remove all effects from the graph and clear
 *  Connection list
 **/
void ProcessGraph::clearGraph(){

    // Clear Connection Graph
    for (auto & c : m_connection) {

        removeConnection(c);
    }

    // Clear Effect Graph
    for (auto & e : m_graph) {

        removeEffect(e.first);
    }
}
/**
 * Function to pass ProcessGraph content to an output stream
 **/
std::ostream& ProcessGraph::print(std::ostream& os){

    os << "Process Graph :" << endl << endl;
    os << "    - Physical Input  : " << TCTLoader::physicalInput() << endl;
    os << "    - Physical Output : " << TCTLoader::physicalOutput() << endl;
    
    os << endl;
    os << "  Effects :" << endl;

    /*
     * Print each effect with format :
     *    - EffectName(15 chars) : Type(EffectTypeCode(3 chars)) : ID(EffectID(3 chars))
     */
    for (auto & e : m_graph) {

        os << "    - ";
        os.width(15); os << left << e.second->getOwner(JACKUnit::PortType::AudioOut,0)->getName();
        os << " : Type(";
        os.width(3); os << left << e.second->getType();
        os << ") : ID(";
        os.width(3); os << left << e.first;
        os << ")"<< endl;
    }

    os << endl;
    os << "  Connections :" << endl;
    
    /*
     * Print each connection with format :
     *    - ConnectionType(Audio|Midi) : SourceID(3):SourcePort(2)->TargetID(3):TargetPort(2)
     */
    for (auto & c : m_connection) {

        os << "    - " << ((c.m_midi)?"Midi ":"Audio") << " : [";
        os.width(3); os << left << c.m_si;
        os << ":";
        os.width(2); os << left << c.m_sp;
        os << "] -> [";
        os.width(3); os << left << c.m_ti;
        os << ":";
        os.width(2); os << left << c.m_tp;
        os << "]" << endl;
    }

    return os << "End" << endl;
}
/**
 * Function to get name of a given port
 *  return empty string if port not found
 **/
std::string ProcessGraph::buildPortName(JACKUnit::PortType pt, SFXP::id1_t id, SFXP::usize_t idx){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "Build Port Name : " << JACKUnit::portTypeString(pt)
            << ":" << id << ":" << idx << endl;

    const char* type = JACKUnit::portTypeString(pt);
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

        if (m_graph.find(id) != m_graph.end()) {

            // return "UniqueNameOfEffect:NameOfPort"
            return m_graph[id]->getOwner(pt,idx)->getName() + ":" + EffectFactory::getPlugin(m_graph[id]->getType())->portName(pt,idx);
        }
    }
    // Else return Physical Port's name
    else {

        const char** port = jack_get_ports(m_client, NULL, type, flag);

        if (port == NULL) return "";
        
        string rtn = port[idx];
        free(port);
        
        return rtn;
    }

    return "";
}
