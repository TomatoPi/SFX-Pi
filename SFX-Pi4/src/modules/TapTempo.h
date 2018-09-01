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
 * File:   TapTempo.h
 * Author: Space-Computer
 *
 * Created on 23 juillet 2018, 15:03
 */

#ifndef TAPTEMPO_H
#define TAPTEMPO_H

#include <deque>
#include <exception>

#include <jack/jack.h>

#include "noyau/log.h"
#include "noyau/midi.h"
#include "process/module/jackWrapper.h"

struct ModuleTapTempo
{
    ModuleTapTempo();
    ~ModuleTapTempo();
    
////////////////////////////////////////////////////////////////////
// Attributs Jack
////////////////////////////////////////////////////////////////////
            
    jack_client_t   *client;
    jack_port_t     *midi_in, *midi_out;
    
////////////////////////////////////////////////////////////////////
// Attributs Gestion du Tempo
////////////////////////////////////////////////////////////////////
    
    jack_time_t clock;
    jack_time_t delta_tempo, delta_on;
    
    sfx::hex_t source;
    int tempo;
    
    int signature_num, signature_den;
    int signature_count;
    
    std::deque<jack_time_t> last_time;
    
    enum {
        ON,
        OFF
    }status;
    
////////////////////////////////////////////////////////////////////
// Fonctions membres
////////////////////////////////////////////////////////////////////
    
    /**
     * @brief Callback Called by the jack process graph
     */
    static int jackCallback(jack_nframes_t nframes, void* arg);
    
    void setSignature(int num, int den);
};

#endif /* TAPTEMPO_H */
