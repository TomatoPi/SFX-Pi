/**********************************************************************
 * @file Chorus.h
 * @author TomatoPi
 * @version 1.0
 *
 * Simple Chorus effect
 * Takes a signal and add it delayed versions
 **********************************************************************/
#ifndef DEF_BASIC_CHORUS_EFFECT_H
#define DEF_BASIC_CHORUS_EFFECT_H

#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../core/buffer/Buffer.h"

#include "../../RegistrableUnit.h"

#include "DelayBase.h"

class ChorusEffect: public AbstractEffectUnit, public AbstractDelayBase,
    public RegistrableUnit<ChorusEffect>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        ChorusEffect(id1_t id, id1_t type);
        ~ChorusEffect();

        static int process(jack_nframes_t nframes, void* arg);

        /**
         * Max allowed delay length is 200ms
         **/
        static const usize_t BUFFER_SIZE = 48*200;

        /**
         * Max number of delay lines is 5
         **/
        static const usize_t MAX_COUNT = 5;

        /**
         * Max delay of a single line is Max delay / 2
         **/
        static const usize_t MAX_DELAY = BUFFER_SIZE / 2;

    private :
    
        virtual void update();

    private :

        Buffer<BUFFER_SIZE> *m_buffer;
};
typedef RegistrableUnit<ChorusEffect> ChorusReg;

#endif
