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
 * File:   Module_EffectUnit.cc
 * Author: Space-Computer
 * 
 * Created on 30 août 2018, 20:22
 */

#include <condition_variable>
#include <utility>

#include "ModuleBase.h"

#define NAME "Effect-API"

////////////////////////////////////////////////////////////////////
// Informations sur l'Unité de Traitement
////////////////////////////////////////////////////////////////////

void EffectUnit::logCompleteInfos()
{
    sfx::log(unique_name, "Unit's Informations :\n");
    sfx::sfxStream
            << sfx::formatString(" Internal Name : \"%s\"\n Display Name : \"%s\"\n Related Module : \"%s\"\n",
            unique_name, display_name, module.lock()->getInformations().unique_name);
    // Print Unit's Ports
    sfx::sfxStream
            << "---------------------+----------------------++----------------------+---------------------\n"
            << "     Audio Inputs    |    Audio Outputs     ||      Midi Inputs     |     Midi Outputs    \n"
            << "=====================+======================++======================+=====================\n";
    size_t n = std::max({audioIns.size(), audioOuts.size(), midiIns.size(), midiOuts.size()});
    for (size_t i = 0; i < n; ++i)
    {
        sfx::sfxStream
                << sfx::formatString("%20s | %20s || %20s | %20s\n",
                ((i < audioIns .size())?audioIns [i].name : std::string("")),
                ((i < audioOuts.size())?audioOuts [i].name : std::string("")),
                ((i < midiIns .size())?midiIns [i].name : std::string("")),
                ((i < midiOuts .size())?midiOuts [i].name : std::string("")));
    }
    sfx::sfxStream
            << "---------------------+----------------------++----------------------+---------------------\n";
    sfx::todo(unique_name, "Log Banks Informations\n");
    this->logLinkedSlots();
}

////////////////////////////////////////////////////////////////////
// Gestion des Ports
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Gestion des Slots
////////////////////////////////////////////////////////////////////

/**
 * Fonction utilisée pour connecter un slot à un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom interne du slot à connecter
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::linkSlot(sfx::hex_pair_t cc, std::string slot)
{
    //sfx::debug(name, "Try Link slot : \"%s\" to cc : %i:%i \n", slot, cc.first, cc.second);

    if (slots.find(slot) == slots.end())
        return MISSING_SLOT;

    if (links.find(cc) == links.end())
        links[cc] = std::vector<std::string>();

    //if (links[cc].find(slot) != links[cc].end())
    //    return 1;

    //sfx::debug(name, "Success !\n");
    links[cc].push_back(slot);
    return 0;
}

/**
 * Fonction utilisée pour déconnecter un slot à d'un cc midi
 * @param cc paire <channel,source> du cc à connecter
 * @param slot nom interne du slot à connecter
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, 1 if link doesn't exist
 */
int EffectUnit::unlinkSlot(sfx::hex_pair_t cc, std::string slot)
{
    if (slots.find(slot) == slots.end())
        return MISSING_SLOT;

    if (links.find(cc) == links.end())
        return 1;

    //if (links[cc].find(slot) == links[cc].end())
    //    return 1;

    bool missing = true;

    for (std::vector<std::string>::iterator itr = links[cc].begin();
            itr != links[cc].end();
            ++itr)
    {
        if (slot == *itr)
        {
            itr = links[cc].erase(itr);
            missing = false;
        }
    }

    if (links[cc].size() == 0) links.erase(cc);

    return (int) missing;
}

/**
 * Fonction utilisée pour activer un slot
 * @param slot le nom interne du slot à activer
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::enableSlot(std::string slot)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;

    sfx::debug(unique_name, "Enabled slot : \"%s\"\n", slot);
    slots[slot].status = SlotStatus::ENABLED;
    return 0;
}

/**
 * Fonction utilisée pour désactiver un slot
 * @param slot le nom interne du slot à désactiver
 * @return 0 on success, MISSING_SLOT if slot doesn't exist
 */
