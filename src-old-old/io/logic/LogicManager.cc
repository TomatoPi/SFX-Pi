/**********************************************************************
 * @file LogicManager.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Manager of all mcp23017 related stuff
 **********************************************************************/
#include "LogicManager.h"
/* ****************************** Const ***************************** */
const hex_t LogicManager::MCP_ADRESS[MCP_COUNT] =
    {
        mcp23017::HEX_MCP_0,
        mcp23017::HEX_MCP_1
    };
    
const hex_t LogicManager::HEX_GPIO[MCP_COUNT] =
    {
        mcp23017::HEX_GPIOA,
        mcp23017::HEX_GPIOB
    };

const hex_t LogicManager::HEX_OLAT[MCP_COUNT] =
    {
        mcp23017::HEX_OLATA,
        mcp23017::HEX_OLATB
    };
/* **** MCP Registers **** */
mcp23017 *LogicManager::m_MCPReg[MCP_COUNT] = {NULL, NULL};
hex_t     LogicManager::m_lastReg[MCP_COUNT][2] = {{0}};
hex_t     LogicManager::m_olatReg[MCP_COUNT][2] = {{0}};

hex_t LogicManager::m_mask_IODIR[MCP_COUNT][2] = {{0}};
hex_t LogicManager::m_mask_IPOL [MCP_COUNT][2] = {{0}};
        
/* **** FootSwitches Register **** */
LogicManager::ADRFootArray LogicManager::m_footSwitchArrayADR[MCP_COUNT][2]
    = {{LogicManager::ADRFootArray()}};

LogicManager::IDFootArray LogicManager::m_footSwitchArrayID
    = LogicManager::IDFootArray();

/* **** LEDS Register **** */
LogicManager::LEDArray LogicManager::m_LEDArray
    = LogicManager::LEDArray();
/* **** Status **** */
bool LogicManager::m_status = false;

/* ****************** Create and Kill Functions ********************* */
int LogicManager::create(){

    printf("Logic Manager : Start ...\n");

    try{
        
        /* **** Setup MCP Registers **** */
        for ( usize_t i = 0; i < MCP_COUNT; i++ ){

            m_MCPReg[i] = new mcp23017(MCP_ADRESS[i], "/dev/i2c-1");

            if ( m_MCPReg[i]->writeReg(mcp23017::HEX_IODIRA, m_mask_IODIR[i][0]) < 0 ){

                throw std::string("Failed Write IODIRA on %u", i);
            }
                
            if ( m_MCPReg[i]->writeReg(mcp23017::HEX_IODIRB, m_mask_IODIR[i][1]) < 0 ){

                throw std::string("Failed Write IODIRB on %u", i);
            }
                
            if ( m_MCPReg[i]->writeReg(mcp23017::HEX_IPOLA, m_mask_IPOL[i][0]) < 0 ){

                throw std::string("Failed Write IPOLA on %u", i);
            }
                
            if ( m_MCPReg[i]->writeReg(mcp23017::HEX_IPOLB, m_mask_IPOL[i][1]) < 0 ){

                throw std::string("Failed Write IPOLB on %u", i);
            }
        }

        printf("... Done\n");
    }
    catch( std::string & e ){

        printf("Logic Manager : Error : %s\n", e.c_str());
        m_status = false;
        return 1;
    }

    m_status = true;
    return 0;
}
int LogicManager::kill(){

    printf("Logic Manager : Stop ...\n");

    if ( m_status ){
        
    /* **** Delete MCP Registers **** */
    for ( usize_t i = 0; i < MCP_COUNT; i++ ){

        delete m_MCPReg[i];
    }
    
    }

    printf("... Done\n");

    return 0;
}

