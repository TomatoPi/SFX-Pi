/**********************************************************************
 * @file EVHEvent.h
 * @author TomatoPi
 * @version 1.0
 *
 * Struct containing EventHandler event informations
 **********************************************************************/
#ifndef DEF_EVH_EVENT_H
#define DEF_EVH_EVENT_H

#include "types.h"

/**
 * @brief Eventhandler events are related to edit EventSequencers from
 *  the preset.
 **/
struct SFXPEvent;
struct EVHEvent {

    EVHEvent(SFXP::id2_t id, SFXP::id2_t tr, int s, int a, SFXP::id2_t ta):
        _id(id), _trigger(tr),
        _status(s), _action(a),
        _target(ta),
        _idx(0),
        _event(nullptr)
    {
    }
    EVHEvent()=default;

    SFXP::id2_t _id;        /**< ID of the sequencer related */
    SFXP::id2_t _trigger;   /**< ID of the trigger (An External Input) */

    int _status;            /**< Wanted Trigger's status */
    int _action;            /**< Action to perform */

    SFXP::id2_t _target;    /**< ID of the Sequence to edit */
    
    SFXP::usize_t _idx;     /**< Index of the event to edit */
    SFXPEvent* _event;      /**< Pointer to the event to link */
};

#endif
