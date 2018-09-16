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
 * File:   types.h
 * Author: Space-Computer
 *
 * Created on 27 juin 2018, 01:31
 */

#ifndef TYPES_H
#define TYPES_H


#include <string>
#include <limits>

#include <jack/jack.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Program's main namespace
 **/
namespace sfx{

    using namespace std;

    /**
     * ID sur 1 octet, compris entre 0x00 et 0xff
     **/
    typedef uint8_t id1_t;
    const id1_t ErrId1 = numeric_limits<id1_t>::max();

    /**
     * ID sur 4 octets, compris entre 0x00000000 et 0xffffffff
     **/
    typedef uint16_t id2_t;
    const id2_t ErrId2 = numeric_limits<id2_t>::max();

    /**
     * TypeCode des effets, valeur comprise entre 0 et 65536
     **/
    typedef uint16_t tc_t;
    const tc_t ErrTypeCode = numeric_limits<tc_t>::max();

    /**
     * Valeur Hexadecimale sur 1 octet
     **/
    typedef unsigned char hex_t;
    
    /**
     * Paire de valeurs Hexadecimales
     */
    typedef std::pair<hex_t, hex_t> hex_pair_t;

    /**
     * Diminutif pour les samples de JACK
     **/
    typedef jack_default_audio_sample_t sample_t;

    /**
     * Flag utilisé pour baliser les fichiers binaires
     **/
    typedef uint8_t flag_t;

    /**
     * Uniformise le type size_t qui change entre Archlinux et Lubuntu
     **/
    typedef uint32_t usize_t;
    
    const string VERSION = "4.1.0";
    const string BUILD   = string(__DATE__) + "   " + string(__TIME__);
}

#ifdef __cplusplus
}
#endif

#endif /* TYPES_H */

