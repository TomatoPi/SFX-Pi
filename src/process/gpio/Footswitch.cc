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
 * File:   Footswitch.cc
 * Author: Space-Computer
 * 
 * Created on 23 juillet 2018, 15:33
 */

#include "Footswitch.h"

#ifndef __SFX_PI__
#error This File is Specific To Processing Environement
#endif

////////////////////////////////////////////////////////////////////
// Configuration du GPIO
////////////////////////////////////////////////////////////////////

#define NAME "GPIO"

/**
 * @return Contiens la table des adresses des differents footswitches 
 *  et leur Adresse MIDI correspondante
 */
InputTable buildFootTable()
{
    InputTable rtn;
    HexTable ma0, mb0, ma1, mb1;
    
    mb0[0x40] = 64;
    mb0[0x20] = 65;
    mb0[0x04] = 66;
    mb0[0x02] = 67;
    
    rtn[0x24][0] = ma0;
    rtn[0x24][1] = mb0;
    
    ma1[0x40] = 68;
    ma1[0x20] = 69;
    ma1[0x04] = 70;
    ma1[0x02] = 71;
    
    rtn[0x20][0] = ma1;
    rtn[0x20][1] = mb1;
    
    return rtn;
}

/**
 * @return Construit la table des adresses des differents leds en fonction
 *  de l'adresse MIDI du switch associé
 */
OutputTable buildLedTable()
{
    OutputTable rtn;
    
    rtn[67] = mcp23017::Adress(0x24, 1, 0x01);
    rtn[66] = mcp23017::Adress(0x24, 1, 0x08);
    rtn[65] = mcp23017::Adress(0x24, 1, 0x10);
    rtn[64] = mcp23017::Adress(0x24, 1, 0x80);
    
    rtn[71] = mcp23017::Adress(0x20, 0, 0x01);
    rtn[70] = mcp23017::Adress(0x20, 0, 0x08);
    rtn[69] = mcp23017::Adress(0x20, 0, 0x10);
    rtn[68] = mcp23017::Adress(0x20, 0, 0x80);
    
    return rtn;
}

/**
 * @return Construit la table des id midi des controleurs analogiques
 */
AnalogTable buildAnalogTable()
{
    AnalogTable rtn;
    
    rtn[0] = 0x00;
    rtn[1] = 0x01;
    rtn[2] = 0x02;
    rtn[3] = 0x03;
    
    rtn[4] = 0x04;
    rtn[5] = 0x05;
    rtn[6] = 0x06;
    rtn[7] = 0x07;
    
    return rtn;
}

////////////////////////////////////////////////////////////////////
// Configuration Hardware
////////////////////////////////////////////////////////////////////

/**< Adresses of GPIO[A|B] Reg */
const sfx::hex_t HEX_GPIO[2] =
{
    mcp23017::HEX_GPIOA,
    mcp23017::HEX_GPIOB
};
/**< Adresses of OLAT[A|B] Reg */
const sfx::hex_t HEX_OLAT[2] =
{
    mcp23017::HEX_OLATA,
    mcp23017::HEX_OLATB
};
/**< Adresses of IODIR[A|B] Reg */
const sfx::hex_t HEX_IODIR[2] =
{
    mcp23017::HEX_IODIRA,
    mcp23017::HEX_IODIRB
};
/**< Adresses of IPOL[A|B] Reg */
const sfx::hex_t HEX_IPOL[2] =
{
    mcp23017::HEX_IPOLA,
    mcp23017::HEX_IPOLB
};

/**
 * @brief Fonction appelée pour initialiser la connection avec les registres MCP
 * @return 
 */
MCP23017Array registerMCP(std::initializer_list<sfx::hex_t> adresses)
{
    MCP23017Array rtn;
    for (auto& adr : adresses)
    {
        auto reg = std::make_unique<mcp23017>(adr, "/dev/i2c-1");
        if (reg->errored())
            sfx::err(NAME, "Failed Register MCP:0x%x", adr);
        else
            rtn[adr] = std::move(reg);
    }
    return rtn;
}

/**
 * @brief Construit un objet pour stoquer l'etat des registres MCP
 * Initialise deux valeur par registre ( Reg A et B )
 */
RegStatus::RegStatus(const mcp23017& reg) :
lastReg(),
olatReg(),
iodirReg(),
ipolReg()
{
    for ( size_t k = 0; k < 2; k++ )
    {
        reg.readReg(HEX_GPIO[k], lastReg[k]);
        olatReg[k] = 0;
        iodirReg[k] = 0;
        ipolReg[k] = 0;
    }
}

/**
 * @brief Fonction appelée pour initialiser la liste des Etats des registres MCP
 * @param regs Liste des Registres
 * @return 
 */
