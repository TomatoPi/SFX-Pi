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
#include <cstdlib>

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
#ifdef __SFX_PI__
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
        enum Type
        {
            MODULE,
            SYSTEM
        };

        ShortInfo(std::string i, std::string n, std::string v) :
        ShortInfo(i, n, v, MODULE)
        {
        }
        ShortInfo(std::string i, std::string n, std::string v, Type t) :
        unique_name(i), name(n), version(v), type(t)
        {
        }

        std::string unique_name;
        std::string name;
        std::string version;
        Type type;
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
        
        enum Type
        {
            VALUE,      /**< Slot that modify a bank's value */
            TRIGGER,    /**< Slot without significant value */
            POST_INIT   /**< Slot that will be called once after client Activation */
        };

        Slot(std::string uname, slot_f f, std::string dname, sfx::hex_t d, Type t) :
        internal_name(uname),
        callback(f),
        display_name(dname),
        default_value(d),
        type(t)
        {

        }

        std::string internal_name;
        slot_f callback;

        std::string display_name;
        sfx::hex_t default_value;
        
        Type type;
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
                def_val,
                Slot::VALUE
                );
    }
    /**
     * Fonction utilisée pour enregistrer un slot
     * @param table la table où ajouter le slot
     * @param internal_name nom interne du slot
     * @param display_name nom d'affichage du slot
     * @param callback fonction appekée pour modifier la valeur controlée par le slot
     */
    static inline void register_slot(SlotTable& table, Slot::Type type, sfx::hex_t def_val
            , std::string internal_name, std::string display_name, Slot::slot_f callback)
    {
        table[internal_name] = std::make_shared<Slot>(
                internal_name,
                callback,
                display_name,
                def_val,
                type
                );
    }

private:

    SlotTable slots; /**< Table of Module's slots */

    /**
     * Fonction appelée au chargement du Module pour enregistrer les slots
     *  natifs
     */
    void register_common_slots();

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

#ifdef __SFX_PI__

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
    
    inline size_t relatedUnitsCount() const
    {
        return relatedUnits.size();
    }

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
#ifdef __SFX_PI__
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
    
    static int forceUnloadModule(std::string unique_name);

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

#ifdef __SFX_PI__

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
#ifdef __SFX_PI__
        jack_port_t* port;
#endif
    };
    /**
     * Liste des Ports d'un Effet
     */
    typedef std::vector<Port> PortArray;
    typedef std::set<std::string> PortReg;

public:

#ifdef __SFX_PI__

    inline void registerAudioInput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
        
        this->portsReg.insert(name);
        this->audioIns.push_back(Port{name, sfx::registerAudioInput(client, name)});
    }

    inline void registerAudioOutput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->audioOuts.push_back(Port{name, sfx::registerAudioOutput(client, name)});
    }

    inline void registerMidiInput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->midiIns.push_back(Port{name, sfx::registerMidiInput(client, name)});
    }

    inline void registerMidiOutput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->midiOuts.push_back(Port{name, sfx::registerMidiOutput(client, name)});
    }

