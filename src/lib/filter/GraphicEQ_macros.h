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
 * File:   GraphicEQ_macros.h
 * Author: Space-Computer
 *
 * Created on 5 septembre 2018, 17:43
 */

#ifndef GRAPHICEQ_MACROS_H
#define GRAPHICEQ_MACROS_H

struct GEQ_3Bands_Params
{
    float low;
    float high;

    float gains[3];
};

#define STRING(A) #A 

#define REGISTER_3BAND_GRAPHIC_EQ_SLOTS(table, grahic_eq_ptr, graphic_eq_params, prefix_internal, prefix_display) \
{\
    Module::register_slot((table), STRING(prefix_internal ## _Lowcut), STRING(prefix_display ##  Lowcut) \
        , [](sfx::hex_t val, Module* mod)->float \
        { \
            if (val < 128) \
            { \
                float res = sfx::mapfm_freq(val); \
                (graphic_eq_params).low = res; \
                (grahic_eq_ptr)->setFrequency(0, res, jack_get_sample_rate(mod->client)); \
            } \
            return (graphic_eq_params).low; \
        }); \
    Module::register_slot((table), STRING(prefix_internal ## _Highcut), STRING(prefix_display ##  Highcut) \
        , [](sfx::hex_t val, Module* mod)->float \
        { \
            if (val < 128) \
            { \
                float res = sfx::mapfm_freq(val); \
                (graphic_eq_params).high = res; \
                (grahic_eq_ptr)->setFrequency(0, res, jack_get_sample_rate(mod->client)); \
            } \
            return (graphic_eq_params).high; \
        }); \
        \
    Module::register_slot((table), STRING(prefix_internal ## _Lowgain), STRING(prefix_display ##  Lowgain) \
        , [](sfx::hex_t val, Module* mod)->float \
        {\
            if (val < 128)\
                (graphic_eq_params).gains[0] = sfx::mapfm_db(val, -30, 30);\
            return (graphic_eq_params).gains[0];\
        });\
    Module::register_slot((table), STRING(prefix_internal ## _Midgain), STRING(prefix_display ##  Midgain) \
        , [](sfx::hex_t val, Module* mod)->float \
        {\
            if (val < 128)\
                (graphic_eq_params).gains[1] = sfx::mapfm_db(val, -30, 30);\
            return (graphic_eq_params).gains[1];\
        });\
    Module::register_slot((table), STRING(prefix_internal ## _Highgain), STRING(prefix_display ##  Highgain) \
        , [](sfx::hex_t val, Module* mod)->float \
        {\
            if (val < 128)\
                (graphic_eq_params).gains[2] = sfx::mapfm_db(val, -30, 30);\
            return (graphic_eq_params).gains[2];\
        }); \
}

#endif /* GRAPHICEQ_MACROS_H */

