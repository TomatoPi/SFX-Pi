/**********************************************************************
 * @file Preset.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset saving stuff
 **********************************************************************/
#include "Preset.h"

namespace{

    /* **** Preset File Flags **** */

    const flag_t flagPressetBegin   = 0xf0; // Preset file Begin
    
    const flag_t flagEffect         = 0xfa; // Next object is an Effect
    const flag_t flagConnection     = 0xfc; // Next object is a connection
    const flag_t flagFootSwitch     = 0xfd; // Next object is a footswict
    const flag_t flagSequencer      = 0xfe; // Next object is a Command Sequencer
    
    const flag_t flagPressetEnd     = 0xff; // Preset file end

    /* **** Effect Object Flags **** */
    
    const flag_t flagEffectBegin    = 0xa0; // Effect object begin
    const flag_t flagEffectEnd      = 0xaf; // Effect object end

    const flag_t flagBankBegin  = 0xa1; // Effect's bank begin
    const flag_t flagBankEnd    = 0xae; // Effect's bank end

    /* **** Connection Object Flags **** */
    
    const flag_t flagConnectionBegin    = 0xc0; // Connection object begin
    const flag_t flagConnnectionEnd     = 0xcf; // Connection object end

    /* **** FootSwitch Object Flags **** */
    
    const flag_t flagFootSwitchBegin    = 0xd0; // Footswtich object begin
    const flag_t flagFootSwitchEnd      = 0xdf; // Footswitch object end

    /* **** Sequencer Object Flags **** */
    const flag_t flagSequencerBegin     = 0xe0; // Sequencer object begin
    const flag_t flagSequencerEnd       = 0xef; // Sequencer object end

    const flag_t flagLinkBegin      = 0xe1; // Sequencer's link begin
    const flag_t flagLinkEnd        = 0xe2; // Sequencer's link end

    const flag_t flagCommandBegin   = 0xe3; // Command Sequence begin
    const flag_t flagCommandEnd     = 0xe4; // Command Sequence end
    
    /**
     * Graph backup while load preset file
     **/
    const std::string TMP_GRAPH = "tmp/bak_graph";

    /* **** Files Extensions **** */
    
    const std::string EXT_PRESET = ".pst";
    const std::string EXT_EFFECT = ".eff";

    /**
     * Funcions to write and read datas to a binary stream
     **/
    template<typename T>
    inline void write(std::ofstream& flux, T bits){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write : %0*x\n", bits, sizeof(bits));
            
        flux.write((char*)&bits, sizeof(T));
    }
    template<typename T>
    inline T read(std::ifstream& flux){

        T bits;
        flux.read((char*)&bits, sizeof(T));

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed : %0*x\n", bits, sizeof(bits));
            
        return bits;
    }

    template<>
    inline void write<std::string>(std::ofstream& flux, std::string text){

        if ( SFXP::GlobalIsDebugEnabled )
            printf("Write String : %s\n", text.c_str());

        write<usize_t>(flux, text.size());
        flux.write(text.c_str(), text.size());
    }

    template<>
    inline std::string read<std::string>(std::ifstream& flux){

        usize_t size = read<usize_t>(flux);
        char buffer[size];
        flux.read(buffer, size);
        
        if ( SFXP::GlobalIsDebugEnabled )
            printf("Readed String : %s\n", buffer);

        return std::string(buffer);
    }
}

