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
 * File:   ModuleBase.h
 * Author: Space-Computer
 *
 * Created on 30 août 2018, 20:22
 */

#ifndef MODULEBASE_H
#define MODULEBASE_H

#include <map>
#include <list>
#include <vector>
#include <memory>
#include <functional>

#include <iostream>
#include <fstream>

#include "noyau/types.h"
#include "noyau/utils.h"
#include "noyau/midi.h"
#include "noyau/id.h"
#include "noyau/serial.h"

////////////////////////////////////////////////////////////////////
// Chargement Librairies dynamiques
////////////////////////////////////////////////////////////////////
#ifdef __UNIX__
#include <dlfcn.h>
#elif defined(__WINDOWS__)
#endif

////////////////////////////////////////////////////////////////////
// Environement avec processing (SFX-Pi)
////////////////////////////////////////////////////////////////////
#ifdef __ARCH_LINUX__
#include "process/module/jackWrapper.h"
#endif

struct EffectUnit;

/**
 * Objet représentant un type d'effet, contiens les informations relatives
 *  et les fonctions permettant d'initialiser un EffectUnit correspondant
 */
class Module
{
    ////////////////////////////////////////////////////////////////////
    // Informations sur le Module
    ////////////////////////////////////////////////////////////////////

public:

    /**
     * Structure contenant les informations sur le Module
     */
    struct ShortInfo
    {

        ShortInfo(std::string i, std::string n, std::string v) :
        unique_name(i), name(n), version(v)
        {
        }

        std::string unique_name;
        std::string name;
        std::string version;
    };

    typedef ShortInfo(*register_module_info_f)(void);

public:

    /**
     * Methode renvoyant les informations sur le module
     */
    inline const ShortInfo& getInformations() const
    {
        return infos;
    }

private:

    ShortInfo infos; /**< Module's Informations */

    ////////////////////////////////////////////////////////////////////
    // Gestion des slots
    ////////////////////////////////////////////////////////////////////

public:

    /**
     * Structure utilisée pour representer une possible connection
     *  entre un parametre et une valeur midi
     */
    struct Slot
    {
        /**
         * Fonction appelée pour modifier la valeur controlée par le slot
         * @param sfx::hex_t Valeur à attribuer (valeur 0-127), ou >= 128 pour demander la valeur courante
         * @param void* Pointeur vers le bloc de paramètres du module
         * @return la valeur nouvelle valeur du slot;
         */
        typedef float (*slot_f)(sfx::hex_t, EffectUnit*);

        Slot(std::string uname, slot_f f, std::string dname, sfx::hex_t d) :
        internal_name(uname),
        callback(f),
        display_name(dname),
        default_value(d)
        {

        }

        std::string internal_name;
        slot_f callback;

        std::string display_name;
        sfx::hex_t default_value;
    };
    /**
     * Table stoquant les Slots par nom
     */
    typedef std::map<std::string, std::shared_ptr<Slot>> SlotTable;

    /**
     * Prototype de la fonction de déclaration des Slots
     */
    typedef SlotTable(*register_slot_table_f)(void);

public:

    /**
     * Methode renvoyant la table des slots du module
     */
    inline const SlotTable& getSlotTable() const
    {
        return slots;
    }

    /**
     * Fonction utilisée pour enregistrer un slot
     * @param table la table où ajouter le slot
     * @param internal_name nom interne du slot
     * @param display_name nom d'affichage du slot
     * @param callback fonction appekée pour modifier la valeur controlée par le slot
     */
    static inline void register_slot(SlotTable& table, sfx::hex_t def_val
            , std::string internal_name, std::string display_name, Slot::slot_f callback)
    {
        table[internal_name] = std::make_shared<Slot>(
                internal_name,
                callback,
                display_name,
                def_val
                );
    }

private:

    SlotTable slots; /**< Table of Module's slots */

    ////////////////////////////////////////////////////////////////////
    // Fonctions de creation et destruction des datas des EffectUnits
    ////////////////////////////////////////////////////////////////////

public:

    /**
     * Proptotype de la fonction pour creer les datas d'un Module
     */
    typedef void* (*create_module_data_f)(EffectUnit*);
    typedef void (*destroy_module_data_f)(void*);

public:

    /**
     * Fonction pour construire les datas d'un EffectUnits liée au module
     */
    inline void* buildEffectUnitDatas(EffectUnit* unit)
    {
        return (*builder)(unit);
    }

    /**
     * Fonction pour detruire les datas d'un EffectUnit liée au module
     */
    inline void destroyEffectUnitDatas(void* datas)
    {
        return (*destructor)(datas);
    }

private:

