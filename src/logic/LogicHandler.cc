/**********************************************************************
 * @file LogicHandler.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Object that manage logical IO
 **********************************************************************/
#include "LogicHandler.h"

using namespace SFXP;
/**< Adresses of GPIO[A|B] Reg */
const SFXP::hex_t LogicHandler::HEX_GPIO[2] =
{
    mcp23017::HEX_GPIOA,
    mcp23017::HEX_GPIOB
};
/**< Adresses of OLAT[A|B] Reg */
const SFXP::hex_t LogicHandler::HEX_OLAT[2] =
{
    mcp23017::HEX_OLATA,
    mcp23017::HEX_OLATB
};
/**< Adresses of IODIR[A|B] Reg */
const SFXP::hex_t LogicHandler::HEX_IODIR[2] =
{
    mcp23017::HEX_IODIRA,
    mcp23017::HEX_IODIRB
};
/**< Adresses of IPOL[A|B] Reg */
const SFXP::hex_t LogicHandler::HEX_IPOL[2] =
{
    mcp23017::HEX_IPOLA,
    mcp23017::HEX_IPOLB
};

LogicHandler::LogicHandler(bool headlessRun):AbstractHandler("LogicHandler")
    ,_headless(headlessRun)
{
    if (_headless)
        SFXPlog::log(_name) << "Headless Run, All IO operations are Virtual"
                            << '\n';

    try {
        // First load Config File
        LogicConfigLoader loader = LogicConfigLoader(SFXP::DIR_CONF);
        if (loader.status())
            throw string("Failed Load Logic Configuration File");

        // First open connections to mcp23017
        vector<LogicConfigLoader::RegisterConfig> regs = loader.registers();
        this->_rCount = regs.size();
        if (regs.size() == 0)
            throw string("No Registers Loaded");

        if (!_headless) {
                
            this->_registers = new mcp23017*[_rCount];
            for (auto& r : regs) {

                if (r._idx >= _rCount)
                    throw string("Invalid Register Index : %lu", r._idx);
                
                _registers[r._idx] = new mcp23017(r._adress, "/dev/i2c-1");
            }
        }
        else {

            this->_registers = nullptr;
        }

        // Create all registers and masks
        this->_lastReg = new SFXP::hex_t[2*_rCount];
        this->_olatReg = new SFXP::hex_t[2*_rCount];
        
        this->_mask_IODIR = new SFXP::hex_t[2*_rCount];
        this->_mask_IPOL  = new SFXP::hex_t[2*_rCount];

        for (usize_t i = 0; i < _rCount; i++) {
            for (usize_t k = 0; k < 2; k++) {

                this->_lastReg[i+_rCount*k] = 0;
                this->_olatReg[i+_rCount*k] = 0;
                
                this->_mask_IODIR[i+_rCount*k] = 0;
                this->_mask_IPOL [i+_rCount*k] = 0;
            }
        }

        this->_footSwitchArrayADR = new ADRFootArray[_rCount*2];

        // Then register footswitches
        for (auto& f : loader.inputs()) {

            // If footswitch has a led
            if (f._hled) {

                if (this->registerFootSwitchLED(
                                            f._id, f._name,
                                            f._adress._adress,
                                            f._adress._mcpIndex,
                                            f._adress._gpiob,

                                            f._led._id,
                                            f._led._adress._adress,
                                            f._led._adress._mcpIndex,
                                            f._led._adress._gpiob))
                {
                    throw string("Failed Register FootswitchLed");
                }
            }
            else {

                if (this->registerFootSwitch(
                                            f._id, f._name,
                                            f._adress._adress,
                                            f._adress._mcpIndex,
                                            f._adress._gpiob))
                {
                    throw string("Failed Register Footswitch");
                }
            }
        }

        // Register Leds
        for (auto& l : loader.outputs()) {

            if (this->registerLED(  l._id, l._adress._adress,
                                    l._adress._mcpIndex,
                                    l._adress._gpiob))
            {
                throw string("Failed Register Led");
            }
        }

        if (!_headless) {
                
            // Write configuration to registers
            for (usize_t i = 0; i < _rCount; i++) {
                for (usize_t k = 0; k < 2; k++) {

                    // Set inputs and Ouputs
                    if (_registers[i]->writeReg(HEX_IODIR[k], _mask_IODIR[i+_rCount*k]) < 0)
                        throw string("Failed Write IODIR on %u", i);

                    // Set IO Polarities
                    if (_registers[i]->writeReg(HEX_IPOL[k], _mask_IPOL[i+_rCount*k]) < 0)
                        throw string("Failed Write IPOL on %u", i);

                    // Set read current values
                    _registers[i]->readReg(HEX_GPIO[k], _lastReg[i+_rCount*k]);
                }
            }
        }

        // All is done
        this->_status = HANDLER_OK;
    }
    catch (const std::string& e) {

        SFXPlog::err(_name) << e << endl;
        _status = ERRORED;
    }
}
LogicHandler::~LogicHandler()
{
    if (_status == HANDLER_OK) {

        if (!_headless) {
                
            // Free the mcp23017
            for (usize_t i = 0; i < _rCount; i++) delete _registers[i];
            delete[] _registers;
        }
        
        delete[] _lastReg;
        delete[] _olatReg;
        
        delete[] _mask_IODIR;
        delete[] _mask_IPOL;
        
        // Free the footswitches
        for (auto& f : _footSwitchArrayID) {
            
            delete f.second;
        } 
        delete[] _footSwitchArrayADR;
    }
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void LogicHandler::pushEvent(SFXPEvent* event) {
    
    if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (_status) {

            SFXPlog::err(_name) << "Handler Errored : " << _status << endl;
        }
        else {
                
            SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
        }
    }
    else if (_status == HANDLER_OK) {

        if (!_headless) {
        if ( event->_type == SFXPEvent::Type::Event_UpdateOutput ) {

            this->eventUpdateOutput(event);
        }
        }
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void LogicHandler::run() {
    
    if ( _status == HANDLER_OK && !_headless){
    /*
     * Read each MCP registers
     * Update footswitch status
     */
    for ( usize_t i = 0; i < _rCount; i++ ){
        for ( usize_t k = 0; k < 2; k++ ){

            hex_t currentReg = 0x00;
            hex_t mask = _mask_IODIR[i+_rCount*k];

            // First read correct register
            _registers[i]->readReg(HEX_GPIO[k], currentReg);

            // If input registers has changed
            if ( (currentReg & mask) != (_lastReg[i+_rCount*k] & mask) ){

                ADRFootArray& foot = _footSwitchArrayADR[i+_rCount*k];
                
                // Try get right footswitch by it adress
                // Will always work if only one switch has changed
                // Adress is taken with XORing last and current register
                hex_t adr = (currentReg & mask) ^ (_lastReg[i+_rCount*k] & mask);
                
                if ( foot.find(adr) != foot.end() ){

                    #ifdef __DEBUG__
                    if ( SFXP::GlobalIsDebugEnabled ){
                        SFXPlog::debug(_name);
                        printf("Switch( %lu:%lu:%02x ) has changed : %02x\n",
                            i, k, adr, adr & currentReg );
                    }
                    #endif

                    // Get footswitch
                    Footswitch *fswitch = foot[adr];
                    // Create the correct Event
                    SFXPEvent e = SFXPEvent(SFXPEvent::Type::Event_FootswitchChanged);
                    SFXPEvent lede = SFXPEvent(SFXPEvent::Type::Event_UpdateOutput);

                    //Verify that switch has to be updated
                    if ( !!(adr & currentReg) )
                    {
                        e._io = IOEvent(IOEvent::RELEASED, 0, fswitch->getID());
                        if (fswitch->hasVisual())
                            lede._io = IOEvent(IOEvent::OFF, 0, fswitch->getVisualID());
                    }
                    else // if ( !(adr & currentReg) )
                    {
                        e._io = IOEvent(IOEvent::PUSHED, 0, fswitch->getID());
                        if (fswitch->hasVisual())
                            lede._io = IOEvent(IOEvent::ON, 0, fswitch->getVisualID());
                    }

                    _eventHandler->pushEvent(&e);
                    if (fswitch->hasVisual())
                        this->eventUpdateOutput(&lede);
                }
                // Else several footswitches were pressed or changed
                // Or an unregistered Button has been pressed
                else{

                    SFXPlog::err(_name);
                    printf("SwitchAdress( %lu:%lu:%02x ) not found\n",
                        i, k, adr);
                } 
            }// Reg Changed Check end

            // Save register status by masking non input entries
            // Prevent error message to be thrown after an output update
            mcp23017::changeBit(_lastReg[i+_rCount*k], currentReg, mask);
        }
    }// MCP read end
    }// If status end
}// Run end

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
int LogicHandler::registerFootSwitch(   id2_t id, std::string name,
                                        hex_t adr,
                                        usize_t regIdx, bool gpb,
                                        id2_t lid)
{
    // First convert given id to a footswitch id
    id =  (id & SFXP::ID_MASK) | SFXP::ID_FOOT;
    
    /* Verify that provided register id is valid */
    if ( regIdx >= _rCount ){

        SFXPlog::err(_name);
        printf("FootSwitch : MCP( %3lu ) doesn't Exist\n",
            regIdx);
        return 1;
    }

    /* Get correct FootSwitch Array */
    ADRFootArray & reg = _footSwitchArrayADR[regIdx+_rCount*gpb];
    
    /* Verify that given id and adresses are not present */
    if ( _footSwitchArrayID.find(id) != _footSwitchArrayID.end() ){

        SFXPlog::err(_name);
        printf("FootSwitch : ID ( 0x%04x ) is already used\n",
            id);
        return 1;
    }
    if ( reg.find(adr) != reg.end() ){

        SFXPlog::err(_name);
        printf("FootSwitch : Adress ( %02x ) is already used\n",
            adr);
        return 1;
    }

    /* Provided Footswitch is valid, Create and add it */
    Footswitch *newSwitch(nullptr);
    if (lid == SFXP::ErrId2) newSwitch = new Footswitch( id, name );
    else newSwitch = new Footswitch( id, name, lid );

    reg[adr]               = newSwitch;
    _footSwitchArrayID[id] = newSwitch;

    /*
     * Update Register masks
     * Set given adress to input (1)
     * And reverse it state (1)
     * */
    mcp23017::changeBit( _mask_IODIR[regIdx+_rCount*gpb], 0xff, adr );
    mcp23017::changeBit( _mask_IPOL [regIdx+_rCount*gpb], 0xff, adr );
    
    SFXPlog::log(_name);
    printf("Registered : FootSwitch : \"%-10s\" : ID( 0x%04x ) : Adress( MCP(%lu) GPIO(%i) Hex(0x%02x) )\n",
        name.c_str(),
        id,
        regIdx,
        gpb,
        adr
        );

    return 0;
}

/**
 * Function used to register a LED
 **/
int LogicHandler::registerLED(  id2_t lid, hex_t ladr,
                                usize_t lregIdx, bool lgpb) 
{
        
    // Convert ids to correct ids types
    lid = (lid & SFXP::ID_MASK) | SFXP::ID_LED;
    
    /* Verify that provided register id is valid */
    if ( lregIdx >= _rCount ){

        SFXPlog::err(_name);
        printf("LED : MCP( %3lu ) doesn't Exist\n",
            lregIdx);
        return 1;
    }
    
    /* Verify that given id and adresses are not present */
    if ( _LEDArray.find(lid) != _LEDArray.end() ){

        SFXPlog::err(_name);
        printf("LED : ID ( 0x%04x ) is already used\n",
            lid);
        return 1;
    }

    /* Provided Leds is valid, create and add it */
    Led         *newLED     = new Led(lid);
    
    _LEDArray[lid]       = LEDReg(newLED, lregIdx, lgpb, ladr);

    /*
     * Update Register masks
     * Set LED adress to ouput (0)
     * */
    mcp23017::changeBit( _mask_IODIR[lregIdx+_rCount*lgpb], 0x00,ladr );

    SFXPlog::log(_name);
    printf("Registered : LED        : ID( 0x%04x ) :              : Adress( MCP(%lu) GPIO(%i) Hex(0x%02x) )\n",
        lid,
        lregIdx,
        lgpb,
        ladr
        );
    
    return 0;
}

/**
 * Function used to register a footswitch and it related LED
 **/
int LogicHandler::registerFootSwitchLED(id2_t id, std::string name,
                                        hex_t adr,
                                        usize_t regIdx, bool gpb,
                                        
                                        id2_t lid, hex_t ladr,
                                        usize_t lregIdx, bool lgpb) 
{
    // First try to register the footswitch
    if (this->registerFootSwitch(id, name, adr, regIdx, gpb, lid)) {
        
        return 1;
    }
    
    // Try to register the LED
    return this->registerLED(lid, ladr, lregIdx, lgpb);
}

LogicHandler::LEDReg::LEDReg(Led* led, usize_t mcp, bool gpb, hex_t adr):
    _led(led),
    _mcp(mcp),
    _gpiob(gpb),
    _adr(adr)
{
}

/* ********************************************************** */
/* ****************** SFXPEvent Handling ******************** */
/* ********************************************************** */

void LogicHandler::eventUpdateOutput(SFXPEvent* event) {

    id2_t id = event->_io._id;
    hex_t status = event->_io._status == IOEvent::ON ? 0xff : 0x00;

    if (_LEDArray.find(id) != _LEDArray.end()) {

        LEDReg led = _LEDArray[id];
        // Index of register that store led value
        usize_t idx = led._mcp+_rCount*led._gpiob;

        // First update led Status
        led._led->setState(status);

        // Update register
        mcp23017::changeBit(_olatReg[idx], status, led._adr);

        // Write new register's value
        _registers[led._mcp]->writeReg(
                    HEX_OLAT[led._gpiob] // Write to the OLAT reg
                    , _olatReg[idx]      // New value for the reg
                    , ~_mask_IODIR[idx]  // Only change ouputs values (IODIR==0)
                    );
    }
}
