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
                ((i < audioIns .size()) ? audioIns [i].name : std::string("")),
                ((i < audioOuts.size()) ? audioOuts [i].name : std::string("")),
                ((i < midiIns .size()) ? midiIns [i].name : std::string("")),
                ((i < midiOuts .size()) ? midiOuts [i].name : std::string("")));
    }
    sfx::sfxStream
            << "---------------------+----------------------++----------------------+---------------------\n";

    // Prints Unit's Ports Reg
    sfx::sfxStream
            << "---------------------\n";
    for (auto& port : portsReg)
        sfx::sfxStream << port << "\n";
    sfx::sfxStream
            << "---------------------\n";

    std::string h1 = sfx::formatString("%-20s", std::string("Bank ID"));
    std::string h2 = "--------------------";
    std::string h3 = sfx::formatString("%-20s", std::string("Slot Name"));
    std::string h4 = "====================";

    for (BankTable::iterator itr = banks.begin();
            itr != banks.end();
            ++itr)
    {
        h1 += sfx::formatString(" ||   %3i       ", itr->first);
        h2 += "-++-----+---------";
        h3 += " ||Midi |  Valeur ";
        h4 += "=||=====|=========";
    }
    sfx::sfxStream << h2 << "\n" << h1 << "\n" << h2 << "\n" << h3 << "\n" << h4 << "\n";
    for (auto& slot : module.lock()->getSlotTable())
    {
        sfx::sfxStream << sfx::formatString("%-20s", slot.first);

        for (BankTable::iterator itr = banks.begin();
                itr != banks.end();
                ++itr)
        {
            if (itr->first == *current_bank)
            {
                sfx::hex_t x_val;
                float f_val;
                this->getSlotValue(slot.first, &x_val, &f_val);

                sfx::sfxStream
                        << sfx::formatString(" || %3i | %1.2e", x_val, f_val);
            }
            else
            {
                sfx::sfxStream
                        << sfx::formatString(" || %3i |  xxx ", itr->second[slot.first]);
            }
        }

        sfx::sfxStream << "\n";
    }
    sfx::sfxStream << h2 << "\n";

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
 * Fonction utilisée pour déconnecter tout les slots des CC midis
 */
