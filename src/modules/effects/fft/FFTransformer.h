/**********************************************************************
 * @file FFTransformer.h
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that transform time domain signal to frequency domain
 * This effect contains two JACKUnits , the first one is the real to
 * complex tranformer, the second one is the complex to real transformer
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#ifndef DEF_FFTRANSFORMER_EFFECT_H
#define DEF_FFTRANSFORMER_EFFECT_H


#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../ffft/FFTRealFixLen.h"

#include "../../RegistrableUnit.h"

#include "../../../Const.h"

class FFTComplexifier : public AbstractEffectUnit,
    public RegistrableUnit<FFTComplexifier>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        FFTComplexifier(uint8_t id, uint8_t type);
        ~FFTComplexifier();

        static int process(jack_nframes_t nframes, void* arg);

    private :
    
        virtual void update();

    private :
    
        ffft::FFTRealFixLen<SFXP::JACK_BLOCK_SIZE> *m_ffft;
};
typedef RegistrableUnit<FFTComplexifier> FFTCReg;

class FFTRealizer : public AbstractEffectUnit,
    public RegistrableUnit<FFTRealizer>
{

    public :

        /**
         * Constructor and Process Callback
         **/
        FFTRealizer(uint8_t id, uint8_t type);
        ~FFTRealizer();

        static int process(jack_nframes_t nframes, void* arg);

    private :
    
        virtual void update();
        
    private :
    
        ffft::FFTRealFixLen<SFXP::JACK_BLOCK_SIZE> *m_ffft;
};
typedef RegistrableUnit<FFTRealizer> FFTRReg;

#endif
