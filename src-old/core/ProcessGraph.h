/**********************************************************************
 * @file ProcessGraph.h
 * @author TomatoPi
 * @version 1.0
 *
 * Program's Core structure
 *  Contains list of all effects instances and connection graph
 **********************************************************************/
#ifndef DEF_PROCESS_GRAPH_H
#define DEF_PROCESS_GRAPH_H

#include <iostream>
#include <set>
#include <map>
#include <jack/jack.h>

#include "lib/unit/AbstractEffectUnit.h"
#include "core/EffectFactory.h"
#include "lib/graph/Connection.h"

#include "Const.h"

class ProcessGraph{
    
    public :
    
        typedef std::map<SFXP::id1_t,AbstractEffectUnit*> EffectGraph;
        typedef std::set<Connection> ConnectionGraph;
        
        /* **************** Graph Building ******************* */
    
        /**
         * Function to Create and Destroy ProcessGraph
         *  return 0 on success
         **/
        static int buildProcessGraph();
        static int killProcessGraph();
        
        /* ***************** Effect Managment ***************** */
    
        /**
         * Function to build add an effect to the graph from it type 
         *  and id
         *  return 0 on success
         **/
        static int addEffect(SFXP::tc_t type, SFXP::id1_t id);
        /**
         * Function to add an effect that already exist to process graph
         *  If effect's id is already used, override it if overide is true
         *  return 0 on success
         **/
        static int addEffect(AbstractEffectUnit* effect, bool overide=false);
        /**
         * Function to remove an effect from process Graph
         *  return 0 on success
         **/
        static int removeEffect(SFXP::id1_t id);
        /**
         * Function to get an effect from it id
         *  return null if id is not found
         **/
        static AbstractEffectUnit* getEffect(SFXP::id1_t id);
        /**
         * Function to get list of all used ids
         **/
        static std::vector<SFXP::id1_t> getEffectList();

        /* **************** Connection Managment ************* */

        /**
         * Function to add a connection to the ProcessGraph
         *  If given connection already exist do nothing
         *  return 0 on success
         **/
        static int addConnection(Connection c);
        /**
         * Function to remove a connection from the ProcessGraph
         *  return 0 on success
         **/
        static int removeConnection(Connection c);
        /**
         * Function to get list of all current connections
         **/
        static ConnectionGraph getConnectionList();
        
        /* ****************** Graph Managment ************** */
        /**
         * Function that remove all effects from the graph and clear
         *  Connection list
         **/
        static void clearGraph();
        /**
         * Function to pass ProcessGraph content to an output stream
         **/
        static std::ostream& print(std::ostream& os);

    public :

        static const int ErrIdNotUnique   = 1;
        static const int ErrInvalidEffect = 2;
        static const int ErrIdNotFound    = 3;
        static const int ErrInvalidId     = 4;
        static const int ErrUnknownError  = 5;

    private :

        /**
         * Function to get name of a given port
         *  return empty string if port not found
         **/
        static std::string buildPortName(JACKUnit::PortType pt, SFXP::id1_t id, SFXP::usize_t idx);
        
    private :
    
        /** List of all Connections **/
        static ConnectionGraph m_connection;
    
        /** List of all effects instances **/
        static EffectGraph m_graph;

        /** Client representing All programm for JACK **/
        static jack_client_t* m_client;
};

#endif
