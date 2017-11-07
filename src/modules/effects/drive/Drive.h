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

#include "../../core/AbstractEffectUnit.h"
#include "../../core/filter/GraphicEQ.h"

#include "../../RegistrableUnit.h"

#include "DriveBase.h"

class DriveEffect : public AbstractEffectUnit, public AbstractDriveBase,
    public RegistrableUnit<DriveEffect>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        DriveEffect(uint8_t id, uint8_t type);
        ~DriveEffect();

        static int process(jack_nframes_t nframes, void* arg);

    private :
    
        virtual void update();

    private :

        GraphicEQ *m_toneIn;
        GraphicEQ *m_toneOut;

        clip_f m_clipP;
        clip_f m_clipN;
};

typedef RegistrableUnit<DriveEffect> DriveReg;

#endif