int EffectUnit::disableSlot(std::string slot)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;

    sfx::debug(unique_name, "Disabled slot : \"%s\"\n", slot);
    slots[slot].status = SlotStatus::DISABLED;
    return 0;
}

/**
 * Fonction utiliée pour changer la valeur d'un slot
 *  Si le slot est désactivé la fonction est sans effet
 * @param slot nom interne du slot à changer
 * @param value valleur à assigner au slot (sur 7bit)
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
 */
int EffectUnit::setSlotValue(std::string slot, sfx::hex_t value, bool force)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;

    //sfx::debug(NAME, "1 Called slot : \"%s\" : Status : %i\n", slot, slots[slot].status);

    if (!!(slots[slot])) // If slot is enabled
    {
        float v = (*slots[slot].slot->callback)(value, this);
        slots[slot].value = value;
        sfx::debug("Native-MIDI", "Called slot : \"%s\" with value : %i => %f\n", slot, value, v);
    }
    else if (force) // Else if slot is disabled, save value anyway
    {
        slots[slot].value = value;
    }
    else return DISABLED_SLOT; // Else return an error

    return 0;
}

/**
 * Fonction utiliée pour récuperer la valeur interne d'un slot
 *  Si le slot est désactivé la fonction est sans effet.
 *  laisser à nullptr les valeurs non souhaitées
 * @param slot nom interne du slot ciblé
 * @param x_result pointeur pour récuperer la valeur sur 7bit du slot
 * @param f_result pointeur pour récuperer la valeur interne du slot
 * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
 */
int EffectUnit::getSlotValue(std::string slot, sfx::hex_t* x_result, float* f_result)
{
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;

    if (!!(slots[slot]))
    {
        float value = (*slots[slot].slot->callback)(255, this);

        if (x_result) *x_result = slots[slot].value;
        if (f_result) *f_result = value;
    }
    else return DISABLED_SLOT;

    return 0;
}

void EffectUnit::logLinkedSlots() const
{
    sfx::log(unique_name, "Linked Slots : \n");
    sfx::sfxStream
            << "----------+----------+--------------------\n"
            << sfx::formatString("%-10s|%-10s|%-20s\n",
            std::string("Channel"), std::string("Source"), std::string("Slot"))
            << "==========|==========|====================\n";
    for (auto& link : links)
        for (auto& slot : link.second)
        {
            sfx::sfxStream << sfx::formatString("%-10i|%-10i|%-20s\n"
                    , link.first.first, link.first.second
                    , slot);
        }
    sfx::sfxStream << "----------+----------+--------------------\n";
}

#ifdef __ARCH_LINUX__

/**
 * Fonction à appeller dans la boucle de process pour traiter les CC midi et 
 *  déclancher les slots eventuels
 * @param event pointeur vers le buffer de l'evenement midi 
 */
void EffectUnit::veryfyAndComputeCCMessage(sfx::hex_t* event)
{
    sfx::hex_t ccc = sfx::Midi_read_CCChannel(event);
    sfx::hex_t ccs = sfx::Midi_read_CCSource(event);
    sfx::hex_t val = sfx::Midi_read_CCValue(event);
    sfx::hex_pair_t cc = std::make_pair(ccc, ccs);

    sfx::debug("Native-MIDI", "Process Midi Event : %i %i\n", ccc, ccs);

    if (links.find(cc) != links.end())
    {
        for (auto& slot : links[cc])
            if (slots.find(slot) != slots.end())
            {
                sfx::debug("Native-MIDI", "Call slot : \"%s\" with value : %i\n", slots[slot].slot->internal_name, val);
                this->setSlotValue(slot, val);
            }
            else
            {
                sfx::wrn("Native-MIDI", "Unable to find slot : \"%s\" linked to CC : %i %i\n", slot, ccc, ccs);
            }
    }
}
#endif

