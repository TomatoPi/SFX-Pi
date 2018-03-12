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

#include "Const.h"

#include "lib/io/Notification.h"
#include "io/logic/mcp23017.h"

#include "lib/io/logic/FootSwitch.h"
#include "lib/io/logic/LED.h"

class LogicManager{

    public :

        static const SFXP::usize_t  MCP_COUNT = 2;
        static const SFXP::hex_t    MCP_ADRESS[MCP_COUNT];
        static const SFXP::hex_t    HEX_GPIO[MCP_COUNT];
        static const SFXP::hex_t    HEX_OLAT[MCP_COUNT];

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
        static int registerFootSwitch( SFXP::id2_t id, std::string name,
                                        SFXP::hex_t adr,
                                        SFXP::usize_t regIdx, bool gpb);

        /**
         * Function used to register a footswitch and it related LED
         **/
        static int registerFootSwitchLED(
                                        SFXP::id2_t id, std::string name,
                                        SFXP::hex_t adr,
                                        SFXP::usize_t regIdx, bool gpb,
                                        
                                        SFXP::id2_t lid, SFXP::hex_t ladr,
                                        SFXP::usize_t lregIdx, bool lgpb);

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
        static FootSwitch* getFootSwitch(SFXP::id2_t id);
        static std::vector<SFXP::id2_t> getFootSwitchList();

        /**
         * Update given footswitch configuration
         * Given footswitch has to be registered
         * ( it ID must correspond to a registered id )
         **/
        static int updateSwitchConfiguration(FootSwitch *nswitch);

        /**
         * Clear all footswitches configurations ( target lists )
         **/
        static void clearManager();
        

    private :
    
        /* **** MCP Registers **** */
        static mcp23017*    m_MCPReg[MCP_COUNT];
        static SFXP::hex_t  m_lastReg[MCP_COUNT][2];
        static SFXP::hex_t  m_olatReg[MCP_COUNT][2];

        /**
         * Mask for IODIR register
         * Define witch port is input or output
         **/
        static SFXP::hex_t m_mask_IODIR[MCP_COUNT][2];
        /**
         * Mask for IPOL register
         * Define polarity of input ports
         **/
        static SFXP::hex_t m_mask_IPOL [MCP_COUNT][2];

        /* **** FootSwitches Register **** */
        
        /**
         * Foot switches are stored register by register
         * inside an array of maps
         * 
         * footSwitchesMapArray :
         * <hexAdress,FootswitchPointer>[MCPregisterIndex][GPIOA-B]
         **/
        typedef std::map<SFXP::hex_t,FootSwitch*> ADRFootArray;
        static ADRFootArray m_footSwitchArrayADR[MCP_COUNT][2];

        /**
         * Map of footswitches stored by ID
         **/
        typedef std::map<SFXP::id2_t,FootSwitch*> IDFootArray;
        static IDFootArray m_footSwitchArrayID;

        /* **** LEDS Register **** */

        struct LEDReg{

            LEDReg()=default;
            LEDReg(LED *led, SFXP::usize_t mcp, bool gpb, SFXP::hex_t adr);

            LED*            m_led;
            SFXP::usize_t   m_mcp;
            bool            m_gpiob;
            SFXP::hex_t     m_adr;
        };
        
        /**
         * Map of LEDS stored by ID
         **/
        typedef std::map<SFXP::id2_t,LEDReg> LEDArray;
        static LEDArray m_LEDArray;

        static bool m_status;
};

#endif
