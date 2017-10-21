/**********************************************************************
 * @file Drive.h
 * @author TomatoPi
 * @version 1.0
 *
 * Basic Distortion Effect.
 * Clip given Signal using hard clipping or dual-tanh Soft Clipping
 * Signal is passed througth a 3Bands EQ before and after clipping
 * Different Settings can be set for positives and negatives half-wave
 **********************************************************************/
#ifndef DEF_EFFECT_DRIVE_H
#define DEF_EFFECT_DRIVE_H

#include <string>

#include "../core/AbstractEffectUnit.h"

class DriveEffect : public AbstractEffectUnit{

    public :

        DriveEffect(uint8_t type, uint8_t id);

        static int process(jack_nframes_t nframes, void* arg);

    private :

        class DriveEffectArg{

            public :
                DriveEffectArg(float* params);
                float* m_params;
        };
    
        virtual void update();
};

#endif