RegStateMap setupRegStatusMap(const MCP23017Array& regs, const InputTable& ins, const OutputTable& outs)
{
    // Create the Status Map
    RegStateMap rtn;
    for (auto& reg : regs)
    {
        rtn[reg.first] = std::make_unique<RegStatus>(*reg.second);
    }
    // Setup Inputs
    for (auto& reg : ins)
    {
        for ( size_t k = 0; k < 2; k++ )
            for (auto& input : reg.second[k])
            {
                /*
                 * Update Register masks
                 * Set given adress to input (1)
                 * And reverse it state (1)
                 * */
                mcp23017::changeBit( rtn[reg.first]->iodirReg[k], 0xff, input.first );
                mcp23017::changeBit( rtn[reg.first]->ipolReg[k] , 0xff, input.first );
                
                sfx::log(NAME, "Registered : FootSwitch : 0x%02x : Adress( MCP(0x%02x) GPIO(%i) Hex(0x%02x) )\n",
                    input.second, reg.first, k, input.first);
            }
    }
    for (auto& output : outs)
    {
        /*
         * Update Register masks
         * Set LED adress to ouput (0)
         * */
        mcp23017::changeBit( rtn[output.second.reg]->iodirReg[output.second.ab], 0x00, output.second.adr );
        sfx::log(NAME, "Registered : LED        : 0x%02x : Adress( MCP(0x%02x) GPIO(%i) Hex(0x%02x) )\n",
            output.first, output.second.reg, output.second.ab, output.second.adr);
    }
    // Upload Configuration in Registers
    for (auto& reg : regs)
        for ( size_t k = 0; k < 2; k++ )
        {
            reg.second->writeReg(HEX_IODIR[k], rtn[reg.first]->iodirReg[k]);
            reg.second->writeReg(HEX_IPOL[k] , rtn[reg.first]->ipolReg[k] );
        }
    return rtn;
}

/**
 * @return Construit la liste des valeurs des controleurs analogiques
 */
AnalogStatus setupAnalogControlers(int spi_base, int spi_chan)
{
    AnalogStatus rtn;
    
    #ifdef __ARCH_LINUX__
    if (wiringPiSetup() == -1)
        sfx::err(NAME, "Failed Setup WiringPi\n");

    mcp3004Setup(spi_base, spi_chan);

    for (size_t i = 0; i < SFX_POT_COUNT; i++)
        rtn[i] = analogRead(spi_base + i);
    
    #endif
    return rtn;
}

////////////////////////////////////////////////////////////////////
// Interface avec le GPIO
////////////////////////////////////////////////////////////////////

/**
 * @brief Fonction appelée pour mettre à jour l'etat des registres MCP
 * @param regs   Liste des Registres
 * @param status Statut des Registres
 */
LogicUpdateQueue UpdateMCP(MCP23017Array& regs, RegStateMap& status
, InputTable& ins, OutputTable& outs, LogicUpdateQueue outUpdates)
{
    LogicUpdateQueue inUpdates;
    for (auto& reg : regs)
    {
        for ( size_t k = 0; k < 2; k++ )
        {
            sfx::hex_t currentReg = 0x00;
            sfx::hex_t mask = status[reg.first]->iodirReg[k];
            
            // Lecture du registre courant
            reg.second->readReg(HEX_GPIO[k], currentReg);
            
            // Verification d'un Changement
            if ( (currentReg & mask) != (status[reg.first]->lastReg[k] & mask) )
            {
                sfx::hex_t adr = (currentReg & mask) ^ (status[reg.first]->lastReg[k] & mask);
                
                /*
                sfx::debug(NAME, "Register( 0x%02x:%lu ) has changed : %i : Adress : 0x%02x\n"
                    , reg.first, k, !!(currentReg&adr), adr );
                //*/
                
                // Récuprération de l'ID du Switch
                HexTable::const_iterator id_itr = ins[reg.first][k].find(adr);
                
                if (id_itr == ins[reg.first][k].end()) sfx::wrn(NAME, "Unknown Adress...\n");
                else
                {
                    inUpdates.push(std::make_pair(id_itr->second, !!(currentReg&adr)));
                    outUpdates.push(std::make_pair(id_itr->second, !!(currentReg&adr)));
                }
            }
            
            // Sauvegarde de l'etat Courant du Registre
            status[reg.first]->lastReg[k] = currentReg;
        }
    }
    while (!outUpdates.empty())
    {
        LogicUpdateRequest lid = outUpdates.front();
        outUpdates.pop();
        
        /*
        sfx::debug(NAME, "Process Update Request : %i:%i\n", lid.first, lid.second);
        //*/
        
        mcp23017::Adress ladr = outs[lid.first];
        sfx::hex_t val = lid.second?0xff:0x00;
        
        if (status.find(ladr.reg) == status.end())
        {
            sfx::wrn(NAME, "Invalid Led Adress : 0x%02x:%lu:0x%02x\n"
                , ladr.reg, ladr.ab, ladr.adr);
            continue;
        }
        //sfx::hex_t old = status[ladr.reg]->olatReg[ladr.ab];

        // Mise à jour du statut du registre
        mcp23017::changeBit(status[ladr.reg]->olatReg[ladr.ab], val, ladr.adr);

        /*
        sfx::debug(NAME, "Update Output : ( 0x%02x:%lu:0x%02x ) : 0x%02x : 0x%02x => 0x%02x\n"
        , ladr.reg, ladr.ab, ladr.adr, val, status[ladr.reg]->olatReg[ladr.ab], old );
        //*/

        // Ecriture du nouvel Etat
        regs[ladr.reg]->writeReg(
            HEX_OLAT[ladr.ab],                  // Write to the OLAT reg
            status[ladr.reg]->olatReg[ladr.ab]  // New value for the reg
        );
    }
    
    return inUpdates;
}

