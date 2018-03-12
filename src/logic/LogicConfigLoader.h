/**********************************************************************
 * @file LogicConfigLoader.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load logical io configuration from an
 *  xml file
 **********************************************************************/
#ifndef DEF_LOGIC_CONFIG_LOADER_H
#define DEF_LOGIC_CONFIG_LOADER_H

#include <tinyxml2.h>
#include <iostream>
#include <vector>
#include <string>

#include "types.h"
#include "log.h"

#include "logic/mcp23017.h"
#include "logic/Footswitch.h"
#include "logic/Led.h"

class LogicConfigLoader {

    public :

        struct AdressConfig {
            
            AdressConfig(SFXP::hex_t adr, SFXP::usize_t idx, bool gpb):
                _adress(adr), _mcpIndex(idx), _gpiob(gpb)
            {
            }
            AdressConfig():AdressConfig(0, 0, 0)
            {
            }
            
            SFXP::hex_t     _adress;    /**< Hex adress on the register */
            SFXP::usize_t   _mcpIndex;  /**< Index of the register */
            bool            _gpiob;     /**< True if on GPIOB */
        };

        struct LedConfig {

            LedConfig(SFXP::id2_t id, AdressConfig adr):
                _id(id), _adress(adr)
            {
            }
            LedConfig():LedConfig(0, AdressConfig())
            {
            }

            SFXP::id2_t  _id;       /**< Id of the led */
            AdressConfig _adress;   /**< Adress of the led */
        };

        struct FootConfig {

            FootConfig(SFXP::id2_t id, std::string name, AdressConfig adr, bool h, LedConfig led):
                _id(id), _name(name), _adress(adr), _hled(h), _led(led)
            {
            }
            FootConfig():FootConfig(0,"", AdressConfig(), false, LedConfig())
            {
            }

            SFXP::id2_t  _id;       /**< Id of the footswitch */
            std::string  _name;     /**< Name of the footswitch */
            AdressConfig _adress;   /**< Adress of the footswitch */
            bool _hled;             /**< True if switch has a led */
            LedConfig _led;         /**< Led if exist config */
        };

        struct RegisterConfig {

            RegisterConfig(SFXP::hex_t adr, SFXP::usize_t idx):
                _adress(adr), _idx(idx)
            {
            }
            RegisterConfig():RegisterConfig(0, 0)
            {
            }

            SFXP::hex_t _adress;/**< I2C Physical adress of the Register */
            SFXP::usize_t _idx; /**< Internal index wich on the reg will be stored */
        };

        LogicConfigLoader(std::string path);
        ~LogicConfigLoader();

        /**
         * Return true if Loader failed parse XML file
         **/
        bool status() const;

        std::vector<RegisterConfig> registers() const;
        std::vector<FootConfig>     inputs() const;
        std::vector<LedConfig>      outputs() const;

    private :

        /**
         * Functions to parse XMLElements
         * return non-zero if an error occured during parsing
         **/
        int parseAdressConfig   (tinyxml2::XMLElement* elem, AdressConfig& adr);
        int parseFootConfig     (tinyxml2::XMLElement* foot, FootConfig& f);
        int parseLedConfig      (tinyxml2::XMLElement* led, LedConfig& l);
        int parseRegisterConfig (tinyxml2::XMLElement* reg, RegisterConfig& r);

        bool _status;

        std::vector<RegisterConfig> _registers;
        std::vector<FootConfig>     _inputs;
        std::vector<LedConfig>      _outputs;
};

#endif
