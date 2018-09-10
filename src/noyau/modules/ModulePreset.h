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
 * File:   ModulePreset.h
 * Author: Space-Computer
 *
 * Created on 8 septembre 2018, 01:46
 */

#ifndef MODULEPRESET_H
#define MODULEPRESET_H

#include <set>

#include "ModuleBase.h"
#include "noyau/serial.h"

////////////////////////////////////////////////////////////////////
// Modules
////////////////////////////////////////////////////////////////////

/**
 *  string : nom unique
 *  string : version
 */
void serialize_infos(std::ofstream& flux, const Module::ShortInfo* obj);
int deserialize_infos(std::ifstream& flux, Module::ShortInfo* obj);

////////////////////////////////////////////////////////////////////
// EffectUnit Internals
////////////////////////////////////////////////////////////////////

/**
 *  usize_t : nombre de paramètres
 *  {
 *      string : nom unique
 *      hex_t  : valeur
 *  }
 */
void serialize_bank(std::ofstream& flux, const EffectUnit::ParamArray* obj);
int deserialize_bank(std::ifstream& flux, EffectUnit::ParamArray* obj);

/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *      bank  : la banque
 *  }
 */
void serialize_banktable(std::ofstream& flux, const EffectUnit::BankTable* obj);
int deserialize_banktable(std::ifstream& flux, EffectUnit::BankTable* obj);

/**
 *  usize_t : nombre de banques
 *  {
 *      hex_t : id de la banque
 *  }
 */
void serialize_bankidlist(std::ofstream& flux, const EffectUnit::BankIdList* obj);
int deserialize_bankidlist(std::ifstream& flux, EffectUnit::BankIdList* obj);

/**
 *  usize_t : nombre de liens
 *  {
 *      hex_t : channel du cc
 *      hex_t : source du cc
 *      usize_t : nombre de cibles
 *      {
 *          string : nom du slot
 *      }
 *  }
 */
void serialize_linktable(std::ofstream& flux, const EffectUnit::LinkTable* obj);
int deserialize_linktable(std::ifstream& flux, EffectUnit::LinkTable* obj);

////////////////////////////////////////////////////////////////////
// Preset Files
////////////////////////////////////////////////////////////////////

/**
 *  flag_t : EffectUnit_Preset_File
 *  Module Infos
 *  string : Nom de l'effet
 *  string : Nom du fichier des banques
 *  string : Nom du fichier des links
 */
int save_PresetFile(std::string file_path, const EffectUnit* obj);
int load_PresetFile(std::string file_path, EffectUnit* obj);

/**
 *  flag_t  : EffectUnit_Bank_File
 *  Module Infos
 *  BankTable
 *  BankIdList
 */
int save_BankFile(std::string file_path, const EffectUnit* obj);
int load_BankFile(std::string file_path, EffectUnit* obj);

/**
 *  flag_t  : EffectUnit_Link_File
 *  Module Infos
 *  LinksTable
 */
int save_LinkFile(std::string file_path, const EffectUnit* obj);
int load_LinkFile(std::string file_path, EffectUnit* obj);

#endif /* MODULEPRESET_H */
