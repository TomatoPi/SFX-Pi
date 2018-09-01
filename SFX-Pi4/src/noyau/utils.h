/*
 * Copyright (C) 2018 Space-Computer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   utils.h
 * Author: Space-Computer
 *
 * Created on 6 août 2018, 21:32
 */

#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include "types.h"

namespace sfx
{

    /**
     * Float absolute value
     **/
    static inline float abs(float a)
    {
        return (a < 0) ? -a : a;
    }

    /**
     * Return random float between 0 and 1
     **/
    static inline float frand()
    {
        return (float) rand() / (float) RAND_MAX;
    }
    
    /**
    * Convert db to gain ( ex : +20dB ==> *100 )
    */
    inline float dbtorms(float d){

        return pow(10, d/10.0f);
    }

    /**
    * Convert gain to db ( ex : *0.5 ==> -3dB )
    */
    inline float rmstodb(float g){

        return 10.0f * log10(g);
    }

    /**
     * Remap given float from [fmin,fmax] to [tmin,tmax]
     **/
    static inline float map(float in, float fmin, float fmax, float tmin, float tmax)
    {
        return ((in - fmin) / (fmax - fmin))*(tmax - tmin) + tmin;
    }
    
    /**
     * Remap given midi_cc value to [tmin;tmax] interval, using linear transformation
     */
    static inline float mapfm_lin(sfx::hex_t in, float tmin, float tmax)
    {
        return map(in, 0, 127, tmin, tmax);
    }
    
    /**
     * Remap given midi_cc value to [tmin;tmax] dB interval, return gain factor
     */
    static inline float mapfm_db(sfx::hex_t in, float tmin, float tmax)
    {
        return dbtorms(map(in, 0, 127, tmin, tmax));
    }
    
    /**
     * Remap given midi_cc value to [0;20k] Hz interval (approximately)
     */
    static inline float mapfm_freq(sfx::hex_t in)
    {
        return (440.0 / 32.0) * pow(2, ((float)in-9.0)/12.0);
    }

    /**
     * Return random float between min and max
     **/
    static inline float frand(float min, float max)
    {
        if (max < min)
        {
            float a = min;
            min = max;
            max = a;
        }
        return frand()*(max - min) + min;
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
     * Drywet between two signals
     **/
    static inline sfx::sample_t drywet(sfx::sample_t dry, sfx::sample_t wet, float a){

        return (1-a)*dry + a*wet;
    }
}

#endif /* UTILS_H */

