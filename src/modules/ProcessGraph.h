/**********************************************************************
 * @file ProcessGraph.h
 * @author TomatoPi
 * @version 1.0
 *
 * Main Processing Object, contains array of all curently activated
 * Effects
 * Each Effect can be called by it ID
 **********************************************************************/
#ifndef DEF_PROCESS_GRAPH_H
#define DEF_PROCESS_GRAPH_H

#include <map>
#include <vector>
#include <jack/jack.h>

#include "../Const.h"

//#include "../core/Singleton.h"
#include "../core/ReminderTree.h"

#include "./core/AbstractEffectUnit.h"

#include "./effects/EndUnit.h"

#include "UnitFactory.h"

class ProcessGraph{
    
    public :

        struct Connection{

            Connection( id1_t si, id1_t sp, id1_t ti, id1_t tp ):
            m_si(si),m_sp(sp),m_ti(ti),m_tp(tp){}
            
            id1_t m_si, m_sp, m_ti, m_tp;
        };
        
        typedef std::map<id1_t, AbstractEffectUnit*> EffectList;
        typedef ReminderTree<id1_t, 4> ConnectionTree;

        static int create();
        static int kill();

        /**
         * Add or remove given effect from the process graph
         **/
        static int addEffect( id1_t type, id1_t id );
        static int addEffect( AbstractEffectUnit* unit, bool override=false );
        static int removeEffect( id1_t id );
        static AbstractEffectUnit* getEffect( id1_t id );
        static std::vector<id1_t> getEffectList();

        /**
         * Add or remove connection from the process graph
         **/
        static int addConnection( Connection c );
        static int removeConnection( Connection c );
        static std::vector<Connection> getConnection();

        /**
         * Change current bank
         **/

        static void clearGraph();

        /**
         * Print Graph Content
         **/
        static void printGraph();

        /**
         * Get Capture and Playback Units
         **/
        static EndUnit* getCapture();
        static EndUnit* getPlayback(); 

    public :

        static const id1_t ErrIdNotUnique   = 0x01;
        static const id1_t ErrInvalidEffect = 0x02;
        static const id1_t ErrIdNotFound    = 0x04;
        static const id1_t ErrInvalidId     = 0x08;

        static const int ErrorValCo = -5;
        static const int ValidValCo = -1;

    private :

        /* ******* Connections Managment ******* */

        static ConnectionTree m_connectionList;

        /* ********** Effect Managment ********* */

        static EffectList m_graph;
};

#endif
