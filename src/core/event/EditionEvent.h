/**********************************************************************
 * @file EditionEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the base struct for the internal comunication protocol
 **********************************************************************/
#ifndef DEF_EDITION_EVENT_H
#define DEF_EDITION_EVENT_H

#include "types.h"

/**
 * Struct contaning Edition event informations
 *  Edition event are related to add or remove an effect from the graph
 **/
struct EditionEvent {

    EditionEvent(SFXP::id1_t id, SFXP::tc_t t):
        _id(id), _type(t){
    }
    EditionEvent()=default;

    SFXP::id1_t _id;    /**< ID of effect to add */
    SFXP::tc_t _type;   /**< Type of effect to add */
};

#endif