int Preset::loadPreset(std::string file, bool bak){

    std::string filename = SFXP::PRESET_PATH + file + EXT_PRESET;

    try{

        std::ifstream flux(filename.c_str());
        printf("Load preset file : %s ... ", filename.c_str());

        if ( flux ){

            bool success = true;

            // Verify begin bits and programm version
            flag_t flag = read<flag_t>(flux);
            if ( flag != flagPressetBegin ){

                printf("PressedLoad : ERROR : Invalid begin bits : %x\n", flag);
                flux.close();
                return 1;
            }

            id1_t version = read<id1_t>(flux);
            if ( version != SFXP::VERSION ){

                printf("PressedLoad : ERROR : Invalid program Version : %u vs %u\n", version, SFXP::VERSION);
                flux.close();
                return 1;
            }

            // Backup and clear process graph
            if ( !bak ){
                if ( savePreset( TMP_GRAPH ) ){

                    printf("PressedLoad : ERROR : Failed backup process Graph\n");
                    flux.close();
                    return 1;
                }
            }
            ProcessGraph::clearGraph();

            // Read each effects contained inside file
            while( flag_t flag = read<flag_t>(flux) ){
                
                if ( SFXP::GlobalIsDebugEnabled )
                    printf("Parsed Flag : 0x%02x\n", flag);

                // If flag is effect flag
                if ( flag == flagEffect ){
                    
                    if ( SFXP::GlobalIsDebugEnabled )
                        printf("\nLoad Effect Unit ...\n");

                    AbstractEffectUnit* unit = parseEffectUnit(flux);

                    if ( unit == NULL ){

                        printf("PressedLoad : ERROR : Failed parse effect\n");
                        success = false;
                        break;
                    }

                    if ( ProcessGraph::addEffect(unit) ){

                        printf("PressetLoad : Warning : Failed add Parsed effect to Graph\n");
                        delete unit;
                    }
                }
                // If flag is connection flag
                else if ( flag == flagConnection ){
                    
                    if ( SFXP::GlobalIsDebugEnabled )
                        printf("\nLoad Connection ...\n");

                    ProcessGraph::Connection con = parseConnection(flux);
                    if ( con.m_si == 255 && con.m_sp == 255 ){

                        printf("PressedLoad : ERROR : Failed parse connection\n");
                        success = false;
                        break;
                    }

                    if ( ProcessGraph::addConnection(con) ){

                        printf("PressetLoad : Warning : Failed add Parsed connection to Graph\n");
                    }
                }
                // If flag is FootSwitch flag
                else if ( flag == flagFootSwitch ){
                    
                    if ( SFXP::GlobalIsDebugEnabled )
                        printf("\nLoad FootSwitch ...\n");

                    FootSwitch *fswitch = parseFootSwitch(flux);

                    if ( fswitch == NULL ){

                        printf("PresetLoad : Error : Failed Parse FootSwitch\n");
                        success = false;
                        break;
                    }

                    if ( LogicManager::updateSwitchConfiguration(fswitch) ){

                        printf("PresetLoad : Warning : Failed add Parsed FootSwitch\n");
                    }
                    
                    // updateSwitchConfiguration only copy configuration
                    // fswitch is no longer used : delete it
                    delete fswitch;
                }
                // If flag is Sequencer flag
                else if ( flag == flagSequencer ){
                    
                    if ( SFXP::GlobalIsDebugEnabled )
                        printf("\nLoad Command Sequencer ...\n");

                    CommandSequencer *sequencer = parseSequencer(flux);

                    if ( sequencer == NULL ){

                        printf("PresetLoad : Error : Failed parse Sequencer\n");
                        success = false;
                        break;
                    }

                    if ( CommandManager::addSequencer(sequencer) ){

                        printf("PresetLoad : Warning : Failed add parsed sequencer\n");
                        delete sequencer;
                    }
                }
                else if ( flag == flagPressetEnd ){
                    
                    if ( SFXP::GlobalIsDebugEnabled )
                        printf("\nPresset File End\n\n");

                    break;
                }
                // If flag doesn't correspond to anything an error occured
                else{

                    printf("PressedLoad : ERROR : Invalid bits : %02x\n", flag);
                    success = false;
                    break;
                }
            }
            // while end

            // If parsing failed
            if ( !success ){

                // if already loading backup
                if ( bak ){

                    printf("PressedLoad : FATAL ERROR : Failed get back process graph\n");
                    flux.close();
                    return 1;
                }
                else{

                    loadPreset( TMP_GRAPH, true );
                }
            }
        }
        else{

            printf("Error : Can't open file\n");
            return 1;
        }
        
        flux.close();
        printf("Preset successfully loaded\n");
    }
    catch( ... ){

        printf("PressedLoad : ERROR : Catched exception while parsing file\n");
    }
    return 0;
}
int Preset::savePreset(std::string file){

    std::string filename = SFXP::PRESET_PATH + file + EXT_PRESET;

    try{

        std::ofstream flux(filename.c_str());
        printf("Save preset file : %s ... ", filename.c_str());

        if ( flux ){

            // Write Preset file begin bits
            write<flag_t>(flux, flagPressetBegin);

            // Write program version
            write<id1_t>(flux, SFXP::VERSION);

            // Write Each Effects inside Process Graph
            std::vector<id1_t> ids = ProcessGraph::getEffectList();

            if ( SFXP::GlobalIsDebugEnabled )
                printf("\n\nSave Effects ( %lu to save ) : \n\n", ids.size());
                
            for ( auto& cur : ids ){

                // Write effect flag
                write<flag_t>(flux, flagEffect);

                // Write effect
                writeEffectUnit(flux, ProcessGraph::getEffect(cur));
            }

            // Write Each connections inside Process Graph
            std::vector<ProcessGraph::Connection> con = ProcessGraph::getConnection();

            if ( SFXP::GlobalIsDebugEnabled )
                printf("\nSave Connections ( %lu to save ) : \n\n", con.size());
                
            for ( auto& cur : con ){

                // Write Connection flag
                write<flag_t>(flux, flagConnection);

                // Write Connection
                writeConnection(flux, cur);
            }

            // Write Each registered Footswitches
            std::vector<id2_t> fsw = LogicManager::getFootSwitchList();

            if ( SFXP::GlobalIsDebugEnabled )
                printf("\nSave FootSwitchs ( %lu to save ) : \n\n", fsw.size());
                
            for ( auto& cur : fsw ){

                write<flag_t>(flux, flagFootSwitch);

                writeFootSwitch(flux, LogicManager::getFootSwitch(cur));
            }

            // Write each Command Sequencer
            std::vector<id2_t> seq = CommandManager::getSequencerList();

            if ( SFXP::GlobalIsDebugEnabled )
                printf("\nSave Command Sequencers ( %lu to save ) : \n\n", seq.size());
                
            for ( auto& cur : seq ){

                write<flag_t>(flux, flagSequencer);

                writeSequencer(flux, CommandManager::getSequencer(cur));
            }
            
            // Write Presset file end bit
            write<flag_t>(flux, flagPressetEnd);
        }
        else{

            printf("Error : Can't open file\n");
            return 1;
        }
        
        flux.close();
        printf("Preset successfully saved\n");
    }
    catch( ... ){

        printf("PressetSave : ERROR : Catched exception while parsing file\n");
        return 1;
    }
    return 0;
}