/**
 * @brief Fonction appelée pour mettre à jour l'etat des entrées analogiques
 * @param table     Table des Id Midi des entrées analogiques
 * @param values    Liste des Valeurs des entrées analogiques
 * @param hysteresis    Seuil de bruit pour detecter un changement de valeur
 * @return la liste des valeurs qui ont changées
 * 
 * @warning Non utilisé car les entrées analogiques sont trop bruitées
 */
AnalogUpdateQueue UpdateExpression(AnalogTable& table, AnalogStatus& values
    , const int& spi_base, const int& hysteresis)
{
    AnalogUpdateQueue inUpdates;
    #ifdef __ARCH_LINUX__
    for (size_t i = 0; i < SFX_POT_COUNT; i++)
    {
        /*
        int value = analogRead(spi_base + i);
        int diff = value - values[i];
        
        if (   (diff > hysteresis && diff > 0)
            || (diff < -hysteresis && diff < 0))
        {
            sfx::debug(NAME, "Expr#%i has changed : %i => %i\n", i, values[i], value);
            inUpdates.push(std::make_pair(table[i], value));
        }
        values[i] = value;
        //*/
    }
    #endif
    return inUpdates;
}

////////////////////////////////////////////////////////////////////
// Interface avec le système RealTime
////////////////////////////////////////////////////////////////////
#undef NAME
#define NAME "GPIO-Client"

/**
 * @brief GPIOClient Constructor, create the jack_client that will
 *  update io
 * @param adresses adresses of mcp23017 registers
 */
GPIOJackClient::GPIOJackClient(std::initializer_list<sfx::hex_t> adresses):
#ifdef __ARCH_LINUX__
client(nullptr),
logic_in(nullptr),
logic_out(nullptr),
analog_out(nullptr),
#endif
        
regs(registerMCP(adresses)),
ftable(buildFootTable()),
ltable(buildLedTable()),
regStatus(setupRegStatusMap(regs, ftable, ltable)),

atable(buildAnalogTable()),
exprStatus(setupAnalogControlers(100, 0)),
        
outQueue()
{
    client = sfx::openJackClient(NAME);

	jack_set_process_callback (client, jackCallback, this);
    
    logic_in = sfx::registerMidiInput (client, "LogicIn");
    logic_out= sfx::registerMidiOutput (client, "LogicOut");
    analog_out=sfx::registerMidiOutput (client, "AnalogOut");
    
    sfx::activateClient(client);
}
GPIOJackClient::~GPIOJackClient()
{
    jack_client_close (client);
}

/**
 * @brief Callback Called by the jack process graph
 */
int GPIOJackClient::jackCallback(jack_nframes_t nframes, void* arg)
{
    GPIOJackClient* gpio = (GPIOJackClient*)arg;
    
    void* port_buff = jack_port_get_buffer(gpio->logic_in, nframes);
    
    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buff);
    
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, port_buff, i);
        /*
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
            , i, in_event.time, *(in_event.buffer));
        //*/
        
        if (sfx::Midi_is_UpdateOutput(in_event.buffer))
        {
            gpio->outQueue.push(std::make_pair(sfx::Midi_UpdateOutput_ID(in_event.buffer)
                , sfx::Midi_UpdateOutput_Status(in_event.buffer)));
        }
        else sfx::wrn(NAME, "Unknown Event Type\n");
    }
    
    LogicUpdateQueue logUpdates = UpdateMCP(gpio->regs, gpio->regStatus
            , gpio->ftable, gpio->ltable
            , gpio->outQueue);

    void* logic_buffer = jack_port_get_buffer(gpio->logic_out, nframes);
    jack_midi_clear_buffer(logic_buffer);
    
    while (!logUpdates.empty())
    {
        LogicUpdateRequest rqst = logUpdates.front();
        logUpdates.pop();

        sfx::Midi_reserve_ControlChange(logic_buffer, 0, 0, rqst.first, rqst.second?0x7F:0);
    }
    
    AnalogUpdateQueue exprUpdates = UpdateExpression(gpio->atable, gpio->exprStatus, 100, 50);

    void* expr_buffer = jack_port_get_buffer(gpio->analog_out, nframes);
    jack_midi_clear_buffer(expr_buffer);
    
    while (!exprUpdates.empty())
    {
        AnalogUpdateRequest rqst = exprUpdates.front();
        exprUpdates.pop();
        
        sfx::Midi_reserve_14bitControlChange(expr_buffer, 0, rqst.first, sfx::Midi_AnalogChange, rqst.second);
    }
    
    while(!gpio->outQueue.empty()) gpio->outQueue.pop();
    return 0;
}