////////////////////////////////////////////////////////////////////
// Gestion des Banques
////////////////////////////////////////////////////////////////////

/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
int EffectUnit::setBank(sfx::hex_t id)
{
    if (banks.find(id) == banks.end()) return -1;

    // Update current_bank iterator
    for (BankIdList::iterator itr = banks_order.begin();
            itr != banks_order.end();
            ++itr)
        if (*itr == id)
        {
            current_bank = itr;
            break;
        }

    // Update all params
    this->updateCurrentBank();
    return 0;
}

/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
void EffectUnit::nextBank()
{
    BankIdList::iterator nitr(current_bank);
    ++nitr;
    if (nitr == banks_order.end())
        nitr = banks_order.begin();

    // Update all params
    current_bank = nitr;
    this->updateCurrentBank();
}

/**
 * @brief Change current bank
 * @pre current_bank bank is valid
 * @post current_bank bank is valid
 **/
void EffectUnit::prevBank()
{
    BankIdList::iterator nitr(current_bank);

    if (nitr == banks_order.begin())
        nitr = banks_order.end();
    --nitr;

    // Update all params
    current_bank = nitr;
    this->updateCurrentBank();
}

/**
 * Fonction interne utilisée pour mettre à jour l'effet en appelant tout les slots
 *  actifs de la banque courante
 * @pre current_bank bank is valid
 */
void EffectUnit::updateCurrentBank()
{
    for (auto& slot : banks[*current_bank])
    {
        this->setSlotValue(slot.first, slot.second, true);
    }
}

/**
 * @brief method to add a bank to the effect
 *  Usage :
 *      createBank() to create a bank filled with default values
 *      createBank(id, size, vals) to create a bank from a valid array of values
 * @pre if specified, bank is a valid expansion of effect's config tree
 */
int EffectUnit::createBank()
{
    return this->createBank(bank_id_manager.reserveUID());
}

int EffectUnit::createBank(sfx::hex_t id)
{
    // If bank doesn't exist create it
    if (banks.find(id) == banks.end())
    {

        ParamArray array;
        for (auto& slot : slots)
            array[slot.first] = slot.second.slot->default_value;

        banks[id] = array;
        return 0;
    }
        // Else this is an error
    else return EXISTING_BANK;
}

/**
 * @brief method to duplicate a bank of the effect
 *  Usage :
 *      copyBank(sid) to copy bank[sid] to a new one
 *      copyBank(sid, tid) to copy bank[sid] in bank[tid]
 * @pre sid != tid
 */
int EffectUnit::copyBank(sfx::hex_t sid, sfx::hex_t tid)
{
    assert(sid != tid);

    if (banks.find(sid) != banks.end())
    {

        if (banks.find(tid) == banks.end()) this->createBank(tid);

        for (auto& slot : banks[tid])
        {
            slot.second = banks[sid][slot.first];
        }

        if (*current_bank == tid) this->updateCurrentBank();

        return 0;
    }
    else return MISSING_BANK;
}

/**
 * @brief Method used to remove a bank from the effect
 *  if the bank removed is the last one, automaticaly regenerate the default bank
 * @pre _currentBank bank is valid
 * @post _currentBank bank is valid
 */
int EffectUnit::removeBank(sfx::hex_t id)
{
    assert(current_bank != banks_order.end());

    if (banks.find(id) == banks.end()) return MISSING_BANK;
    banks.erase(id);

    bool updateAll = false;

    if (*current_bank == id)
    {
        if (current_bank != banks_order.begin())
            --current_bank;
        else if (current_bank != --banks_order.end())
            ++current_bank;

        updateAll = true;

        current_bank = banks_order.erase(current_bank);
    }
    else banks_order.remove_if([id](sfx::hex_t n)->bool {
            return n == id;
        });

    bank_id_manager.releaseUID(id);

    // Always keep one bank inside the ParamSet
    if (banks.size() == 0)
    {
        this->createBank();
        current_bank = banks_order.begin();
        updateAll = true;
    }

    if (updateAll) this->updateCurrentBank();

    assert(current_bank != banks_order.end());
    return 0;
}

