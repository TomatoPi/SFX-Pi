/**********************************************************************
 * @file EffectHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * Previously called ProcessGraph, this file provide the main's program
 *  structure, that manage effects
 **********************************************************************/
#ifndef DEF_EFFECT_HANDLER_H
#define DEF_EFFECT_HANDLER_H

#include <jack/jack.h>

#include "handler/AbstractHandler.h"

#include "lib/preset/Preset.h"
#include "core/EffectFactory.h"

class EffectHandler : public AbstractHandler {

    public :

        EffectHandler();
        ~EffectHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent& event);

    private :

        void eventPresetChanged(SFXPEvent& event);

        void eventAddBank(SFXPEvent& event);
        void eventRemoveBank(SFXPEvent& event);
        void eventChangeBank(SFXPEvent& event);
        void eventEditParam(SFXPEvent& event);

        void eventConnect(SFXPEvent& event);
        void eventDisconnect(SFXPEvent& event);

        void eventAddEffect(SFXPEvent& event);
        void eventRemoveEffect(SFXPEvent& event);

        Preset* _curentPreset;

        /** Client representing All programm for JACK **/
        static jack_client_t* m_client;
};

#endif
