/**********************************************************************
 * @file FFTFilter.h
 * @author TomatoPi
 * @version 1.0
 *
 * Effect Unit that perform filtering on signal in frequency domain
 * Parameters are weigths of each bins
 * 
 * FFT library used is FFTReal by Laurent de Soras
 **********************************************************************/
#ifndef DEF_FFT_FILTER_EFFECT_H
#define DEF_FFT_FILTER_EFFECT_H

#include <string>

#include "../core/AbstractEffectUnit.h"
#include "../ffft/FFTRealFixLen.h"

#include "../RegistrableUnit.h"

#include "../../Const.h"

class FFTFilter : public AbstractEffectUnit,
    public RegistrableUnit<FFTFilter>
{

    public :

        enum FilterType{
            LOWPASS =0,
            HIGHPASS=1,
            BANDPASS=2
        };

        /**
         * Constructor and Process Callback
         **/
        FFTFilter(uint8_t id, uint8_t type);
        ~FFTFilter();

        static int process(jack_nframes_t nframes, void* arg);

    private :
    
        virtual void update();
};
typedef RegistrableUnit<FFTFilter> FFTFilterReg;
#endif
