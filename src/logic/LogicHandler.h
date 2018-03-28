/**********************************************************************
 * @file LogicHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * Object that manage logical IO
 **********************************************************************/
#ifndef DEF_LOGIC_HANDLER_H
#define DEF_LOGIC_HANDLER_H

#include <map>

#include "log.h"
#include "types.h"

#include "core/Event.h"
#include "core/AbstractHandler.h"

#include "logic/mcp23017.h"
#include "Footswitch.h"
#include "Led.h"

#include "LogicConfigLoader.h"

class LogicHandler : public AbstractHandler {
    
    public :
        
        static const SFXP::hex_t HEX_GPIO[2]; /**< Adresses of GPIO[A|B] Reg */
        static const SFXP::hex_t HEX_OLAT[2]; /**< Adresses of OLAT[A|B] Reg */
        static const SFXP::hex_t HEX_IODIR[2];/**< Adresses of IODIR[A|B] Reg */
        static const SFXP::hex_t HEX_IPOL[2]; /**< Adresses of IPOL[A|B] Reg */

    public :
    
        LogicHandler(bool headlessRun);
        virtual ~LogicHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent* event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run();
        
        /* ********************************************************** */
        /* ****************** Registering Functions ***************** */
        /* ********************************************************** */

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
        int registerFootSwitch( SFXP::id2_t id, std::string name,
                                SFXP::hex_t adr,
                                SFXP::usize_t regIdx, bool gpb,
                                SFXP::id2_t lid = SFXP::ErrId2);
                                        
        /**
         * Function used to register a LED
         **/
        int registerLED(SFXP::id2_t lid, SFXP::hex_t ladr,
                        SFXP::usize_t lregIdx, bool lgpb);

        /**
         * Function used to register a footswitch and it related LED
         **/
        int registerFootSwitchLED(  SFXP::id2_t id, std::string name,
                                    SFXP::hex_t adr,
                                    SFXP::usize_t regIdx, bool gpb,
                                    
                                    SFXP::id2_t lid, SFXP::hex_t ladr,
                                    SFXP::usize_t lregIdx, bool lgpb);
        

    private :
    
        void eventUpdateOutput(SFXPEvent* event);

        bool _headless; /**< True if the programm hasn't physical IO connected */
        
        /* ***** MCP23017 Registers Stuff ***** */
        mcp23017**      _registers;     /**< Loaded Registers */
        SFXP::usize_t   _rCount;        /**< Number of Registers */
        
        SFXP::hex_t*     _lastReg; /**< Last GPIO status */
        SFXP::hex_t*     _olatReg; /**< OLAT reg status */

        /**
         * Mask for IODIR register
         * Define witch port is input or output
         **/
        SFXP::hex_t* _mask_IODIR;
        /**
         * Mask for IPOL register
         * Define polarity of input ports
         **/
        SFXP::hex_t* _mask_IPOL;

        /* **** FootSwitches Register **** */
        
        /**
         * Foot switches are stored register by register
         * inside an array of maps
         * 
         * footSwitchesMapArray :
         * <hexAdress,Footswitch*>[MCPregisterIndex][GPIOA-B]
         **/
        typedef std::map<SFXP::hex_t,Footswitch*> ADRFootArray;
        ADRFootArray* _footSwitchArrayADR;

        /**
         * Map of footswitches stored by ID
         **/
        typedef std::map<SFXP::id2_t,Footswitch*> IDFootArray;
        IDFootArray _footSwitchArrayID;

        /* **** LEDS Register **** */

        struct LEDReg{

            LEDReg()=default;
            LEDReg(Led* led, SFXP::usize_t mcp, bool gpb, SFXP::hex_t adr);

            Led*            _led;
            SFXP::usize_t   _mcp;
            bool            _gpiob;
            SFXP::hex_t     _adr;
        };
        
        /**
         * Map of LEDS stored by ID
         **/
        typedef std::map<SFXP::id2_t,LEDReg> LEDArray;
        LEDArray _LEDArray;
};

#endif