AbstractEffectUnit* Preset::loadEffect(std::string file){
    
    std::string filename = SFXP::PRESET_PATH + file + EXT_EFFECT;
    AbstractEffectUnit* unit = NULL;

    try{

        std::ifstream flux(filename.c_str());
        printf("Load effect file : %s ... ", filename.c_str());

        if ( flux ){

            unit = parseEffectUnit(flux);
            
            if ( unit == NULL ){

                printf("Error : Failed read effect file\n");
                flux.close();
                return unit;
            }
        }
        else{

            printf("Error : Can't open file\n");
            return unit;
        }
        
        flux.close();
        printf("Effect successfully loaded\n");
    }
    catch( ... ){

        printf("EffectLoad : ERROR : Catched exception while parsing file\n");
        return unit;
    }
    return unit;
}

int Preset::saveEffect(std::string file, AbstractEffectUnit* unit){

    std::string filename = SFXP::PRESET_PATH + file + EXT_EFFECT;

    try{

        std::ofstream flux(filename.c_str());
        printf("Save effect file : %s ... ", filename.c_str());

        if ( flux ){

            if ( writeEffectUnit(flux, unit) ){

                printf("Error : Failed write effect file\n");
                flux.close();
                return 1;
            }
        }
        else{

            printf("Error : Can't open file\n");
            return 1;
        }
        
        flux.close();
        printf("Preset successfully saved\n");
    }
    catch( ... ){

        printf("EffectSave : ERROR : Catched exception while parsing file\n");
        return 1;
    }
    return 0;
}

