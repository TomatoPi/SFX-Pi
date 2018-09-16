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
 * File:   CommandRegistry.h
 * Author: Space-Computer
 *
 * Created on 12 septembre 2018, 13:29
 */

#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include <map>
#include <vector>

#include "noyau/log.h"

////////////////////////////////////////////////////////////////////
// Command struct
////////////////////////////////////////////////////////////////////

struct Command
{
    std::string key;
    std::string brief;
    std::string help;
    
    size_t minArg;
    size_t maxArg;
    
    typedef void (*command_f)(std::vector<std::string>);
    command_f callback;
};

typedef std::map<std::string, Command> CommandTable;
CommandTable& getCommandTable();
void logCommandTable();

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
    , Command::command_f callback);
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
    , Command::command_f callback);

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
        const char* sperator, const char antiseparator );

/**
 * Fonction à appeler pour interpreter une entrée au termial
 * @param terminal_input chaine à interpréter
 */
void performCommandHandling(std::string terminal_input);

/**
 * Fonction permettant d'executer une commande à partir de la liste
 *   des arguments parsés
 */
void tryPerformCommand(std::vector<std::string> args);

#endif /* COMMANDREGISTRY_H */