////////////////////////////////////////////////////////////////////
// Constructeur / Destructeur
////////////////////////////////////////////////////////////////////

EffectUnit::EffectUnit(std::weak_ptr<Module> module_weak) :
unique_name(""),
display_name(""),
module(module_weak),

datas(nullptr),

audioIns(), audioOuts(), midiIns(), midiOuts(),
portsReg(),

links(),
slots(),

banks(),
banks_order(),
current_bank(),
bank_id_manager(0)
{
    // Verouillage du Module
    std::shared_ptr<Module> module_ptr;
    if (!(module_ptr = module_weak.lock()))
    {
        sfx::err(NAME, "Can't Create Effect : Module Has Been Unloaded\n");
        throw std::runtime_error("Module_Expired");
    }

    // Creation des slots
    for (auto& slot : module_ptr->getSlotTable())
    {
        slots[slot.first] = SlotStatus{
            .slot = slot.second,
            .value = 0,
            .status = SlotStatus::ENABLED
        };
    }

#ifdef __ARCH_LINUX__
    // Construction du client Jack
    client = sfx::openJackClient(module_ptr->getInformations().unique_name);
    jack_set_process_callback(client, module_ptr->getProcessCallback(), this);

    unique_name = jack_get_client_name(client);
#else
    int i = 0;
    do
    {
        unique_name = sfx::formatString("%s_%02i"
                , module_ptr->getInformations().unique_name,
                i);
        ++i;
    }
    while (UnitsTable.find(unique_name) != UnitsTable.end()
            && !module_ptr->addRelatedUnit(unique_name));
#endif
    display_name = module_ptr->getInformations().name;

    // Allocation des Ports
    this->registerMidiInput("Modulation");
    this->registerMidiOutput("Through");

    // Construction des datas du Module et allocation des ports specifiques
    datas = module_ptr->buildEffectUnitDatas(this);
}

EffectUnit::~EffectUnit()
{
#ifdef __ARCH_LINUX__
    jack_client_close(client);
#endif
    if (auto module_ptr = module.lock())
    {
        module_ptr->destroyEffectUnitDatas(datas);
        module_ptr->removeRelatedUnit(unique_name);
    }
    else
    {
        sfx::err(NAME, "Can't Properly Destroy Effect Unit's Datas : Module Has Expired\n");
    }
}

////////////////////////////////////////////////////////////////////
// EffectUnit Handling
////////////////////////////////////////////////////////////////////
EffectUnit::EffectUnitsTable EffectUnit::UnitsTable = EffectUnit::EffectUnitsTable();

const EffectUnit::EffectUnitsTable& EffectUnit::getEffectUnitsTable()
{
    return EffectUnit::UnitsTable;
}

void EffectUnit::logEffectUnitsTable()
{
    sfx::log(NAME, "Loaded Units Table : \n");
    sfx::sfxStream
            << "--------------------+--------------------+--------------------\n"
            << sfx::formatString("%-20s|%-20s|%-20s\n"
            , std::string("Nom Interne"), std::string("Nom d'affichage"), std::string("Module"))
            << "====================|====================|====================\n";

    for (auto& unit : UnitsTable)
    {
        sfx::sfxStream << sfx::formatString("%-20s|%-20s|%-20s\n"
                , unit.first, unit.second->display_name
                , unit.second->module.lock()->getInformations().unique_name);
    }
    sfx::sfxStream << "--------------------+--------------------+--------------------\n";
}

void EffectUnit::logEffectUnit(std::string unit_name)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
    }
    else UnitsTable[unit_name]->logCompleteInfos();
}

