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
 * File:   Polysynth.h
 * Author: Space-Computer
 *
 * Created on 6 août 2018, 21:00
 */

#ifndef POLYSYNTH_H
#define POLYSYNTH_H

#include <math.h>
#include <list>
#include <map>
#include <exception>

#include <jack/jack.h>

#include "noyau/log.h"
#include "noyau/types.h"
#include "noyau/midi.h"
#include "noyau/utils.h"
#include "process/module/jackWrapper.h"

#include "lib/envelope/ADSR.h"

struct ModulePolysynth
{
    ModulePolysynth();
    ~ModulePolysynth();

    ////////////////////////////////////////////////////////////////////
    // Attributs Jack
    ////////////////////////////////////////////////////////////////////

    jack_client_t *client;
    jack_port_t *midi_in, *audio_out;

    ////////////////////////////////////////////////////////////////////
    // Paramètres du synthé
    ////////////////////////////////////////////////////////////////////

    enum WaveForm
    {
        WSIN = 0, /**< Sinwave */
        WSQR = 1, /**< Square */
        WTRI = 2, /**< Triangular */
        WSAW = 3, /**< Sawtooth */
        WVAR = 4, /**< Varislope */
        WNPH = 5, /**< N-Phase */
        WWHI = 6, /**< White noise */
        WCST = 7, /**< Constant */
        WHAR = 8 /**< Harmonics */
    } waveform;

    float phase, sign, volume;
    float param1, param2, phase_distortion, phase_fix;
    
    struct Note
    {
        float ramp, rbs;
        float vol;
        ADSR envelope;
    };
    
    std::map<sfx::hex_t, Note> notes_map;

    ////////////////////////////////////////////////////////////////////
    // Fonctions membres
    ////////////////////////////////////////////////////////////////////

    /**
     * @brief Callback Called by the jack process graph
     */
    static int jackCallback(jack_nframes_t nframes, void* arg);

    ////////////////////////////////////////////////////////////////////
    // Fonctions d'ondes
    ////////////////////////////////////////////////////////////////////

    typedef sfx::sample_t(*transfert_f)(sfx::sample_t, float, float, float);

    transfert_f tfunc;

    /**
     * Sinwave
     **/
    static inline sfx::sample_t w_sin(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)(s * sin(M_PI * 2 * in));
    }

    /**
     * Square wave
     **/
    static inline sfx::sample_t w_sqr(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)((in - 0.5 < 0) ? s * (-1) : s);
    }

    /**
     * Triangular wave
     **/
    static inline sfx::sample_t w_tri(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)(s * (2.0 * sfx::abs(2.0 * in - 1.0) - 1.0));
    }

    /**
     * Sawtooth
     **/
    static inline sfx::sample_t w_saw(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)(s * (2.0 * in - 1.0));
    }

    /**
     * Varislope
     **/
    static inline sfx::sample_t w_var(sfx::sample_t in, float s, float p1, float p2)
    {
        if (in < p1)
        {
            return (sfx::sample_t)(s * (2.0 * ((p2 * in) / p1) - 1.0));
        }
        else
        {
            return (sfx::sample_t)(s * (1.0 - 2.0 * p2 * (in - p1) / (1 - p1)));
        }
    }

    /**
     * N-Phase
     **/
    static inline sfx::sample_t w_nph(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)(s * sin(M_PI * 2.0 * (in + (((float) ((int) (in * (p1 + 1)))) / p2))));
    }

    /**
     * White Noise
     **/
    static inline sfx::sample_t w_whi(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)(sfx::frand(-1.0, 1.0));
    }

    /**
     * Constant
     **/
    static inline sfx::sample_t w_cst(sfx::sample_t in, float s, float p1, float p2)
    {
        return (sfx::sample_t)p1;
    }

    /**
     * Sinus Harmonics
     **/
    static inline sfx::sample_t w_har(sfx::sample_t in, float s, float N, float alpha)
    {
        sfx::sample_t out = 0;
        for(size_t k = 0; k < N; ++k)
        {
            out += sin(M_PI * 2.0 * in * powf(alpha, k));
        }
        return out / N;
    }

    static inline transfert_f castWaveform(WaveForm wave)
    {
        if (wave == WSIN) return w_sin;
        if (wave == WSQR) return w_sqr;
        if (wave == WTRI) return w_tri;
        if (wave == WSAW) return w_saw;
        if (wave == WVAR) return w_var;
        if (wave == WNPH) return w_nph;
        if (wave == WWHI) return w_whi;
        if (wave == WCST) return w_cst;
        if (wave == WHAR) return w_har;

        return w_sin;
    }
};

#endif /* POLYSYNTH_H */
