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
 * File:   jackWrapper.h
 * Author: Space-Computer
 *
 * Created on 25 août 2018, 22:09
 */

#ifndef JACKWRAPPER_H
#define JACKWRAPPER_H

#include <exception>

#include <jack/jack.h>

#include "noyau/log.h"

namespace sfx
{

    /**
     * @brief Wrapping function to alloc a Jack_Client object
     * @param name name of the client to create
     * @return pointer to the client created
     * @throw runtime_error if creation failed
     * 
     * @post returned value points to a valid client
     */
    static inline jack_client_t* openJackClient(std::string name)
    {
        jack_options_t options = JackNullOption;
        jack_status_t status;

        jack_client_t* client = jack_client_open(name.c_str(), options, &status, "");
        if (client == NULL)
        {
            std::string err = sfx::formatString(
                    "jack_client_open() failed, status = 0x%2.0x", status);

            if (status & JackServerFailed)
                err += "\nUnable to connect to JACK server";

            throw std::runtime_error(err);
        }

        if (status & JackServerStarted)
        {
            sfx::log(name, "JACK server started\n");
        }
        if (status & JackNameNotUnique)
        {
            sfx::log(name, "unique name `%s' assigned\n", jack_get_client_name(client));
        }
        
        return client;
    }
    
    /**
     * @biref Wrapping function to allocate an Audio Input Port
     * @return pointer to the port registered
     * @throw runtime_error if registration failed
     * @post returned value points to a valid port
     */
    static inline jack_port_t* registerAudioInput(jack_client_t* client, std::string name)
    {
        jack_port_t* port = jack_port_register(client, name.c_str(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput, 0);
        
        if (port == NULL) 
            throw std::runtime_error(sfx::formatString("Failed Register Jack Port : %s", name));
        return port;
    }
    /**
     * @biref Wrapping function to allocate an Audio Output Port
     * @return pointer to the port registered
     * @throw runtime_error if registration failed
     * @post returned value points to a valid port
     */
    static inline jack_port_t* registerAudioOutput(jack_client_t* client, std::string name)
    {
        jack_port_t* port = jack_port_register(client, name.c_str(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput, 0);
        
        if (port == NULL) 
            throw std::runtime_error(sfx::formatString("Failed Register Jack Port : %s", name));
        return port;
    }
    
    /**
     * @biref Wrapping function to allocate a Midi Input Port
     * @return pointer to the port registered
     * @throw runtime_error if registration failed
     * @post returned value points to a valid port
     */
    static inline jack_port_t* registerMidiInput(jack_client_t* client, std::string name)
    {
        jack_port_t* port = jack_port_register(client, name.c_str(),
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsInput, 0);
        
        if (port == NULL) 
            throw std::runtime_error(sfx::formatString("Failed Register Jack Port : %s", name));
        return port;
    }
    /**
     * @biref Wrapping function to allocate a Midi Output Port
     * @return pointer to the port registered
     * @throw runtime_error if registration failed
     * @post returned value points to a valid port
     */
    static inline jack_port_t* registerMidiOutput(jack_client_t* client, std::string name)
    {
        jack_port_t* port = jack_port_register(client, name.c_str(),
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsOutput, 0);
        
        if (port == NULL) 
            throw std::runtime_error(sfx::formatString("Failed Register Jack Port : %s", name));
        return port;
    }
    
    /**
     * @brief Wrapping function to activate a Jack Client
     * @throw runtime_error if activation failed
     */
    static inline void activateClient(jack_client_t* client)
    {
        if (jack_activate(client))
            throw std::runtime_error("cannot activate client");
    }
}

#endif /* JACKWRAPPER_H */

