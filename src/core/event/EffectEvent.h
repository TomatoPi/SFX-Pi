/**********************************************************************
 * @file EffectEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct contaning Effect event informations
 **********************************************************************/
#ifndef DEF_EFFECT_EVENT_H
#define DEF_EFFECT_EVENT_H

#include "types.h"

/**
 * Struct contaning Effect event informations
 *  Effects events are rised to edit active effects :
 *  - Change / Add / Remove a Bank
 *  - Edit a parameter
 **/
struct EffectEvent {

    EffectEvent(SFXP::id1_t id, SFXP::usize_t idx, float v):
        _id(id), _idx(idx), _value(v) {
        }
    EffectEvent()=default;

    SFXP::id1_t _id;    /**< ID of the ParamBank targeted by the change */
    SFXP::usize_t _idx; /**< Index of the param to change */
    float _value;       /**< The new value for the param */
};

#endif
