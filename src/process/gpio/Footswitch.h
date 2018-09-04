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
 * File:   Footswitch.h
 * Author: Space-Computer
 *
 * Created on 23 juillet 2018, 15:33
 */

#ifndef FOOTSWITCH_H
#define FOOTSWITCH_H

#include <map>
#include <vector>
#include <memory>
#include <initializer_list>
#include <exception>

#include <array>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <queue>

#include "noyau/midi.h"
#include "process/module/jackWrapper.h"

#include "mcp23017.h"

#ifdef __ARCH_LINUX__
    #include <wiringPi.h>
    #include <mcp3004.h>
#endif

#define SFX_POT_COUNT 6

////////////////////////////////////////////////////////////////////
// Configuration du GPIO
////////////////////////////////////////////////////////////////////

typedef std::map<sfx::hex_t, sfx::hex_t> HexTable;
typedef std::map<sfx::hex_t,HexTable[2]> InputTable;
/**
 * @return Construit la table des adresses des differents footswitches 
 *  et leur Adresse MIDI correspondante
 */
InputTable buildFootTable();

typedef std::map<sfx::hex_t, mcp23017::Adress> OutputTable;
/**
 * @return Construit la table des adresses des differents leds en fonction
 *  de l'adresse MIDI du switch associé
 */
OutputTable buildLedTable();

typedef std::array<sfx::hex_t, SFX_POT_COUNT> AnalogTable;
/**
 * @return Construit la table des id midi des controleurs analogiques
 */
AnalogTable buildAnalogTable();

////////////////////////////////////////////////////////////////////
// Configuration Hardware
////////////////////////////////////////////////////////////////////

typedef std::map<sfx::hex_t, std::unique_ptr<mcp23017>> MCP23017Array;
/**
 * @brief Fonction appelée pour initialiser la connection avec les registres MCP
 * @return 
 */
MCP23017Array registerMCP(std::initializer_list<sfx::hex_t> adresses);

struct RegStatus
{
    /**
     * @brief Construit un objet pour stoquer l'etat d'un registres MCP
     * Initialise deux valeur par registre ( Reg A et B )
     */
    RegStatus(const mcp23017& reg);
    sfx::hex_t lastReg[2];
    sfx::hex_t olatReg[2];
    sfx::hex_t iodirReg[2];
    sfx::hex_t ipolReg[2];
};
typedef std::map<sfx::hex_t, std::unique_ptr<RegStatus>> RegStateMap;
/**
 * @brief Fonction appelée pour initialiser la liste des Etats des registres MCP
 * @param regs Liste des Registres
 * @param ins Table des Adresses des entrées
 * @param outs Table des Sorties
 */
RegStateMap setupRegStatusMap(const MCP23017Array& regs
    , const InputTable& ins, const OutputTable& outs);

typedef std::array<int, SFX_POT_COUNT> AnalogStatus;
/**
 * @return Construit la liste des valeurs des controleurs analogiques
 */
AnalogStatus setupAnalogControlers(int spi_base, int spi_chan);

////////////////////////////////////////////////////////////////////
// Interface avec le GPIO
////////////////////////////////////////////////////////////////////

typedef std::pair<sfx::hex_t,bool> LogicUpdateRequest;
typedef std::queue<LogicUpdateRequest> LogicUpdateQueue;

/**
 * @brief Fonction appelée pour mettre à jour l'etat des registres MCP
 * @param regs   Liste des Registres
 * @param status Statut des Registres
 * @return la liste des adresses qui ont changée
 */
LogicUpdateQueue UpdateMCP(MCP23017Array& regs, RegStateMap& status
    , InputTable& ins, OutputTable& outs, LogicUpdateQueue outUpdates);

typedef std::pair<sfx::hex_t, int> AnalogUpdateRequest;
typedef std::queue<AnalogUpdateRequest> AnalogUpdateQueue;

/**
 * @brief Fonction appelée pour mettre à jour l'etat des entrées analogiques
 * @param table     Table des Id Midi des entrées analogiques
 * @param values    Liste des Valeurs des entrées analogiques
 * @param hysteresis    Seuil de bruit pour detecter un changement de valeur
 * @return la liste des valeurs qui ont changées
 */
AnalogUpdateQueue UpdateExpression(AnalogTable& table, AnalogStatus& values
    , const int& spi_base, const int& hysteresis);

////////////////////////////////////////////////////////////////////
// Interface avec le système RealTime
////////////////////////////////////////////////////////////////////

/**
 * @brief Structure used to interface the gpio with the JackProcess Graph
 */
struct GPIOJackClient
{
    /**
     * @brief GPIOClient Constructor, create the jack_client that will
     *  update io
     * @param adresses adresses of mcp23017 registers
     */
    GPIOJackClient(std::initializer_list<sfx::hex_t> adresses);
    ~GPIOJackClient();
    
    jack_client_t   *client;
    jack_port_t     *logic_in, *logic_out, *analog_out;
    
    MCP23017Array   regs;
    InputTable      ftable;
    OutputTable     ltable;
    RegStateMap     regStatus;
    
    AnalogTable     atable;
    AnalogStatus    exprStatus;
    
    LogicUpdateQueue     outQueue;
    
    /**
     * @brief Callback Called by the jack process graph
     */
    static int jackCallback(jack_nframes_t nframes, void* arg);
};

#endif /* FOOTSWITCH_H */