/**
 * Parse an effect unit from a binary file
 * return a pointer to created effect unit if creation is successfull
 * null pointer if not
 * 
 * Binary format is :
 * 
 * >>EffectBeginFlag    ( flag_t )
 * >>Effect Type Code   ( id1_t )
 * >>Effect ID          ( id1_t )
 * >>Number of Banks    ( usize_t : must be changed to platform independant type )
 * >>Size of a Bank     ( usize_t )
 * 
 * For each Bank :
 *   >>BankBeginFlag      ( flag_t )
 *   >>Bank's ID          ( id1_t )
 *   >>All Banks Params   ( Size of a bank : float )
 *   >>BankEndFlag        ( flag_t )
 * 
 * >>EffectEndFlag      ( flag_t )
 **/
AbstractEffectUnit* Preset::parseEffectUnit(std::ifstream& flux){

    AbstractEffectUnit* unit = NULL;
    bool success = true;

    flag_t flag;

    //Verify that first bits are effect begin bits
    flag = read<flag_t>(flux);
    if ( flag != flagEffectBegin ){

        printf("EffectParsing : ERROR : Invalid Start Bit : %0*x\n", flag, sizeof(flag_t));
        return NULL;
    }

    // Read Type and ID
    id1_t type, id;
    type = read<id1_t>(flux);
    id   = read<id1_t>(flux);

    unit = UnitFactory::createEffect( type, id );
    if ( unit == NULL ){

        printf("EffectParsing : ERROR : Invalid Effect Type : %x\n", type);
        return NULL;
    }
    
    usize_t size;
    usize_t count;
    count = read<usize_t>(flux);
    size  = read<usize_t>(flux);

    // Read Each Banks
    id1_t curID;
    float curVal[size];

    for ( usize_t i = 0; i < count; i++ ){

        // Verify that first bit are bank begin
        flag = read<flag_t>(flux);
        if ( flag != flagBankBegin ){

            success = false;
            printf("EffectParsing : ERROR : Bank starting bit Invalid : %0*x\n", flag, sizeof(flag_t));
            break;
        }

        // Get Bank id
        curID = read<id1_t>(flux);

        // Get all Parameters
        for ( usize_t k = 0; k < size; k++ ){

            curVal[k] = read<float>(flux);
        }

        // Verify that last bits are bank end
        flag = read<flag_t>(flux);
        if ( flag != flagBankEnd ){

            success = false;
            printf("EffectParsing : ERROR : Bank Ending bit Invalid : %0*x\n", flag, sizeof(flag_t));
            break;
        }

        // Then add new Bank
        if ( unit->addBank( curID, size, curVal ) ){

            printf("EffectParsing : Warning : Failed Add Parsed Bank\n");
        }
    }

    // Verify that last bit are effect End
    flag = read<flag_t>(flux);
    if ( flag != flagEffectEnd ){

        success = false;
        printf("EffectParsing : ERROR : Effect end bits are Invalid : %0*x\n", flag, sizeof(flag_t));
    }

    if ( !success ){

        printf("EffectParsing : Effect Parsing has Failed, Return NULL\n");
        delete unit;
        unit = NULL;
    }
    
    return unit;
}
int Preset::writeEffectUnit(std::ofstream& flux, AbstractEffectUnit* unit){

    if ( unit == NULL ){

        printf("EffectWriting : ERROR : null effect passed\n");
        return 1;
    }
    
    if ( SFXP::GlobalIsDebugEnabled )
        printf("Save Given Effect ... ");

    //Write effect begin bits
    write<flag_t>(flux, flagEffectBegin);

    // Write Effect Type and ID
    write<id1_t>(flux, unit->getType());
    write<id1_t>(flux, unit->getID());

    std::map<id1_t,float*> banks = unit->getAllBanks();
    usize_t bankSize = UnitFactory::getParamCount(unit->getType());

    // Write banks count
    write<usize_t>(flux, banks.size());

    // Write number of parameters inside a bank
    write<usize_t>(flux, bankSize);

    // Write each Banks
    for ( auto& cur : banks ){

        // Write Bank Begin Bits
        write<flag_t>(flux, flagBankBegin);

        // Write Bank's ID
        write<id1_t>(flux, cur.first);

        // Write Params
        for( usize_t i = 0; i < bankSize; i++ ){
            
            write<float>(flux, cur.second[i]);
        }

        // Write Bank End Bits
        write<flag_t>(flux, flagBankEnd);
    }

    // Write Effect End Bits
    write<flag_t>(flux, flagEffectEnd);

    // Flush Stream
    flux.flush();
    
    if ( SFXP::GlobalIsDebugEnabled )
        printf("Done\n");

    return 0;
}