    create_module_data_f builder; /**< Pointeur vers la fonction de build des datas */
    destroy_module_data_f destructor; /**< Poitneur vers la fonction de destruction des datas */

    ////////////////////////////////////////////////////////////////////
    // Fonctions specifiques aux Modules sous SFX-Pi
    ////////////////////////////////////////////////////////////////////

#ifdef __ARCH_LINUX__

public:

    inline JackProcessCallback getProcessCallback()
    {
        return callback;
    }

private:

    JackProcessCallback callback;

#endif

    ////////////////////////////////////////////////////////////////////
    // Gestion de la librairie dynamique
    ////////////////////////////////////////////////////////////////////

private:

    void* lib_handle;

    ////////////////////////////////////////////////////////////////////
    // Gestion des EffectUnits
    ////////////////////////////////////////////////////////////////////

public:

    int addRelatedUnit(std::string unit_unique_name);
    void removeRelatedUnit(std::string unit_unique_name);

private:

    typedef std::set<std::string> RelatedUnitsList;
    RelatedUnitsList relatedUnits;

    ////////////////////////////////////////////////////////////////////
    // Constructeur et Destructeur
    ////////////////////////////////////////////////////////////////////

public:

    Module(const ShortInfo& infos,
            const SlotTable& slots,

            const create_module_data_f& builder,
            const destroy_module_data_f& destructor,
#ifdef __ARCH_LINUX__
            const JackProcessCallback& callback,
#endif
            void* lib_handle);
    ~Module();

    ////////////////////////////////////////////////////////////////////
    // Fonctions d'info
    ////////////////////////////////////////////////////////////////////

    void logModuleCompleteInfos() const;

    ////////////////////////////////////////////////////////////////////
    // ModuleLoading Structures
    ////////////////////////////////////////////////////////////////////

public:

    typedef std::map<std::string, std::shared_ptr<Module>> LoadedModulesTable;

    static const LoadedModulesTable& getModuleTable();

    static std::weak_ptr<Module> getModule(const std::string& unique_name);

    /**
     * @brief Fonction à appeler au lancement du programme pour charger les modules du fichier
     *  de configuration donné
     * @param config_path chemin du fichier de configuration
     * @return 0 on success
     */
    static int loadModuleTable(std::string config_path);
    /**
     * Fonction à appeler à la fermeture du programme pour libérer les modules chargés
     */
    static void unloadModuleTable();

    /**
     * Fonction pour afficher la liste des modules chargés
     */
    static void logLoadedModuleTable();

    /**
     * Fonction à appeller pour charger un module depuis un fichier de librairie dynamique
     * @param path chemin du fichier librairie
     * @return 0 on success
     */
    static int loadModule(std::string path);
    static int unloadModule(std::string unique_name);

private:

    static LoadedModulesTable ModuleTable;

private:

    /**
     * @brief Fonction de wrapping pour charger une fonction depuis une librairie dynamique
     * @param name Nom de la fonction à charger
     * @return pointeur vers la fonction chargée
     * @throw ios_base::failure
     */
    static void* load_function(void* handle, std::string name);

    ////////////////////////////////////////////////////////////////////
    // Serialisation
    ////////////////////////////////////////////////////////////////////

public:

    /**
     *  string : nom unique
     *  string : version
     * @throw std::ios_base::failure if parsing failed
     */
    static void serialize_infos(std::ofstream& flux, const ShortInfo& obj);
    static ShortInfo deserialize_infos(std::ifstream& flux);
};

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

class EffectUnit
{
    ////////////////////////////////////////////////////////////////////
    // Informations sur l'Unité de Traitement
    ////////////////////////////////////////////////////////////////////

public:

    /**
     * @return Nom interne de l'effet
     */
    inline const std::string& getUniqueName() const
    {
        return unique_name;
    }

    inline const std::string& getDisplayName() const
    {
        return display_name;
    }

    inline void setDisplayName(const std::string& name)
    {
        display_name = name;
    }

    inline void* getDatas()
    {
        return datas;
    }
    
    void logCompleteInfos();

private:

    std::string unique_name; /**< Nom interne de l'effet */
    std::string display_name; /**< Nom d'affichage de l'effet */
    std::weak_ptr<Module> module; /**< Pointeur vers le module parent */

    void* datas; /**< Pointeur vers les datas du Module */

    ////////////////////////////////////////////////////////////////////
    // Attribut des ProcessUnit (EffectUnit sous SFX-Pi)
    ////////////////////////////////////////////////////////////////////

#ifdef __ARCH_LINUX__

public:

