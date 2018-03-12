/**********************************************************************
 * @file Delay.h
 * @author TomatoPi
 * @version 1.1
 *
 * Simple Delay effect
 * Take normal signal and delay it, delayed signal is then mixed with
 * normal signal
 * This delay doesn't allow to insert effects inside the Feedback loop
 **********************************************************************/
#ifndef DEF_BASIC_DELAY_EFFECT_H
#define DEF_BASIC_DELAY_EFFECT_H

#include <string>

#include <iostream>
#include <string>

#include "plugin/JackPlugin.h"
#include "effect/AbstractEffectUnit.h"
#include "core/Event.h"

#include "lib/buffer/Buffer.h"
#include "core/Utils.h"

class DelayEffect: public AbstractEffectUnit
{

    public :

        /**
         * Constructor and Process Callback
         **/
        DelayEffect(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin);
        ~DelayEffect();

        static int process(jack_nframes_t nframes, void* arg);

        /**
         * Function Called When a parameter is modified or bank changed
         * It will update the Param Array passed to callback function
         * Pure Virtual function, must be redefined by every effect type
         **/
        virtual void update(SFXPEvent& event);

    private :

        /**
         * Function called when all effect parameters has changed
         **/
        virtual void updateAll();

        Buffer_S *_buffer;
        float _samplerate;
};

#endif
