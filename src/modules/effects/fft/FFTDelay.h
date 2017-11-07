/**********************************************************************
 * @file FFTDelay.h
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that delay frequency domain signal with a different
 * delay for each bins
 * Resulting signal harmonics appear in a different order that original
 * one
 * At the moment individual delays are random, using two parameters
 * Delay and spread, determinating repartition around given delay
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#ifndef DEF_FFT_DELAY_EFFECT_H
#define DEF_FFT_DELAY_EFFECT_H

#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../core/buffer/BlockBuffer.h"
#include "../../ffft/FFTRealFixLen.h"

#include "../../RegistrableUnit.h"

#include "../../../Const.h"

#include "../../../core/Utility.h"

class FFTDelay : public AbstractEffectUnit,
    public RegistrableUnit<FFTDelay>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        FFTDelay(uint8_t id, uint8_t type);
        ~FFTDelay();

        static int process(jack_nframes_t nframes, void* arg);

        static const size_t BIN = 1 << SFXP::JACK_BLOCK_SIZE;
        static const size_t BIN_COUNT = 938;


    private :
    
        virtual void update();

        BlockBuffer<sample_t,BIN,BIN_COUNT> *m_buffer;
};
typedef RegistrableUnit<FFTDelay> FFTDelayReg;

#endif
