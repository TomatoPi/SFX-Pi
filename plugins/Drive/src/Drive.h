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

#include <iostream>
#include <string>

#include "plugin/JackPlugin.h"
#include "effect/AbstractEffectUnit.h"
#include "core/Event.h"

#include "lib/filter/GraphicEQ.h"
#include "lib/driveBase/DriveBase.h"

#include "types.h"

class DriveEffect : public AbstractEffectUnit, public AbstractDriveBase
{

    public :

        /**
         * Constructor and Process Callback
         **/
        DriveEffect(SFXP::tc_t typeCode, SFXP::id1_t id, JackPlugin* plugin);
        ~DriveEffect();

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

        GraphicEQ *m_toneIn;
        GraphicEQ *m_toneOut;

        clip_f m_clipP;
        clip_f m_clipN;
};
#endif