/**
 * Parse a connection from a binary file
 * Return parsed connection if success
 * 255 255 255 255 if not
 *
 * Binary format is :
 *
 * >>ConnectionBeginFlag    ( flag_t )
 * >>ConnectionSourceID     ( id1_t )
 * >>ConnectionSourcePort   ( id1_t )
 * >>ConnectionTargetID     ( id1_t )
 * >>ConectionTargetPort    ( id1_t )
 * >>ConnectionEndFlag      ( flag_t )
 **/
ProcessGraph::Connection Preset::parseConnection(std::ifstream& flux){

    ProcessGraph::Connection rtn(255,255,255,255, 0);
    bool success = true;
    flag_t flag;
    
    // Verify first bits
    flag = read<flag_t>(flux);
    success = flag == flagConnectionBegin;

    // Get connection back
    if ( success ){
            
        rtn.m_si = read<id1_t>(flux);
        rtn.m_sp = read<id1_t>(flux);
        rtn.m_ti = read<id1_t>(flux);
        rtn.m_tp = read<id1_t>(flux);
        rtn.m_midi = read<id1_t>(flux);
    }

    // Verify End bits
    flag = read<flag_t>(flux);
    success = flag == flagConnnectionEnd;

    // Return created Connection
    if ( success )
        return rtn;
    return ProcessGraph::Connection(255, 255, 255, 255, 0);
}
int Preset::writeConnection(std::ofstream& flux, ProcessGraph::Connection c){

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Save Given Connection ... ");

    // Write Connection Begin Bits
    write<flag_t>(flux, flagConnectionBegin);

    // Write Connection Datas
    write<id1_t>(flux, c.m_si);
    write<id1_t>(flux, c.m_sp);
    write<id1_t>(flux, c.m_ti);
    write<id1_t>(flux, c.m_tp);
    write<id1_t>(flux, c.m_midi);

    // Write Connection End Bits
    write<flag_t>(flux, flagConnnectionEnd);

    // Flush Stream
    flux.flush();

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Done\n");

    return 0;
}

/**
 * Parse a Footswitch Configuration form a binary file
 * Return the parsed Footswitch
 * Null if parsing has failed
 *
 * Binary format is :
 *
 * >>FootSwitchBeginFlag    ( flag_t )
 * >>FootSwitch's ID        ( id2_t )
 * >>FootSwitch's Mode      ( id1_t )
 * >>Number of Targets      ( usize_t )
 *
 * For each target :
 *   >>Target's ID          ( id2_t )
 *
 * >>FootSwitchEndFlag      ( flag_t )
 **/
