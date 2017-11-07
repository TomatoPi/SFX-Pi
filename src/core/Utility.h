/**********************************************************************
 * @file Utility.h
 * @author TomatoPi
 * @version 1.0
 *
 * File containing general purpose math stuff
 **********************************************************************/
#ifndef DEF_MATH_UTILS_H
#define DEF_MATH_UTILS_H

#include <math.h>
#include <jack/jack.h>

namespace utils{

    typedef jack_default_audio_sample_t sample_t;

    /**
     * Modulo operation that alway return positive value
     * ex 8%5 = -2%5 = 3
     **/
    static inline long absmod(long a, long b){

        long r = a%b;
        return (r<0)?b+r:r;
    }

    /**
     * Float absolute value
     **/
    static inline float abs(float a){

        return (a<0)?-a:a;
    }

    /**
     * Convert a value in db to gain ( multiplication factor )
     **/
    static inline float dbtorms(float a){

        return pow(10, a/20.0f);
    }

    /**
     * Convert a gain factory to gain in db
     **/
    static inline float rmstodb(float a){

        return 20.0f * log10(a);
    }

    /**
     * Convert a length in ms to length in sample
     **/
    static inline float mstos(float ms, float sr){

        return (ms*sr)/1000.0f;
    }

    /**
     * Convert a length in sample to length in ms
     **/
    static inline float stoms(float s, float sr){

        return (1000.0f*s)/sr;
    }

    /**
     * Drywet between two signals
     **/
    static inline sample_t drywet(sample_t dry, sample_t wet, float a){

        return (1-a)*dry + a*wet;
    }

    /**
     * Return random float between 0 and 1
     **/
    static inline float frand(){

        return (float)rand()/(float)RAND_MAX;
    }

    /**
     * Return random float between min and max
     **/
    static inline float frand(float min, float max){

        if ( max < min ){

            float a = min;
            min = max;
            max = a;
        }
        return frand()*(max-min) + min;
    }

    /**
     * Force given float to be between min and max
     **/
    static inline float constrain(float in, float min, float max){

        return (in < min)?min:(in > max)?max:in;
    }

    /**
     * Remap given float from [fmin,fmax] to [tmin,tmax]
     **/
    static inline float map(float in, float fmin, float fmax, float tmin, float tmax){

        return ((in-fmin)/(fmax-fmin))*(tmax-tmin) + tmin;
    }
}

#endif