void LogicManager::updateInputs(NotificationList & notif){

    if ( m_status ){
    /*
     * Read each MCP registers
     * Update footswitch status
     */
    for ( usize_t i = 0; i < MCP_COUNT; i++ ){
        for ( usize_t k = 0; k < 2; k++ ){

            hex_t currentReg = 0x00;
            hex_t mask = m_mask_IODIR[i][k];

            // First read correct register
            m_MCPReg[i]->readReg(HEX_GPIO[i], currentReg);

            // If input registers has changed
            if ( (currentReg & mask) != (m_lastReg[i][k] & mask) ){

                ADRFootArray & foot = m_footSwitchArrayADR[i][k];
                
                // Try get right footswitch by it adress
                // Will always work if only one switch has changed
                // Adress is taken with XORing last and current register
                hex_t adr = (currentReg & mask) ^ (m_lastReg[i][k] & mask);
                
                if ( foot.find(adr) != foot.end() ){

                    if ( SFXP::GlobalIsDebugEnabled ){
                        printf("Logic Manager : Update : Switch( %lu:%lu:%02x ) has changed : %02x\n",
                            i, k, adr, adr & currentReg );
                    }

                    // Get footswitch, get it target list
                    // And push an update request for them
                    FootSwitch *fswitch = foot[adr];
                    FootSwitch::Mode mode = fswitch->getMode();

                    //Notification to push
                    Notification::NotificationType notifType
                        = Notification::NullNotification;

                    //Verify that switch has to be updated
                    if ( !!(adr & currentReg)
                        && ( mode == FootSwitch::Mode::Push
                            || mode == FootSwitch::Mode::PushPull ) )
                    {
                        notifType = Notification::PushNotification;
                    }
                    else if ( !(adr & currentReg)
                        && ( mode == FootSwitch::Mode::Pull
                            || mode == FootSwitch::Mode::PushPull ) )
                    {
                        notifType = Notification::PushNotification;
                    }
                    else if ( SFXP::GlobalIsDebugEnabled ){

                        // Do nothing
                        printf("Logic Manager : Update : Nothing to be done \n");
                    }

                    //If a notification has to be pushed
                    if ( notifType != Notification::NullNotification ){

                        //Get list of Footswitch's targets
                        std::vector<id2_t> targets = fswitch->getTargets();
                        id2_t source = fswitch->getID();

                        for ( auto & cur : targets ){


                            if ( SFXP::GlobalIsDebugEnabled ){
                                printf("Logic Manager : Update : Push Notification ( %04x:%04x:%02x )\n"
                                    ,source
                                    ,cur
                                    ,static_cast<id1_t>(notifType)
                                    );
                            }
                            notif.push_back(Notification(source, cur, notifType));
                        }
                    }
                }
                // Else several footswitches were pressed or changed
                // Or an unregistered Button has been pressed
                else{

                    printf("Logic Manager : Error : SwitchAdress( %lu:%lu:%02x ) not found\n",
                        i, k, adr);
                } 
            }// Reg Changed Check end

            // Save register status by masking non input entries
            // Prevent error message to be thrown after an output update
            mcp23017::changeBit(m_lastReg[i][k], currentReg, mask);
        }
    }// MCP read end
    
    }// If status end

}// Update Inputs End

void LogicManager::pushNotification(Notification notif){

    if ( m_status ){
        
    if ( m_LEDArray.find(notif.target()) != m_LEDArray.end() ){

        if ( SFXP::GlobalIsDebugEnabled ){
            printf("LogicManager : Update : Notification received : target 0x%04x\n"
                ,notif.target());
        }

        // Get LED, toggle it state and update register
        LEDReg   led = m_LEDArray[notif.target()];
        hex_t & olat = m_olatReg[led.m_mcp][led.m_gpiob];

        if ( led.m_led->toggle() ){

            mcp23017::changeBit(olat, 0xff, led.m_adr);
        }
        else{

            mcp23017::changeBit(olat, 0x00, led.m_adr);
        }
    }
    else{

        printf("LogicManager : Error : Notification target 0x%04x not Found\n"
                ,notif.target());
    }
    
    }
}
void LogicManager::updateOutputs(){

    if ( m_status ){
        
    for ( usize_t i = 0; i < MCP_COUNT; i++ ){
        for ( usize_t k = 0; k < 2; k++ ){

            m_MCPReg[i]->writeReg(
                HEX_OLAT[k]
                , m_olatReg[i][k]
                , ~m_mask_IODIR[i][k]
                );
        }
    }
    
    }
}

