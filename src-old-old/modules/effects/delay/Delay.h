/**********************************************************************
 * @file Delay.h
 * @author TomatoPi
 * @version 1.0
 *
 * Simple Delay effect
 * Take normal signal and delay it, delayed signal is then mixed with
 * normal signal
 **********************************************************************/
#ifndef DEF_BASIC_DELAY_EFFECT_H
#define DEF_BASIC_DELAY_EFFECT_H

#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../core/buffer/Buffer.h"

#include "../../RegistrableUnit.h"

#include "DelayBase.h"

class DelayEffect: public AbstractEffectUnit, public AbstractDelayBase,
    public RegistrableUnit<DelayEffect>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        DelayEffect(id1_t id, id1_t type);
        ~DelayEffect();

        static int process(jack_nframes_t nframes, void* arg);

        /**
         * Max allowed delay length is 5000ms
         **/
        static const usize_t MAX_DELAY = 48*5000;

    private :
    
        virtual void update();

    private :

        Buffer<MAX_DELAY> *m_buffer;
};
typedef RegistrableUnit<DelayEffect> DelayReg;

#endif