#else

    inline void registerAudioInput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->audioIns.push_back(Port{name});
    }

    inline void registerAudioOutput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->audioOuts.push_back(Port{name});
    }

    inline void registerMidiInput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
        this->midiIns.push_back(Port{name});
    }

    inline void registerMidiOutput(std::string name)
    {
        if (portsReg.find(name) != portsReg.end())
            throw std::runtime_error("Port_Name_Duplication");
                    
        this->portsReg.insert(name);
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
     * Fonction utilisée pour déconnecter tout les slots des CC midis
     */
    void clearLinks();

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

#ifdef __SFX_PI__
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
    //std::string current_links_file;

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
     * @brief Methode permettant de changer la banque courrante de l'Unit
     * @param id id de la banque à activer
     * @return 0 on success, -1 si la banque n'existe pas
     **/
    int setBank(sfx::hex_t id);
    /**
     * @brief Methode permettant de passer à la banque suivant de l'Unit
     *  selon l'ordre dans la liste @see banks_order
     **/
    void nextBank();
    /**
     * @brief Methode permettant de passer à la banque précédente de l'Unit
     *  selon l'odre dans la liste @see banks_order
     **/
    void prevBank();

    /**
     * @brief Methode permettant de creer et ajouter une banque à l'Unit
     *  attribut un ID automatiquement
     */
    int createBank();
    /**
     * @brief Methode permettant de creer et ajouter une banque à l'Unit
     * @param id ID de la banque à creer
     * @return 0 on success, EXISTING_BANK si la banque existe déjà
     **/
    int createBank(sfx::hex_t id);
    /**
     * @brief method to duplicate a bank of the effect
     *  Usage :
     *      copyBank(sid) to copy bank[sid] to a new one
     *      copyBank(sid, tid) to copy bank[sid] in bank[tid]
     * @pre sid != tid
     */
    int copyBank(sfx::hex_t sid);
    int copyBank(sfx::hex_t sid, sfx::hex_t tid);
    /**
     * @brief Method used to remove a bank from the effect
     *  if the bank removed is the last one, automaticaly regenerate the default bank
     * @pre _currentBank bank is valid
     * @post _currentBank bank is valid
     */
    int removeBank(sfx::hex_t id);

    inline sfx::hex_t getCurrentBank() const
    {
        return *current_bank;
    }

private:

    /**
     * Fonction interne utilisée pour mettre à jour l'effet en appelant tout les slots
     *  actifs de la banque courante
     * @pre current_bank bank is valid
     */
    void updateCurrentBank();

    BankTable banks;
    BankIdList banks_order;

    BankIdList::iterator current_bank;

    UIDManager<sfx::hex_t> bank_id_manager;
    //std::string current_banks_file;

    ////////////////////////////////////////////////////////////////////
    // Constructeur / Destructeur
    ////////////////////////////////////////////////////////////////////

public:

    EffectUnit(std::weak_ptr<Module> module_weak);
    ~EffectUnit();
    
private :
    
    /**
     * Methode appelée après la creation de l'effet
     *  Appelle tout les slots de type POST_INIT du module
     */
    void postInit();

    ////////////////////////////////////////////////////////////////////
    // EffectUnit Handling
    ////////////////////////////////////////////////////////////////////

public:

    typedef std::map<sfx::string, std::shared_ptr<EffectUnit>> EffectUnitsTable;

public:

    static const EffectUnitsTable& getEffectUnitsTable();
    static std::weak_ptr<EffectUnit> getEffectUnit(const std::string& unique_name);

    static void logEffectUnitsTable();
    //static void logEffectUnit(std::string unit_name);

    /**
     * Fonction pour creer une Unité de Traitement à partir d'un module
     *  l'unité générée sera celle avec les paramètres par défault
     * @param module_name nom interne du module à utiliser
     * @param unique_name pointeur vers une chaine de caractère où sera stoqué le nom unique de l'effet crée
     * @return 0 on success
     */
    static int buildEffectUnitFromModule(const std::string& module_name, std::string* unique_name = nullptr);
    /**
     * Fonction pour creer une Unité à partir d'un fichier Preset Sauvegardé
     *  précédement
     * @param file_path Chemin du fichier à interpreter
     * @param unique_name pointeur vers une chaine de caractère où sera stoqué le nom unique de l'effet crée
     * @return 0 on success
     */
    static int buildEffectUnitFromPresetFile(const std::string& file_path, std::string* unique_name = nullptr);
    /**
     * Fonction pour detruire une unité
     * @param unit_name nom interne de l'unité à detruire
     * @return 0 on success
     */
    static int destroyEffectUnit(std::string unit_name);
    /**
     * Fonction pour detruire une unité
     * @param unit_name nom interne de l'unité à detruire
     * @return 0 on success
     */
    static int forceDestroyEffectUnit(std::string unit_name);
    /**
     * Fonction pour sauvegarder une unité
     * @param unit_name nom interne de l'unité à sauvegarder
     * @return 0 on success
     */
    static int saveEffectUnit(std::string unit_name, std::string file,
            std::string bank_file = "", std::string link_file = "");

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

public:

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

    ////////////////////////////////////////////////////////////////////
    // Graph Serial
    ////////////////////////////////////////////////////////////////////

public:

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
    static int save_EnvironementFile(std::string file_path);
    static int load_EnvironementFile(std::string file_path);

private:

    typedef std::pair<std::pair<sfx::id1_t, std::string>, std::pair<sfx::id1_t, std::string>> SerialConnection;
    typedef std::map<sfx::id1_t, std::string> SerialToNameTable;
    typedef std::map<std::string, sfx::id1_t> NameToSerialTable;

    static NameToSerialTable effectUnitTable_to_serial();
    static SerialConnection connection_to_serial(NameToSerialTable table, const Connection& obj);
    static Connection connection_from_serial(SerialToNameTable table, const SerialConnection& obj);

    /**
     * id1_t : Source uid
     * string : Source Port Name
     * id1_t : Target uid
     * string : Target Port Name
     */
    static void serialiaze_Connection(std::ofstream& flux, const SerialConnection& obj);
    static SerialConnection deserialize_Connection(std::ifstream& flux);
};

#endif /* MODULEBASE_H */
