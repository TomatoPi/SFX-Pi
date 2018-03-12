/**********************************************************************
 * @file LFO.h
 * @author TomatoPi
 * @version 1.0
 *
 * Low Frequency Oscillator effect
 * Effect used to generate modulation signals for effects like chorus
 * or ring modulator ( tremolo )
 * LFO has an input port name sync :
 * a non zero value on first sample of this port will cause lfo to reset
 * it ramp to it phase value
 * Useful to sync different lfo together
 **********************************************************************/
#ifndef DEF_LFO_EFFECT_H
#define DEF_LFO_EFFECT_H

#include <string>
#include <math.h>

#include "../../core/AbstractEffectUnit.h"

#include "../../RegistrableUnit.h"

#include "../../../core/Utility.h"

#include "../../midi/MIDIBase.h"

class LFOEffect : public AbstractEffectUnit,
    public RegistrableUnit<LFOEffect>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        LFOEffect(id1_t id, id1_t type);
        ~LFOEffect();

        static int process(jack_nframes_t nframes, void* arg);

        enum WAVEFORM{
            WSIN=0, /**< Sinwave */
            WSQR=1, /**< Square */
            WTRI=2, /**< Triangular */
            WSAW=3, /**< Sawtooth */
            WVAR=4, /**< Varislope */
            WNPH=5, /**< N-Phase */
            WWHI=6, /**< White noise */
            WCST=7  /**< Constant */
        };

    private :
    
        virtual void update();

        typedef sample_t (*waveform)(sample_t,float,float,float);

        static waveform castWaveform(WAVEFORM wave);

        waveform m_wave;

    private :

        /**
         * Sinwave
         **/
        static inline sample_t w_sin(sample_t in, float s, float p1, float p2){

            return (sample_t)(s*sin(M_PI * 2 * in));
        }
        /**
         * Square wave
         **/
        static inline sample_t w_sqr(sample_t in, float s, float p1, float p2){

            return (sample_t)((in-0.5 < 0)? s*(-1) : s);
        }
        /**
         * Triangular wave
         **/
        static inline sample_t w_tri(sample_t in, float s, float p1, float p2){

            return (sample_t)(s * (2.0 * utils::abs(2.0*in -1.0) -1.0));
        }
        /**
         * Sawtooth
         **/
        static inline sample_t w_saw(sample_t in, float s, float p1, float p2){

            return (sample_t)(s * (2.0*in - 1.0));
        }
        /**
         * Varislope
         **/
        static inline sample_t w_var(sample_t in, float s, float p1, float p2){

            if ( in < p1 ){

                return (sample_t)(s * (2.0*((p2*in)/p1) -1.0));
            }
            else{

                return (sample_t)(s * (1.0 - 2.0*p2*(in-p1)/(1-p1)));
            }
        }
        /**
         * N-Phase
         **/
        static inline sample_t w_nph(sample_t in, float s, float p1, float p2){

            return (sample_t)(s * sin(M_PI * 2.0 * (in + (((float)((int)(in*(p1+1))))/p2) )));
        }
        /**
         * White Noise
         **/
        static inline sample_t w_whi(sample_t in, float s, float p1, float p2){

            return (sample_t)(utils::frand(-1.0, 1.0));
        }
        /**
         * Constant
         **/
        static inline sample_t w_cst(sample_t in, float s, float p1, float p2){

            return (sample_t)p1;
        }
};
typedef RegistrableUnit<LFOEffect> LFOReg;

#endif
