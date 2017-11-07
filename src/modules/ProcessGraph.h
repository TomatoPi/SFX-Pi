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

#include "../core/Singleton.h"
#include "../core/ReminderTree.h"

#include "./core/AbstractEffectUnit.h"

#include "./effects/EndUnit.h"

#include "UnitFactory.h"

class ProcessGraph : public Singleton<ProcessGraph>{

    friend class Singleton<ProcessGraph>;
    
    public :

        struct Connection{

            Connection( uint8_t si, uint8_t sp, uint8_t ti, uint8_t tp ):
            m_si(si),m_sp(sp),m_ti(ti),m_tp(tp){}
            
            uint8_t m_si, m_sp, m_ti, m_tp;
        };

        /**
         * Add or remove given effect from the process graph
         **/
        int addEffect( uint8_t type, uint8_t id );
        int addEffect( AbstractEffectUnit* unit, bool override=false );
        int removeEffect( uint8_t id );
        AbstractEffectUnit* getEffect( uint8_t id );
        std::vector<uint8_t> getEffectList() const;

        /**
         * Add or remove connection from the process graph
         **/
        int addConnection( Connection c );
        int removeConnection( Connection c );
        std::vector<Connection> getConnection();

        /**
         * Change current bank
         **/

        void clearGraph();
        
        void printGraph();

    public :

        static const uint8_t ErrIdNotUnique   = 0x01;
        static const uint8_t ErrInvalidEffect = 0x02;
        static const uint8_t ErrIdNotFound    = 0x04;
        static const uint8_t ErrInvalidId     = 0x08;

    private :

        static const int ErrorValCo = -5;
        static const int ValidValCo = -1;

        /**
         * Private Constructor and Destructor for Singleton pattern
         **/
        ProcessGraph();
        ~ProcessGraph();

        /* ******* Connections Managment ******* */
        typedef ReminderTree<int, 4> ConnectionTree;

        ConnectionTree m_connectionList;

        /* ********** Effect Managment ********* */
        typedef std::map<uint8_t, AbstractEffectUnit*> EffectList;

        EffectList m_graph;
};

#endif