/**
 * Fonction pour creer une Unité de Traitement à partir d'un module
 *  l'unité générée sera celle avec les paramètres par défault
 * @param module_name nom interne du module à utiliser
 * @return 0 on success
 */
int EffectUnit::buildEffectUnitFromModule(const std::string& module_name)
{
    sfx::debug(NAME, "Build effect From Module : \"%s\" ... ", module_name);

    try
    {
        // First find the proper Module
        if (Module::getModuleTable().find(module_name) == Module::getModuleTable().end())
            throw std::runtime_error("Module_Doesnt_Exist");

        // Then create an Effect Unit from the Module
        std::shared_ptr<EffectUnit> unit = std::make_shared<EffectUnit>(Module::getModule(module_name));

        // Fill the Effect With default Values
        unit->createBank();
        unit->current_bank = unit->banks_order.begin();
        unit->updateCurrentBank();

        // Finaly activate the Effect Unit
#ifdef __ARCH_LINUX__
        unit->activateJackClient();
#endif

        // Then add It to the Units Table
        if (UnitsTable.find(unit->unique_name) != UnitsTable.end())
            throw std::runtime_error("Unique_Name_Duplication");

        UnitsTable[unit->unique_name] = unit;

        sfx::sfxStream << "Done\n";
    }
    catch (std::runtime_error const& e)
    {
        sfx::err(NAME, "Failed Build Unit From \"%s\" : %s\n", module_name, e.what());
        return 1;
    }
    return 0;
}

/**
 * Fonction pour creer une Unité à partir d'un fichier Preset Sauvegardé
 *  précédement
 * @param file_path Chemin du fichier à interpreter
 * @return 0 on success
 */
int EffectUnit::buildEffectUnitFromPresetFile(const std::string& file_path)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load effect from file : \"%s\" ... \n", file_path);

    try
    {
        // Open the file
        sfx::debug(NAME, "Open File ... \n");
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");

        // Read file header
        sfx::debug(NAME, "Read Header ... \n");
        sfx::serial::controlValue(flux, sfx::serial::EffectUnit_Preset_File, "Invalid_File_Flag");

        Module::ShortInfo infos = Module::deserialize_infos(flux);

        // Verify if corresponding module has been loaded
        sfx::debug(NAME, "Verify Module ... \n");
        if (Module::getModuleTable().find(infos.unique_name) == Module::getModuleTable().end())
            throw std::ios_base::failure("Module_Type_Invalid");

        sfx::debug(NAME, "Get Module ... \n");
        std::shared_ptr<Module> module_ptr;
        if (!(module_ptr = Module::getModule(infos.unique_name).lock()))
            throw std::runtime_error("Module_Expired");

        // Verify module version
        sfx::debug(NAME, "Verify Version ... \n");
        if (infos.version != module_ptr->getInformations().version)
            sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                infos.version, module_ptr->getInformations().version);

        // Then create an Effect Unit from the Module
        sfx::debug(NAME, "Create Unit ... \n");
        std::shared_ptr<EffectUnit> unit = std::make_shared<EffectUnit>(module_ptr);

        // Read Effect Name
        sfx::debug(NAME, "Read Name ... \n");
        std::string name = sfx::serial::read<std::string>(flux);
        unit->display_name = name;

        // Read banks location
        sfx::debug(NAME, "Read Banks Location ... \n");
        std::string banks_file = sfx::serial::read<std::string>(flux);
        // Read links location
        sfx::debug(NAME, "Read Links Location ... \n");
        std::string links_file = sfx::serial::read<std::string>(flux);

        // Read bank file
        sfx::debug(NAME, "Read Banks File ... \n");
        if (load_BankFile(banks_file, unit))
            throw std::ios_base::failure("Failed read bank file");

        // Read links file
        sfx::debug(NAME, "Read Links File ... \n");
        if (load_LinkFile(links_file, unit))
            throw std::ios_base::failure("Failed read links file");

        // Close the file
        sfx::debug(NAME, "Close File ... \n");
        flux.close();

        // Finaly activate the Effect Unit