FootSwitch* Preset::parseFootSwitch(std::ifstream& flux){

    FootSwitch* fswitch = NULL;
    flag_t flag;

    // Verify first bits
    flag = read<flag_t>(flux);
    if ( flag != flagFootSwitchBegin ){

        printf("FootSwitch : Parsing Error : Begin bits Invalids : %0*x\n", flag, sizeof(flag_t));
        return NULL;
    }

    // Get Id and Mode and create footswitch pointer
    id2_t id = 0;
    id1_t mode = 0;

    id   = read<id2_t>(flux);
    mode = read<id1_t>(flux);

    fswitch = new FootSwitch(id, "", static_cast<FootSwitch::Mode>(mode));

    // Get number of targets
    usize_t tc = read<usize_t>(flux);

    // Add Each targets to the switch
    for ( usize_t i = 0; i < tc; i++ ){

        fswitch->addTarget(read<id2_t>(flux));
    }

    // Verify end bits
    flag = read<flag_t>(flux);
    if ( flag != flagFootSwitchEnd ){

        printf("FootSwitch : Parsing Error : End bits Invalids : %0*x\n", flag, sizeof(flag_t));
        
        delete fswitch;
        return NULL;
    }

    return fswitch;
}
int Preset::writeFootSwitch(std::ofstream& flux, FootSwitch *fswitch){

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Save Given FootSwitch ... ");

    if ( fswitch == NULL ){

        printf("FootSwitch : Writing Error : Null pointer passed\n");
        return 1;
    }

    // Write footswitch begin bits
    write<flag_t>(flux, flagFootSwitchBegin);

    // Write footswitch ID and Mode
    write<id2_t>(flux, fswitch->getID());
    write<id1_t>(flux, static_cast<id1_t>(fswitch->getMode()));

    //Get target list
    std::vector<id2_t> targets = fswitch->getTargets();

    //Write number of targets then write them
    write<usize_t>(flux, targets.size());
    
    for ( auto & t : targets ){

        write<id2_t>(flux, t);
    }

    // Write footswitch end bits
    write<flag_t>(flux, flagFootSwitchEnd);

    // Flush Stream
    flux.flush();

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Done\n");
    
    return 0;
}

/**
 * Parse a Command Sequencer from a binary file
 * Return Parsed Command Sequencer
 * Null if parsing has failed
 *
 * Binary format is :
 *
 * >>SequencerBeginFlag     ( flag_t )
 * >>Sequencer ID           ( id2_t )
 * >>Number Of Links        ( usize_t )
 *
 * For each Link :
 *   >>LinkBeginFlag        ( flag_t )
 *   >>Notification Source  ( id2_t )
 *   >>Notification Type    ( id1_t )
 *   >>Action Type          ( id1_t )
 *   >>Action Argument      ( id2_t )
 *   >>LinkEndFlag          ( flag_t )
 *
 * >>Number of Sequencies   ( usize_t )
 *
 * For each Sequence :
 *   >>SequenceBeginFlag    ( flag_t )
 *   >>Sequence ID          ( id2_t )
 *   >>Sequence Size        ( usize_t )
 *   For each Command :
 *     >>Command Size       ( usize_t )
 *     >>Command            ( std::string : Command Size )
 *   >>SequenceEndFlag      ( flag_t )
 *
 * >>SequencerEndFlag       ( flag_t )
 **/
