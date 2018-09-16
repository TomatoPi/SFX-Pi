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
 * File:   CommandRegistry.cc
 * Author: Space-Computer
 * 
 * Created on 12 septembre 2018, 13:29
 */

#include "CommandRegistry.h"

#define NAME ("Command-Registry")

////////////////////////////////////////////////////////////////////
// Command struct
////////////////////////////////////////////////////////////////////

namespace
{
    CommandTable CommandRegistry;
    std::string prev_cmd;
}
CommandTable& getCommandTable()
{
    return CommandRegistry;
}
void logCommandTable()
{
    sfx::log(NAME, "Registered Commands : \n");
    sfx::sfxStream
        << sfx::formatString("%-15s || %-3s | %-3s | %-40s\n",
            std::string("Clef"), std::string("Min"), std::string("Max")
            ,std::string("Brief"))
        << "================||=====|=====|=========================================\n";
    for (auto& cmd : CommandRegistry)
    {
        sfx::sfxStream << sfx::formatString("%-15s || %-3u | %-3u | %-40s\n",
            cmd.first, cmd.second.minArg, cmd.second.maxArg, cmd.second.brief);
    }
    sfx::sfxStream
        << "----------------++-----+-----+-----------------------------------------\n";
}

////////////////////////////////////////////////////////////////////
// Command registry
////////////////////////////////////////////////////////////////////

/**
 * Fonction pour ajouter une commande à nombre strict de paramètres à
 *  la table des commandes
 *
 * @param key       clef de la commande (premier mot)
 * @param help      message d'aide de la commande
 * @param argCount  nombre de paramètres à fournir
 * @param callback  fonction à appeller poru excecuter la commande
 */
void registerStrictArgCommand(std::string key
    , std::string brief
    , std::string help
    , size_t argCount
    , Command::command_f callback)
{
    if ( CommandRegistry.find(key) != CommandRegistry.end() )
    {
        sfx::err(NAME, "Key : \"%s\" is already used\n", key);
        return;
    }
    
    CommandRegistry[key] = Command{
        .key = key
        ,.brief = brief
        ,.help = help
        ,.minArg = argCount
        ,.maxArg = argCount
        ,.callback = callback
    };
}
/**
 * Fonction pour ajouter une commande à nombre variable de paramètres à
 *  la table des commandes
 * 
 * @param key       clef de la commande (premier mot)
 * @param help      message d'aide de la commande
 * @param argMin    nombre minimal de paramètres
 * @param argMax    nombre maximal de paramètres
 * @param callback  fonction à appeller poru excecuter la commande
 */
void registerVarArgCommand(std::string key
    , std::string brief
    , std::string help
    , size_t argMin
    , size_t argMax
    , Command::command_f callback)
{
    if ( CommandRegistry.find(key) != CommandRegistry.end() )
    {
        sfx::err(NAME, "Key : \"%s\" is already used\n", key);
        return;
    }
    
    CommandRegistry[key] = Command{
        .key = key
        ,.brief = brief
        ,.help = help
        ,.minArg = argMin
        ,.maxArg = argMax
        ,.callback = callback
    };
}

////////////////////////////////////////////////////////////////////
// Command execution
////////////////////////////////////////////////////////////////////

/**
 * Fonction pour découper une chaine de caractères en vecteur de sous
 *  chaines, selon une liste de caractères de séparation, et un caractère
 *  d'antiséparation
 * @param input         chaine à découper
 * @param sperator      chaine des séparateurs
 * @param antiseparator antiséparateur (entre deux antiséparateur, les
 *  séparateurs sont ignorés et conservés)
 * @return la liste des sous chaines extraite
 */
std::vector<std::string> parseSimpleString( std::string input,
        const char* sperator, const char antiseparator )
{
    // If given string is empty, return empty vector
    if ( input.size() == 0 ){
        return std::vector<std::string>();
    }
    // First remove white spaces before first token
    input.erase( 0, input.find_first_not_of(sperator));

    // Token extraction variables
    uint16_t begin = 0;
    uint16_t end = 0;
    std::vector<std::string> out;

    // Extract All tokens
    while( end != std::string::npos && end < input.size() && begin < input.size() ){
        begin = input.find_first_not_of(sperator, end);
        end = input.find_first_of(sperator, begin);

        if ( begin < input.size() ){

            // If a spaced sequence is detected get it all in a single row
            if ( input.at(begin) == antiseparator ){

                begin = input.find_first_not_of(antiseparator, begin);
                end = input.find_first_of(antiseparator, begin);

                out.push_back(input.substr( begin, end-begin ));

                end = input.find_first_of(sperator, end);
            }
            else{

                out.push_back(input.substr( begin, end-begin ));
            }
        }
    }

    return out;
}
/**
 * Fonction à appeler pour interpreter une entrée au termial
 * @param terminal_input chaine à interpréter
 */
void performCommandHandling(std::string terminal_input)
{
    if ( terminal_input.size() != 0 )
    {
        std::vector<std::string> args = parseSimpleString( terminal_input, " ", '"' );
        if (args.size() != 0 )
        {
            if (CommandRegistry.find(args[0]) != CommandRegistry.end())
            {
                tryPerformCommand(args);

                if (args[0] != "p") 
                {
                    prev_cmd = args[0];
                }
            }
            else
            {
                sfx::wrn(NAME, "Unkown Command : \"%s\"\n", args[0]);
            }
        }
    }
}
/**
 * Fonction permettant d'executer une commande à partir de la liste
 *   des arguments parsés
 */
void tryPerformCommand(std::vector<std::string> args)
{
    try
    {
        std::string key = args[0];
        args.erase(args.begin());
        Command cmd = CommandRegistry[key];
        
        if ( args.size() >= cmd.minArg && args.size() <= cmd.maxArg )
            (*cmd.callback)(args);
        else
            sfx::wrn("Command", "\"%s\" : Invalid Argument Count : %u providen\nUsage : %s\n"
                    ,key, args.size(), cmd.help);
    }
    catch( std::invalid_argument const& e )
    {
        sfx::err(NAME, "Invalid Argument : %s\n", e.what());
    }
}