    jack_client_t* getClient()
    {
        return client;
    }

private:

    inline void activateJackClient()
    {
        sfx::activateClient(client);
    }

    jack_client_t* client;

#endif

    ////////////////////////////////////////////////////////////////////
    // Gestion des Ports
    ////////////////////////////////////////////////////////////////////

public:

    /**
     * Structure utilisée pour representer un Port
     */
    struct Port
    {
        std::string name;
#ifdef __ARCH_LINUX__
        jack_port_t* port;
#endif
    };
    /**
     * Liste des Ports d'un Effet
     */
    typedef std::vector<Port> PortArray;
    typedef std::set<std::string> PortReg;

public:

#ifdef __ARCH_LINUX__

    inline void registerAudioInput(std::string name)
    {
        this->audioIns.push_back(Port{name, sfx::registerAudioInput(client, name)});
    }

    inline void registerAudioOutput(std::string name)
    {
        this->audioOuts.push_back(Port{name, sfx::registerAudioOutput(client, name)});
    }

    inline void registerMidiInput(std::string name)
    {
        this->midiIns.push_back(Port{name, sfx::registerMidiInput(client, name)});
    }

    inline void registerMidiOutput(std::string name)
    {
        this->midiOuts.push_back(Port{name, sfx::registerMidiOutput(client, name)});
    }

#else

    inline void registerAudioInput(std::string name)
    {
        this->audioIns.push_back(Port{name});
    }

    inline void registerAudioOutput(std::string name)
    {
        this->audioOuts.push_back(Port{name});
    }

    inline void registerMidiInput(std::string name)
    {
        this->midiIns.push_back(Port{name});
    }

    inline void registerMidiOutput(std::string name)
    {
        this->midiOuts.push_back(Port{name});
    }

#endif

    inline const Port& getAudioInputs(size_t i) const
    {
        return audioIns[i];
    }

    inline const Port& getAudioOutputs(size_t i) const
    {
        return audioOuts[i];
    }

    inline const Port& getMidiInputs(size_t i) const
    {
        return midiIns[i];
    }

    inline const Port& getMidiOutputs(size_t i) const
    {
        return midiOuts[i];
    }

private:

    PortArray audioIns, audioOuts, midiIns, midiOuts;
    PortReg portsReg;

    ////////////////////////////////////////////////////////////////////
    // Gestion des Slots
    ////////////////////////////////////////////////////////////////////

public:

    static const int MISSING_SLOT = -1;
    static const int DISABLED_SLOT = 1;

    struct SlotStatus
    {
        std::shared_ptr<Module::Slot> slot;
        sfx::hex_t value;

        enum
        {
            ENABLED = 0, DISABLED
        } status;

        inline bool operator!() const
        {
            return status != ENABLED;
        };
    };

    typedef std::map<sfx::hex_pair_t, std::vector<std::string>> LinkTable;
    typedef std::map<std::string, SlotStatus> SlotStatusTable;

public:

    /**
     * Fonction utilisée pour connecter un slot à un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom interne du slot à connecter
     * @return 0 on success, MISSING_SLOT is slot doesn't exist
     */
    int linkSlot(sfx::hex_pair_t cc, std::string slot);
    /**
     * Fonction utilisée pour déconnecter un slot à d'un cc midi
     * @param cc paire <channel,source> du cc à connecter
     * @param slot nom interne du slot à connecter
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, 1 if link doesn't exist
     */
    int unlinkSlot(sfx::hex_pair_t cc, std::string slot);

    /**
     * Fonction utilisée pour activer un slot
     * @param slot le nom interne du slot à activer
     * @return 0 on success, MISSING_SLOT if slot doesn't exist
     */
    int enableSlot(std::string slot);
    /**
     * Fonction utilisée pour désactiver un slot
     * @param slot le nom interne du slot à désactiver
     * @return 0 on success, MISSING_SLOT if slot doesn't exist
     */
    int disableSlot(std::string slot);

    /**
     * Fonction utiliée pour changer la valeur d'un slot
     *  Si le slot est désactivé la fonction est sans effet
     * @param slot nom interne du slot à changer
     * @param value valleur à assigner au slot (sur 7bit)
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
     */
    int setSlotValue(std::string slot, sfx::hex_t value, bool force = false);
    /**
     * Fonction utiliée pour récuperer la valeur interne d'un slot
     *  Si le slot est désactivé la fonction est sans effet.
     *  laisser à nullptr les valeurs non souhaitées
     * @param slot nom interne du slot ciblé
     * @param x_result pointeur pour récuperer la valeur sur 7bit du slot
     * @param f_result pointeur pour récuperer la valeur interne du slot
     * @return 0 on success, MISSING_SLOT if slot doesn't exist, DISABLED_SLOT if slot is not active
     */
    int getSlotValue(std::string slot, sfx::hex_t* x_result, float* f_result);