CommandSequencer* Preset::parseSequencer(std::ifstream& flux){

    CommandSequencer *sequencer = NULL;
    bool success = true;

    flag_t flag;

    // Verify that first bits are command sequencer begin bits
    flag= read<flag_t>(flux);
    if ( flag != flagSequencerBegin ){

        printf("Sequencer : Parsing Error : Invalid start bits : %0*x\n", flag, sizeof(flag_t));
        return NULL;
    }

    // Read sequencer id
    id2_t id = read<id2_t>(flux);

    // Now build Sequencer
    sequencer = new CommandSequencer(id);

    // Read number of Links
    usize_t linkCount = read<usize_t>(flux);

    // Parse each links
    for ( usize_t i = 0; i < linkCount; i++ ){

        // Verify first bits
        flag = read<flag_t>(flux);
        if ( flag != flagLinkBegin ){

            success = false;
            printf("Sequencer : Parsing Error : Link Starting invalid bits : %0*x\n", flag, sizeof(flag_t));
            break;
        }

        // Get Notif Source, Notif Type, Action Type, Action Argument

        id2_t ns = read<id2_t>(flux);
        id1_t nt = read<id1_t>(flux);
        id1_t at = read<id1_t>(flux);
        id2_t aa = read<id2_t>(flux);

        sequencer->assign(
            ns
            , static_cast<Notification::NotificationType>(nt)
            , static_cast<CommandSequencer::ActionT>(at)
            , aa
            );

        // Verify last bits
        flag = read<flag_t>(flux);
        if ( flag != flagLinkEnd ){

            success = false;
            printf("Sequencer : Parsing Error : Link end invalid bits : %0*x\n", flag, sizeof(flag_t));
            break;
        }
    }

    // Read number of command sequencies
    usize_t seqCount = read<usize_t>(flux);

    // Parse each Command
    for ( usize_t i = 0; i < seqCount; i++ ){

        // Verify first bits
        flag = read<flag_t>(flux);
        if ( flag != flagCommandBegin ){

            success = false;
            printf("Sequencer : Parsing Error : Command begin invalid bits : %0*x\n", flag, sizeof(flag_t));
            break;
        }

        // Get Sequence id
        id2_t id = read<id2_t>(flux);

        // Get Sequence size
        usize_t seqSize = read<usize_t>(flux);

        // Get each sequence
        for ( usize_t k = 0; k < seqSize; k++ ){

            // Add command to sequencer
            sequencer->addCommand(id, read<std::string>(flux));
        }

        // Verify end bits
        flag = read<flag_t>(flux);
        if ( flag != flagCommandEnd ){

            success = false;
            printf("Sequencer : Parsing Error : Command end invalid bits : %0*x\n", flag, sizeof(flag_t));
            break;
        }
    }

    // Verify end bits
    flag = read<flag_t>(flux);
    if ( flag != flagSequencerEnd ){

        success = false;
        printf("Sequencer : Parsing Error : end bits invalid : %0*x\n", flag, sizeof(flag_t));
    }

    if ( success ){

        return sequencer;
    }

    delete sequencer;
    return NULL;
}
int Preset::writeSequencer(std::ofstream& flux, CommandSequencer *seq){

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Save Given Command Sequencer ... ");

    if ( seq == NULL ){

        printf("Command Sequencer : Writing Error : Null pointer passed\n");
        return 1;
    }

    // Write Sequencer Begin bits
    write(flux, flagSequencerBegin);

    // Write Sequencer ID
    write(flux, seq->getID());

    // Write number of links
    CommandSequencer::ActionMap actMap = seq->getActionList();
    write(flux, actMap.size());

    // Write each Link
    for ( auto & action : actMap ){

        // Write Link Begin bits
        write(flux, flagLinkBegin);

        // Write Link Configuration
        write(flux, action.first.source());
        write(flux, static_cast<id1_t>(action.first.type()));
        write(flux, static_cast<id1_t>(action.second.first));
        write(flux, action.second.second);

        // Write Link end bits
        write(flux, flagLinkEnd);
    }

    // Write number of sequencies
    CommandSequencer::CommandSequenceList seqList = seq->getSequenceList();
    write(flux, seqList.size());

    // Write each Sequence
    for ( auto & sequency : seqList ){

        // Write Sequence begin bits
        write(flux, flagCommandBegin);

        // Write sequence ID and size
        write(flux, sequency.first);
        write(flux, sequency.second.size());

        // Write each command
        for ( auto & command : sequency.second ){

            write(flux, command);
        }

        // Write sequence end bits
        write(flux, flagCommandEnd);
    }

    // Write Sequencer end bits
    write(flux, flagSequencerEnd);

    // Flush Stream
    flux.flush();

    if ( SFXP::GlobalIsDebugEnabled )
        printf("Done\n");
    
    return 0;
}