#ifdef __ARCH_LINUX__
        sfx::debug(NAME, "OActivate Client ... \n");
        unit->activateJackClient();
#endif

        // Then add It to the Units Table
        sfx::debug(NAME, "Verify name Unicity ... \n");
        if (UnitsTable.find(unit->unique_name) != UnitsTable.end())
            throw std::runtime_error("Unique_Name_Duplication");

        sfx::debug(NAME, "Add Unit ... \n");
        UnitsTable[unit->unique_name] = unit;

        sfx::debug(NAME, "Done ... \n");
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    catch (std::runtime_error const& e)
    {
        sfx::err(NAME, "Failed Load Unit From \"%s\" : %s\n", file_path, e.what());
        return 1;
    }
    return 0;
}

/**
 * Fonction pour detruire une unité
 * @param unit_name nom interne de l'unité à detruire
 * @return 0 on success
 */
int EffectUnit::destroyEffectUnit(std::string unit_name)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    UnitsTable.erase(unit_name);
    return 0;
}

/**
 * Fonction pour sauvegarder une unité
 * @param unit_name nom interne de l'unité à sauvegarder
 * @return 0 on success
 */
int EffectUnit::saveEffectUnit(std::string unit_name, std::string file,
        std::string bank_file, std::string link_file)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    if (bank_file == "") bank_file = file + std::string("_banks");
    if (link_file == "") link_file = file + std::string("_links");

    return save_PresetFile(file, UnitsTable[unit_name], bank_file, link_file);
}

////////////////////////////////////////////////////////////////////

/**
 * Fonction pour sauvegarder les banques d'une unité dans un fichier
 * @param unit_name nom interne de l'unité
 * @param file fichier cible
 * @return 0 on success
 */
int EffectUnit::saveEffectUnitBanks(std::string unit_name, std::string file)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return save_BankFile(file, UnitsTable[unit_name]);
}

/**
 * Fonction pour Charger les banques d'une unité depuis un fichier
 * @param unit_name nom interne de l'unité
 * @param file fichier cible
 * @return 0 on success
 */
int EffectUnit::saveEffectUnitLinks(std::string unit_name, std::string file)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return save_LinkFile(file, UnitsTable[unit_name]);
}

/**
 * Fonction pour sauvegarder les links Midi d'une unité dans un fichier
 * @param unit_name nom interne de l'unité
 * @param file fichier cible
 * @return 0 on success
 */
int EffectUnit::loadEffectUnitBanks(std::string unit_name, std::string file)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return load_BankFile(file, UnitsTable[unit_name]);
}

/**
 * Fonction pour Charger les links Midi d'une unité depuis un fichier
 * @param unit_name nom interne de l'unité
 * @param file fichier cible
 * @return 0 on success
 */
int EffectUnit::loadEffectUnitLinks(std::string unit_name, std::string file)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return load_LinkFile(file, UnitsTable[unit_name]);
}

////////////////////////////////////////////////////////////////////

/**
 * Wrapper for the Unit's linkSlot method
 * @param unit_name internal name of targeted unit
 * @param cc midi cc source
 * @param slot targeted slot
 * @return 0 on success
 */
int EffectUnit::linkEffectUnitSlot(std::string unit_name, sfx::hex_t ccc, sfx::hex_t ccs, std::string slot)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return UnitsTable[unit_name]->linkSlot(std::make_pair(ccc, ccs), slot);
}

/**
 * Wrapper for the Unit's unlinkSlot method
 * @param unit_name internal name of targeted unit
 * @param cc midi cc source
 * @param slot targeted slot
 * @return 0 on success
 */
int EffectUnit::unlinkEffectUnitSlot(std::string unit_name, sfx::hex_t ccc, sfx::hex_t ccs, std::string slot)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    return UnitsTable[unit_name]->unlinkSlot(std::make_pair(ccc, ccs), slot);
}


