/**********************************************************************
 * @file ProcessGraph.h
 * @author TomatoPi
 * @version 1.0
 *
 * Main Processing Object, contains array of all curently activated
 * Effects
 * Each Effect can be called by it ID
 **********************************************************************/
#include "ProcessGraph.h"
/**
 * Private Constructor and Destructor for Singleton pattern
 **/
ProcessGraph::ProcessGraph():
    m_connectionList(ConnectionTree(ValidValCo, ErrorValCo)),
    m_graph(EffectList())
{

    printf("Build Process Graph ... ");
    try{

        // Create Capture and Playback Modules
        EndUnit *capture  = (EndUnit*) UnitFactory::createEffect(
            SFXP::TC_CAPTURE, SFXP::TC_CAPTURE );
        EndUnit *playback = (EndUnit*) UnitFactory::createEffect(
            SFXP::TC_PLAYBACK, SFXP::TC_PLAYBACK );

        if ( capture == NULL || playback == NULL ){

            throw std::string("Failed Build Boundary Unit");
        }
        m_graph[SFXP::TC_CAPTURE] = capture;
        m_graph[SFXP::TC_PLAYBACK] = playback;

        // Connect Physical Capture Ports
        const char **port;
        port = jack_get_ports(capture->getJACKUnit(0)->getClient(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);

        if ( port == NULL ){
            throw std::string("No Physical Capture Ports");
        }

        capture->getJACKUnit(0)->connect( port[0], UnitFactory::buildPortName( capture, 0, 0 ));
        capture->getJACKUnit(1)->connect( port[1], UnitFactory::buildPortName( capture, 0, 1 ));
        free(port);

        // Connect Physical Playback Ports
        port = jack_get_ports(playback->getJACKUnit(0)->getClient(), NULL, NULL, JackPortIsPhysical|JackPortIsInput);

        if ( port == NULL ){
            throw std::string("No Physical Playback Ports");
        }

        playback->getJACKUnit(0)->connect( UnitFactory::buildPortName( playback, 1, 0 ), port[0] );
        playback->getJACKUnit(1)->connect( UnitFactory::buildPortName( playback, 1, 1 ), port[1] );
        free(port);

        // Process Graph OK Mudda Fukkazz
        printf("Done\n");
    }
    catch( std::string const& e ){

        printf("ERROR : %s\n", e.c_str());
        SFXP::GlobalNeedToExit = true;
    }
}
ProcessGraph::~ProcessGraph(){

    printf("Destroy Process Graph ... ");
    this->clearGraph();

    printf("Delete Capture and Playback Units ... ");
    for ( auto& cur : m_graph ){

        delete cur.second;
    }
    printf("Done\n");
}

/**
 * Add or remove given effect from the process graph
 **/
int ProcessGraph::addEffect( uint8_t type, uint8_t id ){

    printf("Add New Effect ... ");
    // Verify that Provided Id is Unique
    if ( m_graph.find(id) != m_graph.end() ){

        printf("ERROR : Id Is Not Unique\n");
        return ErrIdNotUnique;
    }

    // Try Create New Effect
    AbstractEffectUnit* effect;
    try{

        // Build New Effect
        effect = UnitFactory::createEffect( type, id );
        if ( effect == NULL )
            throw std::string("Invalid Effect Type");

        // Add It To te Graph
        m_graph[id] = effect;

        printf("New Effect Added : Type( %2u ) Id( %2u ) UniqueName( %-18s )\n",
            type, id, effect->getJACKUnit(0)->getName().c_str());
    }
    catch( std::string const& e ){

        printf("ERROR : %s\n", e.c_str());
        return ErrInvalidEffect;
    }
    return 0;
}
int ProcessGraph::addEffect( AbstractEffectUnit* unit, bool override ){

    printf("Add Given Effect ... ");

    // If given effect is a capture or playback unit, copy it parameters
    if ( unit->getID() == SFXP::TC_CAPTURE || unit->getID() == SFXP::TC_PLAYBACK ){

        printf("Given effect is a limit unit : copy parameters ... ");
        AbstractEffectUnit* end = m_graph[unit->getID()];

        // clear it banks
        end->clearEffect();

        //get new banks
        std::map<uint8_t,float*> banks = unit->getAllBanks();
        uint8_t size = unit->getBankSize();

        for ( auto& cur : banks ){

            end->addBank( cur.first, size, cur.second );
        }

        printf("Done\n");
        return 1;
    }
    
    // Verify that Provided Id is Unique
    if ( m_graph.find(unit->getID()) != m_graph.end() ){

        // If override is enabled and effects are of same type, replace banks
        if ( override && unit->getType() == m_graph[unit->getID()]->getType() ){

            printf("Given effect is already present : copy parameters ... ");
            AbstractEffectUnit* old = m_graph[unit->getID()];

            // clear it banks
            old->clearEffect();

            //get new banks
            std::map<uint8_t,float*> banks = unit->getAllBanks();
            uint8_t size = unit->getBankSize();

            for ( auto& cur : banks ){

                old->addBank( cur.first, size, cur.second );
            }

            printf("Done\n");
            return 0;
        }
        else{
                
            printf("ERROR : Id Is Not Unique\n");
        }
        return ErrIdNotUnique;
    }

    // Add It To te Graph
    m_graph[unit->getID()] = unit;

    printf("Given Effect Added : Type( %2u ) Id( %2u ) UniqueName( %-18s )\n",
        unit->getType(),
        unit->getID(),
        unit->getJACKUnit(0)->getName().c_str() );

    return 0;
}
int ProcessGraph::removeEffect( uint8_t id ){

    if ( id == SFXP::TC_CAPTURE || id == SFXP::TC_PLAYBACK ){

        printf("Warning : Attempt to delete Playback or Capture Unit ... Abort\n");
        return ErrInvalidId;
    }

    printf("Remove Effect ( %2u ) From Graph ... ", id);
    if ( m_graph.find(id) != m_graph.end() ){

        std::vector<Connection> c = this->getConnection();

        for ( auto& cur : c ){

            if ( cur.m_si == id || cur.m_ti == id ){

                this->removeConnection(cur);
            }
        }

        delete m_graph[id];
        m_graph.erase(id);

        printf("Done\n");
        return 0;
    }

    printf("ERROR : Effect Not Found\n");
    return ErrIdNotFound;
}
AbstractEffectUnit* ProcessGraph::getEffect( uint8_t id ){

    if ( m_graph.find(id) != m_graph.end() ){

        return m_graph[id];
    }
    
    return NULL;
}

std::vector<uint8_t> ProcessGraph::getEffectList() const{

    std::vector<uint8_t> out;
    for ( auto& cur : m_graph ){

        out.push_back(cur.first);
    }
    return out;
}

/**
 * Add or remove connection from the process graph
 **/
int ProcessGraph::addConnection( Connection c ){

    int co[4] = {c.m_si, c.m_sp, c.m_ti, c.m_tp};
    printf("Add Connection( %3d %2d %3d %2d ) ... ", co[0], co[1], co[2], co[3]);

    if ( m_connectionList.find(co) == ValidValCo ){

        printf("Connection Already Exist");
        return 1;
    }

    // Verify that Given Effects Exist
    if ( m_graph.find(co[0]) != m_graph.end() && m_graph.find(co[2]) != m_graph.end() ){

        std::string source = UnitFactory::buildPortName( m_graph[co[0]], 1, co[1] );
        std::string target = UnitFactory::buildPortName( m_graph[co[2]], 0, co[3] );

        try{
            
            m_graph[co[0]]->getJACKUnit(co[1])->connect( source, target );

            m_connectionList.add( co );
        }
        catch( std::string const& e ){

            printf("ERROR : %s", e.c_str());
            return 1;
        }
    }
    else{

        printf("ERROR : Effects Not Found\n");
        return 1;
    }

    printf("Connection Added\n");
    
    return 0;
}
int ProcessGraph::removeConnection( Connection c ){

    int co[4] = {c.m_si, c.m_sp, c.m_ti, c.m_tp};
    printf("Remove Connection( %3d %2d %3d %2d ) ... ", co[0], co[1], co[2], co[3]);

    if ( m_connectionList.find(co) == ErrorValCo ){

        printf("Error Given Connection Is Not Present\n");
        return 1;
    }
    
    // Verify that Given Effects Exist
    if ( m_graph.find(co[0]) != m_graph.end() && m_graph.find(co[2]) != m_graph.end() ){

        std::string source = UnitFactory::buildPortName( m_graph[co[0]], 1, co[1] );
        std::string target = UnitFactory::buildPortName( m_graph[co[2]], 0, co[3] );

        try{
            
            m_graph[co[0]]->getJACKUnit(co[1])->disconnect( source, target );

            m_connectionList.remove(co);
        }
        catch( std::string const& e ){

            printf("ERROR : %s", e.c_str());
            return 1;
        }
    }
    else{

        printf("ERROR : Effects are Invalid\n");
        return 1;
    }
    printf("Connection Removed\n");
    
    return 0;
}
std::vector<ProcessGraph::Connection> ProcessGraph::getConnection(){

    std::vector<int*> tree = m_connectionList.getBackTree();
    std::vector<Connection> out;

    for( auto& cur : tree ){

        out.push_back(Connection( cur[0], cur[1], cur[2], cur[3] ));
        delete cur;
    }
    tree.clear();
    
    return out;
}

void ProcessGraph::clearGraph(){

    printf("Clear Connections List ... \n");
    std::vector<Connection> co = this->getConnection();
    for( auto& cur : co ){

        this->removeConnection(cur);
    }
    printf("... Done\n");

    printf("Clear Process Graph ... \n");
    for( auto& cur : m_graph ){

        if ( cur.second->getID() != SFXP::TC_CAPTURE && cur.second->getID() != SFXP::TC_PLAYBACK ){

            printf("Remove Effect From Graph : Id( %2u )\n", cur.second->getID());
            delete cur.second;
            m_graph.erase( cur.first );
        }
    }
    printf("... Done\n");
}

void ProcessGraph::printGraph(){

    printf("Process Graph :\n");
    
    printf("   Effects :\n");
    for ( auto& cur : m_graph ){

        printf("      - %2u : %s\n", cur.first, cur.second->getJACKUnit(0)->getName().c_str() );
    }
    
    printf("   Connections :\n");
    std::vector<int*> co = m_connectionList.getBackTree();
    for ( auto& cur : co ){

        printf("      - From : %3i:%i To : %3i:%i\n", cur[0], cur[1], cur[2], cur[3]);
        delete cur;
    }
    printf("End\n");
}
