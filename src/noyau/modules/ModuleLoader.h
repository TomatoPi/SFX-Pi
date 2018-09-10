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
 * File:   ModuleLoader.h
 * Author: Space-Computer
 *
 * Created on 30 août 2018, 22:21
 */

#ifndef MODULELOADER_H
#define MODULELOADER_H

#include <functional>
#include <memory>
#include <map>

#include <iostream>
#include <fstream>

#include "noyau/log.h"

#include "ModuleBase.h"

#ifdef __UNIX__
#include <dlfcn.h>
#elif defined(__WINDOWS__)
#endif

////////////////////////////////////////////////////////////////////
// Module Handling structures
////////////////////////////////////////////////////////////////////

typedef std::map<std::string,std::shared_ptr<Module>> LoadedModulesTable;

LoadedModulesTable& getModuleTable();

/**
 * @brief Fonction à appeler au lancement du programme pour charger les modules du fichier
 *  de configuration donné
 * @param config_path chemin du fichier de configuration
 * @return 0 on success
 */
int loadModuleTable(std::string config_path);

void unloadModuleTable();

/**
 * Fonction pour afficher la liste des modules chargés
 */
void logLoadedModuleTable();

////////////////////////////////////////////////////////////////////
// ModuleLoading Structures
////////////////////////////////////////////////////////////////////

/**
 * @brief Fonction utilisée pour charger un Module
 * @param path Chemin d'accès au module à charger
 * @return Pointeur vers le module chargé ou nullptr si le chargement a échoué
 */
int loadModule(std::string path);

/**
 * @brief Fonction à appeller pour décharger un Module
 * @param module module à décharger
 */
void unloadModule(std::string unique_name);

/**
 * @brief Fonction de wrapping pour charger une fonction depuis une librairie dynamique
 * @param name Nom de la fonction à charger
 * @return pointeur vers la fonction chargée
 * @throw ios_base::failure
 */
void* load_function(void* handle, std::string name);


#endif /* MODULELOADER_H */