////////////////////////////////////////////////////////////////////
// Connection Managment
////////////////////////////////////////////////////////////////////

EffectUnit::ConnectionList EffectUnit::ConnectionGraph = EffectUnit::ConnectionList();

const EffectUnit::ConnectionList& EffectUnit::getConnectionGraph()
{
    return EffectUnit::ConnectionGraph;
}

void EffectUnit::logConnectionGraph()
{
    sfx::log(NAME, "Loaded Units Table : \n");
    sfx::sfxStream
            << "-----------------------------------------++-----------------------------------------\n"
            << "                  Source                 ||                  Target                 \n"
            << "--------------------+--------------------++--------------------+--------------------\n"
            << sfx::formatString("%-20s|%-20s||%-20s|%-20s\n"
            , std::string("Unit"), std::string("Port"), std::string("Unit"), std::string("Port"))
            << "====================|====================||====================|====================\n";

    for (auto& connection : ConnectionGraph)
    {
        sfx::sfxStream << sfx::formatString("%-20s|%-20s||%-20s|%-20s\n",
                connection.first.first, connection.first.second,
                connection.second.first, connection.second.second);
    }
    sfx::sfxStream
            << "--------------------+--------------------++--------------------+--------------------\n";
}

////////////////////////////////////////////////////////////////////

int EffectUnit::connect(std::string s_unit, std::string s_port, std::string t_unit, std::string t_port)
{
    // First find the EffectUnits
    if (UnitsTable.find(s_unit) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", s_unit);
        return 1;
    }
    if (UnitsTable.find(t_unit) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", t_unit);
        return 1;
    }

    // Verify that the ports exist
    if (UnitsTable[s_unit]->portsReg.find(s_port) == UnitsTable[s_unit]->portsReg.end())
    {
        sfx::wrn(NAME, "Port : \"%s\" Doesn't Exist\n", s_port);
        return 1;
    }
    if (UnitsTable[t_unit]->portsReg.find(t_port) == UnitsTable[t_unit]->portsReg.end())
    {
        sfx::wrn(NAME, "Port : \"%s\" Doesn't Exist\n", t_port);
        return 1;
    }

    std::string source = sfx::formatString("%s:%s", s_unit, s_port);
    std::string target = sfx::formatString("%s:%s", t_unit, t_port);

    // Verify if connection doesn't exist
    if (ConnectionGraph.find(std::make_pair(std::make_pair(s_unit, s_port), std::make_pair(t_unit, t_port))) != ConnectionGraph.end()
            && ConnectionGraph.find(std::make_pair(std::make_pair(t_unit, t_port), std::make_pair(s_unit, s_port))) != ConnectionGraph.end())
    {
        sfx::wrn(NAME, "Connection : \"%s\" => \"%s\" Alsready Exist\n", source, target);
        return 1;
    }

    // Make the connection
#ifdef __ARCH_LINUX__
    if (int res = jack_connect(UnitsTable[s_unit]->client, source.c_str(), target.c_str()))
    {
        sfx::wrn(NAME, "Unable to Make Connection : Err#%i", res);
        return 1;
    }
#else
    sfx::wrn(NAME, "NoJackRun : Cannot verify that connection is valid\n");
#endif

    ConnectionGraph.emplace(std::make_pair(s_unit, s_port), std::make_pair(t_unit, t_port));
    return 0;
}

////////////////////////////////////////////////////////////////////

