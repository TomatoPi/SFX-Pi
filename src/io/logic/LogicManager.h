/**********************************************************************
 * @file LogicManager.h
 * @author TomatoPi
 * @version 1.0
 *
 * Manager of all mcp23017 related stuff
 **********************************************************************/
#ifndef DEF_LOGIC_MANAGER_H
#define DEF_LOGIC_MANAGER_H

#include <map>

#include "../../core/Typedefs.h"
#include "../../Const.h"

#include "../core/Message.h"
#include "mcp23017.h"

#include "FootSwitch.h"
#include "LED.h"

class LogicManager{

    public :

        static const size_t MCP_COUNT = 2;
        static const hex_t MCP_ADRESS[MCP_COUNT];
        static const hex_t HEX_GPIO[MCP_COUNT];
        static const hex_t HEX_OLAT[MCP_COUNT];

    public :

        static int create();
        static int kill();

        static void updateInputs(NotificationList & notif);
        static void pushNotification(Notification notif);
        static void updateOutputs();

        /**
         * Function used to add a footswitch to the UIManager
         * Provided id will be used to access this footswitch later
         * Provided adress is only used inside manager and must correspond
         * to footswitch physical adress
         * This function register a footswhitch without any indicating led
         * @see registerFootSwitchLED to add footswitch with visual indicator
         *
         * @param id Footswitch Unide Identifer
         * @param adr Footswitch adress on mcp register, a power of two between
         *      0x00 and 0x80
         * @param regIdx mcp register's index witch the footswitch is on
         * @param gpb true if footswitch is on GPIOB, false if on GPIOA
         **/
        static int registerFootSwitch( id2_t id, std::string name,
                                        hex_t adr,
                                        size_t regIdx, bool gpb);

        /**
         * Function used to register a footswitch and it related LED
         **/
        static int registerFootSwitchLED(
                                        id2_t id, std::string name,
                                        hex_t adr,
                                        size_t regIdx, bool gpb,
                                        
                                        id2_t lid, hex_t ladr,
                                        size_t lregIdx, bool lgpb);

        /**
         * Print MCP registries
         **/
        static void printMCPConfig();

        /**
         * Print Manager Content
         **/
        static void printManager();

        /**
         * Get given footswitch
         **/
        static FootSwitch* getFootSwitch(id2_t id);

    private :
    
        /* **** MCP Registers **** */
        static mcp23017 *m_MCPReg[MCP_COUNT];
        static hex_t m_lastReg[MCP_COUNT][2];
        static hex_t m_olatReg[MCP_COUNT][2];

        /**
         * Mask for IODIR register
         * Define witch port is input or output
         **/
        static hex_t m_mask_IODIR[MCP_COUNT][2];
        /**
         * Mask for IPOL register
         * Define polarity of input ports
         **/
        static hex_t m_mask_IPOL [MCP_COUNT][2];

        /* **** FootSwitches Register **** */
        
        /**
         * Foot switches are stored register by register
         * inside an array of maps
         * 
         * footSwitchesMapArray :
         * <hexAdress,FootswitchPointer>[MCPregisterIndex][GPIOA-B]
         **/
        typedef std::map<hex_t,FootSwitch*> ADRFootArray;
        static ADRFootArray m_footSwitchArrayADR[MCP_COUNT][2];

        /**
         * Map of footswitches stored by ID
         **/
        typedef std::map<id2_t,FootSwitch*> IDFootArray;
        static IDFootArray m_footSwitchArrayID;

        /* **** LEDS Register **** */

        struct LEDReg{

            LEDReg()=default;
            LEDReg(LED *led, size_t mcp, bool gpb, hex_t adr);

            LED    *m_led;
            size_t  m_mcp;
            bool    m_gpiob;
            hex_t   m_adr;
        };
        
        /**
         * Map of LEDS stored by ID
         **/
        typedef std::map<id2_t,LEDReg> LEDArray;
        static LEDArray m_LEDArray;
};

#endif