void EffectUnit::clearLinks()
{
    sfx::debug(unique_name, "Clear Links Table : %u => ", links.size());
    links.clear();
    sfx::sfxStream << links.size() << "\n";
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
    sfx::debug(NAME, "setSlotValue %s %x %i\n", slot, value, force);
    if (slots.find(slot) == slots.end()) return MISSING_SLOT;

    //sfx::debug(NAME, "1 Called slot : \"%s\" : Status : %i\n", slot, slots[slot].status);

    if (!!(slots[slot])) // If slot is enabled
    {
        float v = (*slots[slot].slot->callback)(value, this);
        slots[slot].value = value;
        banks[*current_bank][slot] = value;
        sfx::debug(NAME, "Called slot : \"%s\" with value : %i => %f\n", slot, value, v);
    }
    else if (force) // Else if slot is disabled, save value anyway
    {
        slots[slot].value = value;
        banks[*current_bank][slot] = value;
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

#ifdef __SFX_PI__
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
 * @brief Methode permettant de changer la banque courrante de l'Unit
 * @param id id de la banque à activer
 * @return 0 on success, -1 si la banque n'existe pas
 **/
int EffectUnit::setBank(sfx::hex_t id)
{
    sfx::debug(NAME, "setBank\n");
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
 * @brief Methode permettant de passer à la banque suivant de l'Unit
 *  selon l'ordre dans la liste @see banks_order
 **/
void EffectUnit::nextBank()
{
    sfx::debug(NAME, "nextBank\n");
    BankIdList::iterator nitr(current_bank);
    ++nitr;
    if (nitr == banks_order.end())
        nitr = banks_order.begin();

    // Update all params
    current_bank = nitr;
    this->updateCurrentBank();
}

/**
 * @brief Methode permettant de passer à la banque précédente de l'Unit
 *  selon l'odre dans la liste @see banks_order
 **/
void EffectUnit::prevBank()
{
    sfx::debug(NAME, "prevBank\n");
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
    sfx::debug(NAME, "updateCurrentBank\n");
    for (auto& slot : banks[*current_bank])
    {
        sfx::debug(NAME, "updateCurrentBank : %s\n", slot.first);

        if (slots[slot.first].slot->type == Module::Slot::VALUE)
            this->setSlotValue(slot.first, slot.second, true);
    }
}

/**
 * @brief Methode permettant de creer et ajouter une banque à l'Unit
 *  attribut un ID automatiquement
 */
int EffectUnit::createBank()
{
    return this->createBank(bank_id_manager.reserveUID());
}
/**
 * @brief Methode permettant de creer et ajouter une banque à l'Unit
 * @param id ID de la banque à creer
 * @return 0 on success, EXISTING_BANK si la banque existe déjà
 **/
int EffectUnit::createBank(sfx::hex_t id)
{
    // If bank doesn't exist create it
    if (banks.find(id) == banks.end())
    {

        ParamArray array;
        for (auto& slot : slots)
            array[slot.first] = slot.second.slot->default_value;

        banks[id] = array;
        banks_order.emplace_back(id);
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
int EffectUnit::copyBank(sfx::hex_t sid)
{
    return this->copyBank(sid, bank_id_manager.reserveUID());
}

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
    sfx::debug(NAME, "removeBank\n");
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

#ifdef __SFX_PI__
    // Construction du client Jack
    client = sfx::openJackClient(module_ptr->getInformations().unique_name);
    jack_set_process_callback(client, module_ptr->getProcessCallback(), this);

    unique_name = jack_get_client_name(client);
    module_ptr->addRelatedUnit(unique_name);
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
    sfx::debug(NAME, "Destroy Effect Unit : %s \n", unique_name);
#ifdef __SFX_PI__
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

/**
 * Methode appelée après la creation de l'effet
 *  Appelle tout les slots de type POST_INIT du module
 */
void EffectUnit::postInit()
{
    for (auto& slot : slots)
    {
        if (slot.second.slot->type == Module::Slot::POST_INIT)
        {
            this->setSlotValue(slot.first, 0);
            this->disableSlot(slot.first);
        }
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

std::weak_ptr<EffectUnit> EffectUnit::getEffectUnit(const std::string& unit_name)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return std::weak_ptr<EffectUnit>();
    }
    return UnitsTable[unit_name];
}

void EffectUnit::logEffectUnitsTable()
{
    sfx::log(NAME, "Loaded Units Table : \n");
    sfx::sfxStream
            << "------------------------------+--------------------+--------------------\n"
            << sfx::formatString("%-30s|%-20s|%-20s\n"
            , std::string("Nom Interne"), std::string("Nom d'affichage"), std::string("Module"))
            << "==============================|====================|====================\n";

    for (auto& unit : UnitsTable)
    {
        sfx::sfxStream << sfx::formatString("%-30s|%-20s|%-20s\n"
                , unit.first, unit.second->display_name
                , unit.second->module.lock()->getInformations().unique_name);
    }
    sfx::sfxStream << "------------------------------+--------------------+--------------------\n";
}
/*
void EffectUnit::logEffectUnit(std::string unit_name)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
    }
    else UnitsTable[unit_name]->logCompleteInfos();
}
//*/

/**
 * Fonction pour creer une Unité de Traitement à partir d'un module
 *  l'unité générée sera celle avec les paramètres par défault
 * @param module_name nom interne du module à utiliser
 * @param unique_name pointeur vers une chaine de caractère où sera stoqué le nom unique de l'effet crée
 * @return 0 on success
 */
int EffectUnit::buildEffectUnitFromModule(const std::string& module_name, std::string* unique_name)
{
    sfx::debug(NAME, "Build effect From Module : \"%s\" ... ", module_name);

    try
    {
        // First find the proper Module
        if (Module::getModuleTable().find(module_name) == Module::getModuleTable().end())
            throw std::runtime_error("Module_Doesnt_Exist");

        // Verify that module allows to build a new unit
        if (auto module_ptr = Module::getModule(module_name).lock())
        {
            if (module_ptr->getInformations().type == Module::ShortInfo::SYSTEM)
            {
                size_t c = module_ptr->relatedUnitsCount();
                sfx::debug(NAME, "System_unit Childs : %u\n", c);
                if (c != 0)
                    throw std::runtime_error("System_Unit_Already_Built");
            }
        }
        else throw std::runtime_error("Module_Expired");

        // Then create an Effect Unit from the Module
        std::shared_ptr<EffectUnit> unit = std::make_shared<EffectUnit>(Module::getModule(module_name));

        // Fill the Effect With default Values
        unit->createBank();
        unit->current_bank = unit->banks_order.begin();
        unit->updateCurrentBank();

        //unit->current_links_file = sfx::formatString("%s_links_%i", unit->unique_name, std::rand());
        //unit->current_banks_file = sfx::formatString("%s_banks_%i", unit->unique_name, std::rand());

        // Finaly activate the Effect Unit
#ifdef __SFX_PI__
        unit->activateJackClient();
#endif
        // Run the POST_INIT Routine
        unit->postInit();

        // Then add It to the Units Table
        if (UnitsTable.find(unit->unique_name) != UnitsTable.end())
            throw std::runtime_error("Unique_Name_Duplication");

        UnitsTable[unit->unique_name] = unit;
        if (unique_name) *unique_name = unit->unique_name;

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
 * @param unique_name pointeur vers une chaine de caractère où sera stoqué le nom unique de l'effet crée
 * @return 0 on success
 */
int EffectUnit::buildEffectUnitFromPresetFile(const std::string& file_path, std::string* unique_name)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load effect from file : \"%s\" ... \n", file_path);

    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");
            
        /////////////////////////////////////////////////////
        // Verification de l'integrité du Fichier
        /////////////////////////////////////////////////////

        // Read file header
        sfx::serial::controlValue(flux, sfx::serial::EffectUnit_Preset_File, "Invalid_File_Flag");

        Module::ShortInfo infos = Module::deserialize_infos(flux);

        // Verify if corresponding module has been loaded
        if (Module::getModuleTable().find(infos.unique_name) == Module::getModuleTable().end())
            throw std::ios_base::failure("Module_Type_Invalid");

        std::shared_ptr<Module> module_ptr;
        if (!(module_ptr = Module::getModule(infos.unique_name).lock()))
            throw std::runtime_error("Module_Expired");

        // Verify module version
        if (infos.version != module_ptr->getInformations().version)
            sfx::wrn(NAME, "Module versions differs : Preset:%s LoadedModule:%s",
                infos.version, module_ptr->getInformations().version);

        ;

        // Verify that module allows to build a new unit
        if (module_ptr->getInformations().type == Module::ShortInfo::SYSTEM
                && module_ptr->relatedUnitsCount() != 0)
        {
            sfx::wrn(NAME, "System Unit Already Built\n");
            if (unique_name) *unique_name = module_ptr->getInformations().unique_name;
            return 0;
        }
        
        /////////////////////////////////////////////////////
        // Construction de l'Unit
        /////////////////////////////////////////////////////

        // Then create an Effect Unit from the Module
        std::shared_ptr<EffectUnit> unit = std::make_shared<EffectUnit>(module_ptr);

        // Read Effect Name
        std::string name = sfx::serial::read<std::string>(flux);
        unit->display_name = name;

        // Read banks location
        std::string banks_file = sfx::serial::read<std::string>(flux);
        
        // Read links location
        std::string links_file = sfx::serial::read<std::string>(flux);

        // Read bank file
        if (load_BankFile(banks_file, unit))
            throw std::ios_base::failure("Failed read bank file");

        // Read links file
        if (load_LinkFile(links_file, unit))
            throw std::ios_base::failure("Failed read links file");

        // Close the file
        flux.close();

        // Finaly activate the Effect Unit
#ifdef __SFX_PI__
        unit->activateJackClient();
#endif

        /////////////////////////////////////////////////////
        // POST INIT
        /////////////////////////////////////////////////////
        
        // Run the POST_INIT Routine
        unit->postInit();

        // Then add It to the Units Table
        if (UnitsTable.find(unit->unique_name) != UnitsTable.end())
            throw std::runtime_error("Unique_Name_Duplication");

        UnitsTable[unit->unique_name] = unit;
        if (unique_name) *unique_name = unit->unique_name;

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

    // Verify that unit is not a System Unit
    if (auto module_ptr = UnitsTable[unit_name]->module.lock())
    {
        if (module_ptr->getInformations().type == Module::ShortInfo::SYSTEM)
        {
            sfx::wrn(NAME, "Destroy System Unit is Not Allowed\n");
            return 1;
        }
    }
    else
    {
        sfx::wrn(NAME, "Module Expired\n");
        return 1;
    }

    for (ConnectionList::iterator citr = ConnectionGraph.begin();
            citr != ConnectionGraph.end();
            ++citr)
    {
        if (citr->first.first == unit_name || citr->second.first == unit_name)
        {
            citr = ConnectionGraph.erase(citr);
        }
    }

    UnitsTable.erase(unit_name);
    return 0;
}

/**
 * Fonction pour detruire une unité
 * @param unit_name nom interne de l'unité à detruire
 * @return 0 on success
 */
int EffectUnit::forceDestroyEffectUnit(std::string unit_name)
{
    // First find the EffectUnit
    if (UnitsTable.find(unit_name) == UnitsTable.end())
    {
        sfx::wrn(NAME, "Effect Unit : \"%s\" Doesn't Exist\n", unit_name);
        return 1;
    }

    for (ConnectionList::iterator citr = ConnectionGraph.begin();
            citr != ConnectionGraph.end();
            ++citr)
    {
        if (citr->first.first == unit_name || citr->second.first == unit_name)
        {
            citr = ConnectionGraph.erase(citr);
        }
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

    if (bank_file == "") bank_file = sfx::formatString("%s_banks", file);//UnitsTable[unit_name]->current_banks_file;
    if (link_file == "") link_file = sfx::formatString("%s_links", file);//UnitsTable[unit_name]->current_links_file;

    return save_PresetFile(file, UnitsTable[unit_name], bank_file, link_file);
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
            << "---------------------------------------------------++---------------------------------------------------\n"
            << "                       Source                      ||                       Target                      \n"
            << "------------------------------+--------------------++------------------------------+--------------------\n"
            << sfx::formatString("%-30s|%-20s||%-30s|%-20s\n"
            , std::string("Unit"), std::string("Port"), std::string("Unit"), std::string("Port"))
            << "==============================|====================||==============================|====================\n";

    for (auto& connection : ConnectionGraph)
    {
        sfx::sfxStream << sfx::formatString("%-30s|%-20s||%-30s|%-20s\n",
                connection.first.first, connection.first.second,
                connection.second.first, connection.second.second);
    }
    sfx::sfxStream
            << "------------------------------+--------------------++------------------------------+--------------------\n";
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
#ifdef __SFX_PI__
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
#ifdef __SFX_PI__
    if (int res = jack_connect(UnitsTable[unit_name]->client, source.c_str(), target.c_str()))
    {
        sfx::wrn(NAME, "Unable to Make Connection : Err#%i", res);
        return 1;
    }
#else
    sfx::wrn(NAME, "NoJackRun : Cannot verify if connection is valid\n");
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
#ifdef __SFX_PI__
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
#ifdef __SFX_PI__
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
// Graph Serial
////////////////////////////////////////////////////////////////////

/**
 * flag_t : Environement_Preset_File
 * usize_t : Units Count
 * {
 *      id1_t : uid
 *      string Units files path
 * }
 * usize_t : Connection Count
 * {
 *      Connections (<<source_uid,source_port>,<target_uid,target_port>>)
 * }
 */
int EffectUnit::save_EnvironementFile(std::string file_path)
{
    std::ofstream flux;
    sfx::debug(NAME, "Save Environement to file : \"%s\"\n", file_path);

    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");

        // Write file header
        sfx::serial::write<sfx::flag_t> (flux, sfx::serial::Environement_Preset_File);

        // Generate Serial Units names Table
        NameToSerialTable idtable = effectUnitTable_to_serial();

        // Write Units count
        sfx::serial::write<sfx::usize_t> (flux, UnitsTable.size());

        // Write all Units
        for (auto& unit : UnitsTable)
        {
            sfx::serial::write<sfx::id1_t> (flux, idtable[unit.first]);
            std::string unit_file = sfx::formatString("%s_unit_%i", file_path, idtable[unit.first]);
            sfx::serial::write<std::string>(flux, unit_file);
            saveEffectUnit(unit.first, unit_file);
        }

        // Write Connections count
        sfx::serial::write<sfx::usize_t> (flux, ConnectionGraph.size());

        // Write all Connections
        for (auto& connection : ConnectionGraph)
        {
            serialiaze_Connection(flux, connection_to_serial(idtable, connection));
        }

        // Close the file
        flux.close();
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Writing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    return 0;
}

int EffectUnit::load_EnvironementFile(std::string file_path)
{
    std::ifstream flux;
    sfx::debug(NAME, "Load banks from file : \"%s\"\n", file_path);

    try
    {
        // Open the file
        flux.open(file_path.c_str(),
                std::ifstream::in | std::ifstream::binary);
        if (!flux.is_open())
            throw std::ios_base::failure("Failed open file");

        // Read file header
        sfx::serial::controlValue(flux, sfx::serial::Environement_Preset_File, "Invalid_File_Flag");

        // Read units and build table
        SerialToNameTable name_table;
        sfx::usize_t unit_count = sfx::serial::read<sfx::usize_t> (flux);

        // Read each Effects
        for (sfx::usize_t i = 0; i < unit_count; ++i)
        {
            std::string created_unit_name("");
            sfx::id1_t unit_id = sfx::serial::read<sfx::id1_t> (flux);
            std::string unit_file = sfx::serial::read<std::string>(flux);

            if (buildEffectUnitFromPresetFile(unit_file, &created_unit_name))
                throw std::runtime_error("Failed_Load_Effect");

            name_table[unit_id] = created_unit_name;
        }

        sfx::debug(NAME, "Serial to Unit Table\n");
        sfx::sfxStream << ("-----|--------------------------------\n");
        for (auto& unit : name_table)
            sfx::sfxStream << sfx::formatString(" %03i | %-30s \n", unit.first, unit.second);
        sfx::sfxStream << ("-----|--------------------------------\n");

        sfx::debug(NAME, "Load Connection Graph\n");
        // Read Each Connections
        sfx::usize_t graph_size = sfx::serial::read<sfx::usize_t> (flux);

        // Read each Effects
        for (sfx::usize_t i = 0; i < graph_size; ++i)
        {
            SerialConnection parsed_c = deserialize_Connection(flux);
            // Convert connection
            Connection c = connection_from_serial(name_table, parsed_c);

            if (connect(c.first.first, c.first.second, c.second.first, c.second.second))
                sfx::wrn(NAME, "Failed add Connection : %s %s => %s %s\n",
                    c.first.first, c.first.second, c.second.first, c.second.second);
        }

        // Close the file
        flux.close();
        sfx::debug(NAME, "Environement Loading Successfull !!\n");
    }
    catch (std::ios_base::failure const& e)
    {
        sfx::err(NAME, "Error While Parsing File : Error#%i : %s\n", e.code(), e.what());
        return 1;
    }
    catch (std::runtime_error const& e)
    {
        sfx::err(NAME, "Error Broken File : %s\n", e.what());
        return 1;
    }
    return 0;
}

/**
 * id1_t : Source uid
 * string : Source Port Name
 * id1_t : Target uid
 * string : Target Port Name
 */
void EffectUnit::serialiaze_Connection(std::ofstream& flux, const EffectUnit::SerialConnection& obj)
{
    sfx::serial::write<sfx::id1_t> (flux, obj.first.first);
    sfx::serial::write<std::string> (flux, obj.first.second);

    sfx::serial::write<sfx::id1_t> (flux, obj.second.first);
    sfx::serial::write<std::string> (flux, obj.second.second);
}

EffectUnit::SerialConnection EffectUnit::deserialize_Connection(std::ifstream& flux)
{
    sfx::id1_t sid = sfx::serial::read<sfx::id1_t> (flux);
    std::string sp = sfx::serial::read<std::string> (flux);

    sfx::id1_t tid = sfx::serial::read<sfx::id1_t> (flux);
    std::string tp = sfx::serial::read<std::string> (flux);

    return std::make_pair(std::make_pair(sid, sp),
            std::make_pair(tid, tp));
}

EffectUnit::NameToSerialTable EffectUnit::effectUnitTable_to_serial()
{
    NameToSerialTable table;
    UIDManager<sfx::id1_t> uidgen(0);
    for (auto& unit : UnitsTable)
    {
        table[unit.first] = uidgen.reserveUID();
    }
    return table;
}

EffectUnit::SerialConnection EffectUnit::connection_to_serial(NameToSerialTable table, const Connection& obj)
{
    assert(table.find(obj.first.first) != table.end());
    assert(table.find(obj.second.first) != table.end());

    sfx::id1_t sid = table[obj.first.first], tid = table[obj.second.first];

    return std::make_pair(std::make_pair(sid, obj.first.second),
            std::make_pair(tid, obj.second.second));
}

EffectUnit::Connection EffectUnit::connection_from_serial(SerialToNameTable table, const SerialConnection& obj)
{
    if (table.find(obj.first.first) == table.end())
        throw std::runtime_error(sfx::formatString("Source_UID#%i_Not_Found", obj.first.first));
    if (table.find(obj.second.first) == table.end())
        throw std::runtime_error(sfx::formatString("Target_UID#%i_Not_Found", obj.second.first));

    std::string sname = table[obj.first.first], tname = table[obj.second.first];

    return std::make_pair(std::make_pair(sname, obj.first.second),
            std::make_pair(tname, obj.second.second));
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