/* *************************** Registration ************************* */
int LogicManager::registerFootSwitch( id2_t id, std::string name,
                                hex_t adr,
                                usize_t regIdx, bool gpb)
{

    if ( m_status ){
        
    // First convert given id to a footswitch id
    id =  (id & ~SFXP::ID_MASK) | SFXP::ID_FOOT;
    
    /* Verify that provided register id is valid */
    if ( regIdx >= MCP_COUNT ){

        printf("Logic Manager : Error : FootSwitch : MCP( %3lu ) doesn't Exist\n",
            regIdx);
        return 1;
    }

    /* Get correct FootSwitch Array */
    ADRFootArray & reg = m_footSwitchArrayADR[regIdx][gpb];
    
    /* Verify that given id and adresses are not present */
    if ( m_footSwitchArrayID.find(id) != m_footSwitchArrayID.end() ){

        printf("Logic Manager : Error : FootSwitch : ID ( 0x%04x ) is already used\n",
            id);
        return 1;
    }
    if ( reg.find(adr) != reg.end() ){

        printf("Logic Manager : Error : FootSwitch : Adress ( %02x ) is already used\n",
            adr);
        return 1;
    }

    /* Provided Footswitch is valid, Create and add it */
    FootSwitch *newSwitch = new FootSwitch( id, name );

    reg[adr]                = newSwitch;
    m_footSwitchArrayID[id] = newSwitch;

    /*
     * Update Register masks
     * Set given adress to input (1)
     * And reverse it state (1)
     * */
    mcp23017::changeBit( m_mask_IODIR[regIdx][gpb], 0xff, adr );
    mcp23017::changeBit( m_mask_IPOL [regIdx][gpb], 0xff, adr );
    

    printf("Logic Manager : Registered : FootSwitch : \"%-10s\" : ID( 0x%04x ) : Adress( MCP(%lu) GPIO(%i) Hex(0x%02x) )\n",
        name.c_str(),
        id,
        regIdx,
        gpb,
        adr
        );

    }
    return 0;
}

int LogicManager::registerFootSwitchLED(
                                id2_t id, std::string name,
                                hex_t adr,
                                usize_t regIdx, bool gpb,
                                
                                id2_t lid, hex_t ladr,
                                usize_t lregIdx, bool lgpb)
{
    
    if ( m_status ){
        
    // Convert ids to correct ids types
    id =  (id  & ~SFXP::ID_MASK) | SFXP::ID_FOOT;
    lid = (lid & ~SFXP::ID_MASK) | SFXP::ID_LED;
    
    /* Verify that provided register id is valid */
    if ( regIdx >= MCP_COUNT ){

        printf("Logic Manager : Error : FootSwitch : MCP( %3lu ) doesn't Exist\n",
            regIdx);
        return 1;
    }

    /* Get correct FootSwitch Array */
    ADRFootArray & reg = m_footSwitchArrayADR[regIdx][gpb];
    
    /* Verify that given id and adresses are not present */
    if ( m_footSwitchArrayID.find(id) != m_footSwitchArrayID.end() ){

        printf("Logic Manager : Error : FootSwitch : ID ( 0x%04x ) is already used\n",
            id);
        return 1;
    }
    if ( reg.find(adr) != reg.end() ){

        printf("Logic Manager : Error : FootSwitch : Adress ( %u:%02x ) is already used\n",
            gpb, adr);
        return 1;
    }

    /* Then verify that LED is valid */
    
    /* Verify that provided register id is valid */
    if ( lregIdx >= MCP_COUNT ){

        printf("Logic Manager : Error : LED : MCP( %3lu ) doesn't Exist\n",
            lregIdx);
        return 1;
    }
    
    /* Verify that given id and adresses are not present */
    if ( m_LEDArray.find(id) != m_LEDArray.end() ){

        printf("Logic Manager : Error : LED : ID ( 0x%04x ) is already used\n",
            lid);
        return 1;
    }

    /* Provided Footswitch and Leds are valid, create and add them */
    FootSwitch  *newSwitch  = new FootSwitch(id, name, lid);
    LED         *newLED     = new LED(lid);
    
    reg[adr]                = newSwitch;
    m_footSwitchArrayID[id] = newSwitch;
    
    m_LEDArray[lid]       = LEDReg(newLED, lregIdx, lgpb, ladr);

    /*
     * Update Register masks
     * Set FootSwitch adress to input (1)
     * And reverse it state (1)
     * Set LED adress to ouput (0)
     * */
    mcp23017::changeBit( m_mask_IODIR[regIdx][gpb], 0xff, adr );
    mcp23017::changeBit( m_mask_IPOL [regIdx][gpb], 0xff, adr );
    
    mcp23017::changeBit( m_mask_IODIR[regIdx][gpb], 0x00,ladr );
    
    printf("Logic Manager : Registered : FootSwitch : \"%-10s\" : ID( 0x%04x ) : Adress( MCP(%lu) GPIO(%i) Hex(0x%02x) )\n",
        name.c_str(),
        id,
        regIdx,
        gpb,
        adr
        );
    printf("Logic Manager : Registered : LED        : ID( 0x%04x ) : Adress( MCP(%lu) GPIO(%i) Hex(0x%02x) )\n",
        lid,
        lregIdx,
        lgpb,
        ladr
        );

    }
    
    return 0;
}

