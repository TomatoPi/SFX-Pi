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
#include "../core/filter/GraphicEQ.h"

class DriveEffect : public AbstractEffectUnit{

    public :

        /**
         * Constructor and Process Callback
         **/
        DriveEffect(uint8_t id, uint8_t type);
        ~DriveEffect();

        static int process(jack_nframes_t nframes, void* arg);

        typedef enum{
            SOFT,
            HARD
        }TYPE;

        /**
         * Consts for UnitFactory Register
         **/
        static const std::string NAME;
        
        static AbstractEffectUnit* BUILDER(uint8_t id, uint8_t type);

        static const std::string PARNAMES[];
        static const uint8_t PARCOUNT;

        static const std::string PORNAMES[];
        static const uint8_t AI;
        static const uint8_t AO;
        static const uint8_t MI;
        static const uint8_t MO;

    private :

        static const uint8_t PARSIZE;
    
        virtual void update();

    private :

        typedef sample_t (*clip_f)( sample_t, float, float );
        static clip_f castClipper( TYPE );

        GraphicEQ *m_toneIn;
        GraphicEQ *m_toneOut;

        float m_samplerate;

        clip_f m_clipP;
        clip_f m_clipN;
};

#endif
