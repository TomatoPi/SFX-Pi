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
 * File:   Chorus.h
 * Author: Space-Computer
 *
 * Created on 11 août 2018, 02:20
 */

#ifndef CHORUS_H
#define CHORUS_H

#include <math.h>
#include <exception>
#include <memory>

#include <jack/jack.h>

#include "noyau/log.h"
#include "noyau/types.h"
#include "process/module/jackWrapper.h"

#include "lib/buffer/Buffer.h"

struct ModuleChorus
{
    ModuleChorus();
    ~ModuleChorus();
    
////////////////////////////////////////////////////////////////////
// Attributs Jack
////////////////////////////////////////////////////////////////////
            
    jack_client_t   *client;
    jack_port_t     *audio_in, *mod_in, *audio_out;
    
////////////////////////////////////////////////////////////////////
// Paramètres de la distortion
////////////////////////////////////////////////////////////////////
    
    float feedback, dry_wet, size, volume;
    
    std::unique_ptr<Buffer_M> buffer;
    
    struct Voice_Param
    {
        size_t delay;
        float weight;
        float depth;
        
    }voices_params[5];
    
////////////////////////////////////////////////////////////////////
// Fonctions membres
////////////////////////////////////////////////////////////////////
    
    /**
     * @brief Callback Called by the jack process graph
     */
    static int jackCallback(jack_nframes_t nframes, void* arg);
    
    void setBank1(void);
    void setBank2(void);
    void setBank3(void);
    void setBank4(void);
};

#endif /* CHORUS_H */