int EffectUnit::connect(std::string unit_name, std::string source, std::string target)
{
    // First find the EffectUnits
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    // Verify if connection doesn't exist
    if (ConnectionGraph.find(std::make_pair(std::make_pair(unit_name, source), std::make_pair(unit_name, target))) != ConnectionGraph.end()
            && ConnectionGraph.find(std::make_pair(std::make_pair(unit_name, target), std::make_pair(unit_name, source))) != ConnectionGraph.end())
    {
        sfx::wrn(NAME, "Connection : \"%s\" => \"%s\" Alsready Exist\n", source, target);
        return 1;
    }

    // Make the connection
#ifdef __ARCH_LINUX__
    if (int res = jack_connect(UnitsTable[unit_name]->client, source.c_str(), target.c_str()))
    {
        sfx::wrn(NAME, "Unable to Make Connection : Err#%i", res);
        return 1;
    }
#else
    sfx::wrn(NAME, "NoJackRun : Cannot verify that connection is valid\n");
#endif

    ConnectionGraph.emplace(std::make_pair(unit_name, source), std::make_pair(unit_name, target));
    return 0;
}

////////////////////////////////////////////////////////////////////

int EffectUnit::disconnect(std::string s_unit, std::string s_port, std::string t_unit, std::string t_port)
{
    // First find the EffectUnits
    if (UnitsTable.find(s_unit) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", s_unit);
        return 1;
    }
    if (UnitsTable.find(t_unit) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", t_unit);
        return 1;
    }

    // Verify that the ports exist
    if (UnitsTable[s_unit]->portsReg.find(s_port) == UnitsTable[s_unit]->portsReg.end())
    {
        sfx::wrn(NAME, "Port : \"%s\" Doesn't Exist\n", s_port);
        return 1;
    }
    if (UnitsTable[t_unit]->portsReg.find(t_port) == UnitsTable[t_unit]->portsReg.end())
    {
        sfx::wrn(NAME, "Port : \"%s\" Doesn't Exist\n", t_port);
        return 1;
    }

    std::string source = sfx::formatString("%s:%s", s_unit, s_port);
    std::string target = sfx::formatString("%s:%s", t_unit, t_port);

    // Verify if connection exist
    if (ConnectionGraph.find(std::make_pair(std::make_pair(s_unit, s_port), std::make_pair(t_unit, t_port))) == ConnectionGraph.end()
            && ConnectionGraph.find(std::make_pair(std::make_pair(t_unit, t_port), std::make_pair(s_unit, s_port))) == ConnectionGraph.end())
    {
        sfx::wrn(NAME, "Connection : \"%s\" => \"%s\" Doesn't Exist\n", source, target);
        return 1;
    }

    // Make the connection
#ifdef __ARCH_LINUX__
    if (int res = jack_disconnect(UnitsTable[s_unit]->client, source.c_str(), target.c_str()))
    {
        sfx::wrn(NAME, "Unable to Remove Connection : Err#%i", res);
        return 1;
    }
#endif

    ConnectionGraph.erase(std::make_pair(std::make_pair(s_unit, s_port), std::make_pair(t_unit, t_port)));
    return 0;
}

////////////////////////////////////////////////////////////////////

int EffectUnit::disconnect(std::string unit_name, std::string source, std::string target)
{
    // First find the EffectUnits
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    // Verify if connection doesn't exist
    if (ConnectionGraph.find(std::make_pair(std::make_pair(unit_name, source), std::make_pair(unit_name, target))) == ConnectionGraph.end()
            && ConnectionGraph.find(std::make_pair(std::make_pair(unit_name, target), std::make_pair(unit_name, source))) == ConnectionGraph.end())
    {
        sfx::wrn(NAME, "Connection : \"%s\" => \"%s\" Doesn't Exist\n", source, target);
        return 1;
    }

    // Make the connection
#ifdef __ARCH_LINUX__
    if (int res = jack_disconnect(UnitsTable[unit_name]->client, source.c_str(), target.c_str()))
    {
        sfx::wrn(NAME, "Unable to Remove Connection : Err#%i", res);
        return 1;
    }
#endif

    ConnectionGraph.erase(std::make_pair(std::make_pair(unit_name, source), std::make_pair(unit_name, target)));
    return 0;
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////