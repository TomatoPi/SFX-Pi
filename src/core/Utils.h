/**********************************************************************
 * @file Utils.h
 * @author TomatoPi
 * @version 2.0
 *
 * Math utils for the program
 **********************************************************************/
#ifndef DEF_UTILS_H
#define DEF_UTILS_H

#include <math.h>

namespace utils {

    enum Curve{

        CURVE_LIN=0,
        CURVE_SIG=1,
        CURVE_COS=2
    };
    typedef float (*tranfsert_f)(float);

    /**
     * Identity Tranfert Function, simply return given value
     **/
    static inline float curve_lin(float v) {
        return v;
    }
    /**
     * Hyperbolic Tan tranfert function
     **/
    static inline float curve_sig(float v) {
        return tanh(3*v);
    }
    /**
     * Hanning window tranfert function
     *
     **/
    static inline float curve_cos(float v) {
        return cos((v-1)*M_PI/2);
    }
    /**
     * Function for mapping a value from [fmin,fmax] to [tmin,tmax]
     **/
    static inline float map(float val, float fmin, float fmax, float tmin, float tmax){
        
        return  (val-fmin)*(tmax-tmin)/(fmax-fmin) + tmin;
    }
    /**
     * Function to get a tranfert curve from a Curve enum
     **/
    static inline tranfsert_f convertCurve(Curve c) {

        if      (c == CURVE_SIG)    return curve_sig;
        else if (c == CURVE_COS)    return curve_cos;
        else                        return curve_lin;
    }

    /**
     * Drywet between two signals
     **/
    static inline SFXP::sample_t drywet(SFXP::sample_t dry, SFXP::sample_t wet, float a){

        return (1-a)*dry + a*wet;
    }
    
    /**
    * Convert db to gain ( ex : +20dB ==> *100 )
    */
    static inline float dbtorms(float d){

        return pow(10, d/20.0f);
    }

    /**
    * Convert gain to db ( ex : *0.5 ==> -3dB )
    */
    static inline float rmstodb(float g){

        return 20.0f * log10(g);
    }


    /**
    * Convert a length in ms to sample
    */
    static inline int mstos(float ms, int sr){

        return (int)( (ms*sr) / 1000.0f );
    }

    /**
    * Convert a length in sample to ms
    */
    static inline int stoms(float sample, int sr){

        return (int)( (sample * 1000.0f) / (float)(sr) );
    }

    /**
    * Return random float between min and max
    */
    static inline float frand(float min, float max){

        if(max < min){

            float a = min;
            min = max;
            max = a;
        }

        return ((float)rand()/(float)RAND_MAX) * (max - min) + min;
    }

};

#endif