    void logLinkedSlots() const;

#ifdef __ARCH_LINUX__
    /**
     * Fonction à appeller dans la boucle de process pour traiter les CC midi et 
     *  déclancher les slots eventuels
     * @param event pointeur vers le buffer de l'evenement midi 
     */
    void veryfyAndComputeCCMessage(sfx::hex_t* event);
#endif

private:

    LinkTable links; /**< Table des liens Slot <-> CC */
    SlotStatusTable slots; /**< Table des etats des differents slots du module */

    ////////////////////////////////////////////////////////////////////
    // Gestion des Banques
    ////////////////////////////////////////////////////////////////////

public:

    static const int MISSING_BANK = -1;
    static const int EXISTING_BANK = 1;

    typedef std::map<std::string, sfx::hex_t> ParamArray;
    typedef std::map<sfx::hex_t, ParamArray> BankTable;
    typedef std::list<sfx::hex_t> BankIdList;

public:

    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    int setBank(sfx::hex_t id);
    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    void nextBank();
    /**
     * @brief Change current bank
     * @pre current_bank bank is valid
     * @post current_bank bank is valid
     **/
    void prevBank();

    /**
     * Fonction interne utilisée pour mettre à jour l'effet en appelant tout les slots
     *  actifs de la banque courante
     * @pre current_bank bank is valid
     */
    void updateCurrentBank();

    /**
     * @brief method to add a bank to the effect
     *  Usage :
     *      createBank() to create a bank filled with default values
     *      createBank(id, size, vals) to create a bank from a valid array of values
     * @pre if specified, bank is a valid expansion of effect's config tree
     */
    int createBank();
    int createBank(sfx::hex_t id);
    /**
     * @brief method to duplicate a bank of the effect
     *  Usage :
     *      copyBank(sid) to copy bank[sid] to a new one
     *      copyBank(sid, tid) to copy bank[sid] in bank[tid]
     * @pre sid != tid
     */
    int copyBank(sfx::hex_t sid, sfx::hex_t tid);
    /**
     * @brief Method used to remove a bank from the effect
     *  if the bank removed is the last one, automaticaly regenerate the default bank
     * @pre _currentBank bank is valid
     * @post _currentBank bank is valid
     */
    int removeBank(sfx::hex_t id);

private:

    BankTable banks;
    BankIdList banks_order;

    BankIdList::iterator current_bank;

    UIDManager<sfx::hex_t> bank_id_manager;

    ////////////////////////////////////////////////////////////////////
    // Constructeur / Destructeur
    ////////////////////////////////////////////////////////////////////

public:

    EffectUnit(std::weak_ptr<Module> module_weak);
    ~EffectUnit();

    ////////////////////////////////////////////////////////////////////
    // EffectUnit Handling
    ////////////////////////////////////////////////////////////////////

public:

    typedef std::map<sfx::string, std::shared_ptr<EffectUnit>> EffectUnitsTable;

public:

    static const EffectUnitsTable& getEffectUnitsTable();
    
    static void logEffectUnitsTable();
    static void logEffectUnit(std::string unit_name);

    /**
     * Fonction pour creer une Unité de Traitement à partir d'un module
     *  l'unité générée sera celle avec les paramètres par défault
     * @param module_name nom interne du module à utiliser
     * @return 0 on success
     */
    static int buildEffectUnitFromModule(const std::string& module_name);
    /**
     * Fonction pour creer une Unité à partir d'un fichier Preset Sauvegardé
     *  précédement
     * @param file_path Chemin du fichier à interpreter
     * @return 0 on success
     */
    static int buildEffectUnitFromPresetFile(const std::string& file_path);
    /**
     * Fonction pour detruire une unité
     * @param unit_name nom interne de l'unité à detruire
     * @return 0 on success
     */
    static int destroyEffectUnit(std::string unit_name);
    /**
     * Fonction pour sauvegarder une unité
     * @param unit_name nom interne de l'unité à sauvegarder
     * @return 0 on success
     */
    static int saveEffectUnit(std::string unit_name, std::string file,
            std::string bank_file = "", std::string link_file = "");
    
    ////////////////////////////////////////////////////////////////////
    
