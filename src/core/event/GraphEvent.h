/**********************************************************************
 * @file GraphEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct contaning Graph event informations
 **********************************************************************/
#ifndef DEF_GRAPH_EVENT_H
#define DEF_GRAPH_EVENT_H

#include "types.h"

/**
 *  Graph events are related to modification of the process graph
 *  - Add or Remove a Connection
 **/
struct GraphEvent {

    GraphEvent(SFXP::id1_t si, SFXP::usize_t sp, SFXP::id1_t ti, SFXP::usize_t tp, bool m) :
        _source(si), _sport(sp), _target(ti), _tport(tp), _midi(m) {
        }
    GraphEvent()=default;

    SFXP::id1_t _source;    /**< Id of the source  for the connection */
    SFXP::usize_t _sport;   /**< Index of source port for the connection */
    
    SFXP::id1_t _target;    /**< ID of the target for the connection */
    SFXP::usize_t _tport;   /**< Index of target port for the connection */

    bool _midi; /**< True if this linnk is a midi link */
};

#endif
