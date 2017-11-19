/**********************************************************************
 * @file Reverb.h
 * @author TomatoPi
 * @version 1.0
 *
 * First attempt of reverb effect
 * Based on multiple small delay lengths summ, then summ is filtered
 * to remove clicks caused by audible delay between each lengths
 **********************************************************************/
#ifndef DEF_BASIC_REVERB_EFFECT_H
#define DEF_BASIC_REVERB_EFFECT_H

#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../core/buffer/Buffer.h"
#include "../../core/filter/MonoPoleFilter.h"

#include "../../RegistrableUnit.h"

#include "../../../core/Utility.h"

#include "DelayBase.h"


class ReverbEffect: public AbstractEffectUnit, public AbstractDelayBase,
    public RegistrableUnit<ReverbEffect>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        ReverbEffect(id1_t id, id1_t type);
        ~ReverbEffect();

        static int process(jack_nframes_t nframes, void* arg);

        /**
         * Delays lines are between 10 and 100ms
         **/
        static const size_t MIN_TIME = 48*10;
        static const size_t MAX_TIME = 48*300;
        static const size_t LINE_COUNT = 10;

    private :
    
        virtual void update();

    private :

        Buffer<MAX_TIME> *m_buffer;
};
typedef RegistrableUnit<ReverbEffect> ReverbReg;

#endif