void LogicManager::printMCPConfig(){

    if ( m_status ){
        
    for ( usize_t i = 0; i < MCP_COUNT; i++ ){

        for ( usize_t k = 0; k < 2; k++ ){

            printf("MCP-Configuration : 0x%2x : IODIR%c ( 0x%02x ) : IPOL%c ( 0x%02x )\n",
                MCP_ADRESS[i],
                (k==0)?'A':'B',
                m_mask_IODIR[i][k],
                (k==0)?'A':'B',
                m_mask_IPOL[i][k]
                );
        }
    }

    }
    else{

        printf("Logic Manager : Error : Logic Manager is in an errored state\n");
    }
}

void LogicManager::printManager(){

    if ( m_status ){
        
        printf("Ooooh le beau manager\n");

    }
    else{

        printf("Logic Manager : Error : Logic Manager is in an errored state\n");
    }
}

/**
 * Get given footswitch
 **/
FootSwitch* LogicManager::getFootSwitch(id2_t id){

    if ( m_status ){
            
        id =  (id & ~SFXP::ID_MASK) | SFXP::ID_FOOT;

        if ( m_footSwitchArrayID.find(id) != m_footSwitchArrayID.end() ){

            return m_footSwitchArrayID[id];
        }
    }
    return NULL;
}
std::vector<id2_t> LogicManager::getFootSwitchList(){

    if ( m_status ){
            
        std::vector<id2_t> ids;

        for ( auto & cur : m_footSwitchArrayID ){

            ids.push_back(cur.first);
        }
        return ids;
    }
    return std::vector<id2_t>();
}


/**
 * Update given footswitch configuration
 * Given footswitch has to be registered
 * ( it ID must correspond to a registered id )
 **/
int LogicManager::updateSwitchConfiguration(FootSwitch *nswitch){

    if ( m_status ){
            
    id2_t nid = nswitch->getID();

    // Search for given footswitch inside switch array
    if ( m_footSwitchArrayID.find(nid) != m_footSwitchArrayID.end() ){

        // Get correct footswitch
        FootSwitch* oswitch = m_footSwitchArrayID[nid];
        oswitch->clearTargets();

        // Copy it targets
        std::vector<id2_t> targets = nswitch->getTargets();
        
        for ( auto & t : targets ){

            oswitch->addTarget(t);
        }

        // Change it mode
        oswitch->setMode(nswitch->getMode());
    }
    else{

        printf("Logic Manager : Error : Can't update switch configuration : ID( 0x%04x ) not found\n"
            ,nid);
        return 1;
    }

    }
    return 0;
}

/**
 * Clear all footswitches configurations ( target lists )
 **/
void LogicManager::clearManager(){

    if ( m_status ){
                
        // Clear footswitches configuration
        for ( auto & cur : m_footSwitchArrayID ){

            cur.second->clearTargets();
        }
    }
}