    /**
     * Fonction pour sauvegarder les banques d'une unité dans un fichier
     * @param unit_name nom interne de l'unité
     * @param file fichier cible
     * @return 0 on success
     */
    static int saveEffectUnitBanks(std::string unit_name, std::string file);
    /**
     * Fonction pour Charger les banques d'une unité depuis un fichier
     * @param unit_name nom interne de l'unité
     * @param file fichier cible
     * @return 0 on success
     */
    static int saveEffectUnitLinks(std::string unit_name, std::string file);
    /**
     * Fonction pour sauvegarder les links Midi d'une unité dans un fichier
     * @param unit_name nom interne de l'unité
     * @param file fichier cible
     * @return 0 on success
     */
    static int loadEffectUnitBanks(std::string unit_name, std::string file);
    /**
     * Fonction pour Charger les links Midi d'une unité depuis un fichier
     * @param unit_name nom interne de l'unité
     * @param file fichier cible
     * @return 0 on success
     */
    static int loadEffectUnitLinks(std::string unit_name, std::string file);
    
    ////////////////////////////////////////////////////////////////////
    
    /**
     * Wrapper for the Unit's linkSlot method
     * @param unit_name internal name of targeted unit
     * @param cc midi cc source
     * @param slot targeted slot
     * @return 0 on success
     */
    static int linkEffectUnitSlot(std::string unit_name, sfx::hex_t ccc, sfx::hex_t ccs, std::string slot);
    /**
     * Wrapper for the Unit's unlinkSlot method
     * @param unit_name internal name of targeted unit
     * @param cc midi cc source
     * @param slot targeted slot
     * @return 0 on success
     */
    static int unlinkEffectUnitSlot(std::string unit_name, sfx::hex_t ccc, sfx::hex_t ccs, std::string slot);

private:

    static EffectUnitsTable UnitsTable;

    ////////////////////////////////////////////////////////////////////
    // Connection Managment
    ////////////////////////////////////////////////////////////////////

public:

    typedef std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>> Connection;
    typedef std::set<Connection> ConnectionList;

public:

    static const ConnectionList& getConnectionGraph();
    static void logConnectionGraph();

    static int connect(std::string source, std::string s_port, std::string target, std::string t_port);
    static int connect(std::string unit_name, std::string source, std::string target);

    static int disconnect(std::string source, std::string s_port, std::string target, std::string t_port);
    static int disconnect(std::string unit_name, std::string source, std::string target);

private:

    static ConnectionList ConnectionGraph;

    ////////////////////////////////////////////////////////////////////
    // EffectUnit Internals
    ////////////////////////////////////////////////////////////////////

private:

    /**
     *  usize_t : nombre de paramètres
     *  {
     *      string : nom unique
     *      hex_t  : valeur
     *  }
     */
    static void serialize_bank(std::ofstream& flux, const ParamArray& obj);
    static ParamArray deserialize_bank(std::ifstream& flux);

    /**
     *  usize_t : nombre de banques
     *  {
     *      hex_t : id de la banque
     *      bank  : la banque
     *  }
     */
    static void serialize_banktable(std::ofstream& flux, const BankTable& obj);
    static BankTable deserialize_banktable(std::ifstream& flux);

    /**
     *  usize_t : nombre de banques
     *  {
     *      hex_t : id de la banque
     *  }
     */
    static void serialize_bankidlist(std::ofstream& flux, const BankIdList& obj);
    static BankIdList deserialize_bankidlist(std::ifstream& flux);

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
    static void serialize_linktable(std::ofstream& flux, const LinkTable& obj);
    static LinkTable deserialize_linktable(std::ifstream& flux);

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
    static int save_PresetFile(std::string file_path, std::weak_ptr<EffectUnit> obj,
            std::string bank_file, std::string link_file);
    //static std::unique_ptr<EffectUnit> load_PresetFile(std::string file_path);

    /**
     *  flag_t  : EffectUnit_Bank_File
     *  Module Infos
     *  BankTable
     *  BankIdList
     */
    static int save_BankFile(std::string file_path, std::weak_ptr<EffectUnit> obj);
    static int load_BankFile(std::string file_path, std::weak_ptr<EffectUnit> obj);

    /**
     *  flag_t  : EffectUnit_Link_File
     *  Module Infos
     *  LinksTable
     */
    static int save_LinkFile(std::string file_path, std::weak_ptr<EffectUnit> obj);
    static int load_LinkFile(std::string file_path, std::weak_ptr<EffectUnit> obj);
};

#endif /* MODULEBASE_H */
