/**********************************************************************
 * @file PresetParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to read and write preset finary files
 **********************************************************************/
#include "PresetParser.h"

namespace PresetParser{

    /* **** Preset File Flags **** */

    const flag_t flagPreset        = 0xf0; // Preset file Begin

    const flag_t flagEffect         = 0xfa; // Next object is an Effect
    const flag_t flagConnection     = 0xfc; // Next object is a connection
    const flag_t flagFootSwitch     = 0xfd; // Next object is a footswict
    const flag_t flagSequencer      = 0xfe; // Next object is a Command Sequencer

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
 * Function to read and write a Preset File
 **/
int savePreset(std::string file, Preset* preset) {

    if (preset == NULL) {

        cout << "PresetParser : Error : Null Preset Passed" << endl;
        return 1;
    }

    std::string filename = SFXP::DIR_PRST + file;
    std::ofstream flux;

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Save Preset : " << filename << endl;
    
    try {

        flux.open(filename.c_str(), std::ofstream::out | std::ofstream::binary);

        if (flux) {

            // First write Header
            writeHeader(flux, ParserVersion, SFXP::VERSION, flagPreset);

            // Write each Effects
            Preset::EffectList effects = preset->getEffectList();
            for (auto & effect : effects) {

                // Write effect flag
                write<flag_t>(flux, flagEffect);

                // Write the effect
                if (writeEffectPrototype(flux, (EffectPrototype*)(effect.second)))
                    throw string("Failed Write An Effect");
            }

            // Write each Connections
            Preset::ConnectionList connections = preset->getConnectionList();
            for (auto & con : connections) {

                // Write Connection Flag
                write<flag_t>(flux, flagConnection);

                // Write the Connection
                if (writeConnection(flux, con))
                    throw string("Failed Write A Connection");
            }
            
            // Write Each registered Footswitches
            Preset::FootArray fsw = preset->getFootSwitchList();
            for ( auto& f : fsw ){

                // Write FootSwitch Flag
                write<flag_t>(flux, flagFootSwitch);

                // Write the FootSwitch
                writeFootSwitch(flux, f.second);
            }

            // Write each Command Sequencer
            Preset::SequencerList seq = preset->getSequencerList();
            for ( auto& s : seq ){

                // Write Sequencer Flag
                write<flag_t>(flux, flagSequencer);

                // Write Sequencer
                writeSequencer(flux, s.second);
            }
        }
        else throw string("Can't Open File");
        
        flux.close();
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;
        
        return 1;
    }
    catch ( ... ) {

        cout << "PresetParser : Error : Catched Exception while Writing Preset" << endl;
        return 1;
    }
    
    return 0;
}
Preset* loadPreset(std::string file) {

    std::string filename = SFXP::DIR_PRST + file;
    std::ifstream flux;
    Preset* preset = NULL;

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Load Preset : " << filename << endl;
    
    try {

        flux.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

        if (flux) {

            // Read File Header
            if (readHeader(flux, ParserVersion, SFXP::VERSION, flagPreset))
                throw string("Invalid Header");

            preset = new Preset();

            while (flag_t flag = read<flag_t>(flux)) {

                if (flag == flagEffect) {

                    EffectPrototype* effect = readEffectPrototype(flux);

                    if (effect == NULL)
                        throw string("Failed Parse Effect");

                    if (preset->addEffect((AbstractEffectUnit*)effect)) {

                        cout << "PresetParser : Warning : Failed Add Parsed Effect" << endl;
                        delete effect;
                    }
                }
                else if (flag == flagConnection) {

                    Connection c = readConnection(flux);

                    if (c.isInvalid())
                        throw string("Failed Parse Connection");

                    if (preset->addConnection(c)) {
                        
                        cout << "PresetParser : Warning : Failed Add Parsed Connection" << endl;
                    }
                }
                else if (flag == flagFootSwitch) {

                    FootSwitch* f = readFootSwitch(flux);

                    if (f == NULL)
                        throw string("Failed Parse FootSwitch");

                    if (preset->addFootSwitch(f)) {

                        cout << "PresetParser : Warning : Failed Add Parsed FootSwitch" << endl;
                        delete f;
                    }
                }
                else if (flag == flagSequencer) {

                    CommandSequencer* s = readSequencer(flux);

                    if (s == NULL)
                        throw string("Failed Parse CommandSequencer");

                    if (preset->addSequencer(s)) {

                        cout << "PresetParser : Warning : Failed Add Parsed Sequencer" << endl;
                        delete s;
                    }
                }
                else throw string("Unknown Flag");
            }
        }
        else throw string("Can't Open File");
        
        flux.close();
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;

        if (preset) delete preset;
        return NULL;
    }
    catch ( ... ) {

        cout << "PresetParser : Error : Catched Exception while Parsing Preset" << endl;
        
        if (preset) delete preset;
        return NULL;
    }
    
    return preset;
}

/**
 * Function to read and write an Effect File
 **/
int saveEffectPrototype(std::string file, EffectPrototype* effect) {

    std::string filename = SFXP::DIR_PRST + file;
    std::ofstream flux;

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Save Effect : " << filename << endl;
    
    try {

        flux.open(filename.c_str(), std::ofstream::out | std::ofstream::binary);

        if (flux) {

            // Write File Header
            writeHeader(flux, ParserVersion, SFXP::VERSION, flagEffect);

            // Write Effect
            if (writeEffectPrototype(flux, effect))
                throw string("Failed Write Effect Prototype");
        }
        else throw string("Can't Open File");
        
        flux.close();
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;
        
        return 1;
    }
    catch ( ... ) {

        cout << "PresetParser : Error : Catched Exception while Writing Effect" << endl;
        return 1;
    }
    
    return 0;
}
EffectPrototype* loadEffectPrototype(std::string file) {

    std::string filename = SFXP::DIR_PRST + file;
    std::ifstream flux;
    EffectPrototype* effect = NULL;

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Load Effect : " << filename << endl;
    
    try {

        flux.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

        if (flux) {

            // Read File Header
            if (readHeader(flux, ParserVersion, SFXP::VERSION, flagEffect))
                throw string("Invalid Header");

            effect = readEffectPrototype(flux);
            
            if (effect == NULL)
                throw string("Failed Read Effect Prototype");
        }
        else throw string("Can't Open File");
        
        flux.close();
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;

        if (effect) delete effect;
        return NULL;
    }
    catch ( ... ) {

        cout << "PresetParser : Error : Catched Exception while Parsing Effect" << endl;
        
        if (effect) delete effect;
        return NULL;
    }
    
    return effect;
}

/**
 * Function to write File's header
 * Write Parser version, and Program Version and file's type
 **/
void writeHeader(std::ofstream& flux, const std::string& parser, const std::string& program, flag_t type) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write File Header" << endl;
        
    //Write File type
    write<flag_t>(flux, type);

    // Write Program Version
    write<std::string>(flux, program);

    // Write Parser Version
    write<std::string>(flux, parser);
}
/**
 * Function to read File's header
 * Verify that parser version and type flag match
 * return 0 is header is valid
 **/
int readHeader(std::ifstream& flux, const std::string& parser, const std::string& program, flag_t type) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read File Header" << endl;

    try {

        // Verify File Type
        controlValue<flag_t>(flux, type, "Invalid File Type");

        // Verify Program Version
        std::string version = read<std::string>(flux);
        if (version != program)
            cout << "PresetParser : Warning : Invalid Program Version : " << version << endl;

        // Verify Parser Version
        controlValue<std::string>(flux, parser, "Invalid Parser Version");
    }
    catch (const std::string & e) {

        cout << "PresetParser : Error : " << e << endl;
        return 1;
    }
    return 0;
}

/**
 * Parse an EffectPrototype from a binary file
 * return a pointer to created effect if creation is successfull
 * null pointer if not
 * 
 * Binary format is :
 * 
 * >>EffectBeginFlag    ( flag_t )
 * >>Effect Type Code   ( tc_t )
 * >>Effect ID          ( id1_t )
 * >>Number of Banks    ( usize_t )
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
EffectPrototype* readEffectPrototype(std::ifstream& flux) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read Effect Prototype" << endl;

    EffectPrototype* effect = NULL;

    try {

        // Verify Begin Flag
        controlValue<flag_t>(flux, flagEffectBegin, "Invalid Effect Begin Flag");

        // Get type and id
        tc_t type = read<tc_t>(flux);
        id1_t id  = read<id1_t>(flux);

        // Get Number of banks and bank Count
        usize_t count = read<usize_t>(flux);
        usize_t size  = read<usize_t>(flux);

        // Create EffectPrototype
        effect = new EffectPrototype(id, type, size);

        // Get Banks
        for ( usize_t i = 0; i < count; i++ ){

            controlValue<flag_t>(flux, flagBankBegin, "Invalid Bank Begin Flag");

            // Get Bank ID
            id1_t bid = read<id1_t>(flux);
            float bank[size];
            
            // Get all Parameters
            for ( usize_t k = 0; k < size; k++ ){

                bank[k] = read<float>(flux);
            }

            controlValue<flag_t>(flux, flagBankEnd, "Invalid Bank End Flag");

            // Then add new Bank
            if ( effect->addBank( bid, size, bank ) ){

                printf("PresetParser : Warning : Failed Add Parsed Bank\n");
            }
        }

        // Verify last bits
        controlValue<flag_t>(flux, flagEffectEnd, "Invalid Effect End Flag");
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;

        if (effect != NULL) delete effect;
        return NULL;
    }
    return effect;
}
int writeEffectPrototype(std::ofstream& flux, EffectPrototype* effect) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write Effect Prototype" << endl;

    if (effect == NULL){

        cout << "PresetParser : Error : Passed Null effect" << endl;
        return 1;
    }

    // Write effect Begin Flag
    write<flag_t>(flux, flagEffectBegin);

    // Write Effect Type and ID
    write<tc_t>(flux, effect->getType());
    write<id1_t>(flux, effect->getID());

    EffectPrototype::ModBankArray banks = effect->getAllBanks();
    usize_t bankSize = effect->getBankSize();

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
    
    return 0;    
}
    
/**
 * Parse a connection from a binary file
 * Return parsed connection if success
 * connection with source and target id to ErrId1 if not
 *
 * Binary format is :
 *
 * >>ConnectionBeginFlag    ( flag_t )
 *  >>ConnectionSourceID     ( id1_t )
 *  >>ConnectionSourcePort   ( usize_t )
 *  >>ConnectionTargetID     ( id1_t )
 *  >>ConectionTargetPort    ( usize_t )
 *  >>ConnectionType         ( bool )
 * >>ConnectionEndFlag      ( flag_t )
 **/
Connection readConnection(std::ifstream& flux) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read Connection" << endl;

    Connection c;

    try {

        // Verify Begin Flag
        controlValue<flag_t>(flux, flagConnectionBegin, "Invalid Connection Begin Flag");

        // Get datas
        c.m_si = read<id1_t>(flux);
        c.m_sp = read<usize_t>(flux);
        c.m_ti = read<id1_t>(flux);
        c.m_ti = read<usize_t>(flux);
        c.m_midi = read<bool>(flux);

        // Verify End Flag
        controlValue<flag_t>(flux, flagConnnectionEnd, "Invalid Connection End Flag");
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;
        c = Connection();
    }

    return c;
}
int writeConnection(std::ofstream& flux, Connection c) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write Connection" << endl;

    if (c.m_si == SFXP::ErrId1 || c.m_ti == SFXP::ErrId1) {

        cout << "PresetParser : Error : Passed An Invalid Connection : "
            << c << endl;
        return 1;
    }

    // Write Begin Flag
    write<flag_t>(flux, flagConnectionBegin);

    // Write Connection
    write<id1_t>(flux, c.m_si);
    write<usize_t>(flux, c.m_sp);
    write<id1_t>(flux, c.m_ti);
    write<usize_t>(flux, c.m_tp);
    write<bool>(flux, c.m_midi);

    // Write End Flag
    write<flag_t>(flux, flagConnnectionEnd);

    // Flush Stream
    flux.flush();
    
    return 0; 
}

/**
 * Parse a Footswitch Configuration form a binary file
 * Return the parsed Footswitch
 * Null if parsing has failed
 **/
FootSwitch* readFootSwitch(std::ifstream& flux) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read FootSwitch" << endl;

    FootSwitch* fswitch = NULL;

    try {
        
        controlValue<flag_t>(flux, flagFootSwitchBegin, "Invalid FootSwitch Begin Flag");

        // Get Id and Mode and create footswitch pointer
        id2_t id = read<id2_t>(flux);
        id1_t mode = read<id1_t>(flux);
        
        fswitch = new FootSwitch(id, "", static_cast<FootSwitch::Mode>(mode));

        // Get number of targets
        usize_t tc = read<usize_t>(flux);

        // Add Each targets to the switch
        for ( usize_t i = 0; i < tc; i++ ){

            fswitch->addTarget(read<id2_t>(flux));
        }

        controlValue<flag_t>(flux, flagFootSwitchEnd, "Invalid FootSwitch End Flag");
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;
        if (fswitch) delete fswitch;
        return NULL;
    }

    return fswitch;
}
int writeFootSwitch(std::ofstream& flux, FootSwitch *fswitch) {

    if ( SFXP::GlobalIsDebugEnabled )
        printf("PresetParser : Write FootSwitch");

    if ( fswitch == NULL ){

        printf("PresetParser : Error : Null FootSwitch Passed\n");
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
CommandSequencer* parseSequencer(std::ifstream& flux) {

    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read CommandSequencer" << endl;

    CommandSequencer *sequencer = NULL;

    try {
        
        controlValue<flag_t>(flux, flagSequencerBegin, "Invalid CommandSequencer Begin Flag");

        // Read sequencer id
        id2_t id = read<id2_t>(flux);

        // Now build Sequencer
        sequencer = new CommandSequencer(id);

        // Read number of Links
        usize_t linkCount = read<usize_t>(flux);

        // Parse each links
        for ( usize_t i = 0; i < linkCount; i++ ){

            // Verify first bits
            controlValue<flag_t>(flux, flagLinkBegin, "Invalid Link Begin Flag");

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
            controlValue<flag_t>(flux, flagLinkEnd, "Invalid Link End Flag");
        }

        // Read number of command sequencies
        usize_t seqCount = read<usize_t>(flux);

        // Parse each Command
        for ( usize_t i = 0; i < seqCount; i++ ){

            // Verify first bits
            controlValue<flag_t>(flux, flagCommandBegin, "Invalid Command Begin Flag");

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
            controlValue<flag_t>(flux, flagCommandEnd, "Invalid Command End Flag");
        }

        controlValue<flag_t>(flux, flagSequencerEnd, "Invalid CommandSequencer End Flag");
    }
    catch (const std::string& e) {

        cout << "PresetParser : Error : " << e << endl;
        if (sequencer) delete sequencer;
        return NULL;
    }

    return sequencer;
}
int writeSequencer(std::ofstream& flux, CommandSequencer *seq) {

    if ( SFXP::GlobalIsDebugEnabled )
        printf("PresetParser : Write CommandSequencer");

    if ( seq == NULL ){

        printf("PresetParser : Error : Null CommandSequencer Passed\n");
        return 1;
    }

    // Write Sequencer Begin bits
    write<flag_t>(flux, flagSequencerBegin);

    // Write Sequencer ID
    write<id2_t>(flux, seq->getID());

    // Write number of links
    CommandSequencer::ActionMap actMap = seq->getActionList();
    write<usize_t>(flux, actMap.size());

    // Write each Link
    for ( auto & action : actMap ){

        // Write Link Begin bits
        write<flag_t>(flux, flagLinkBegin);

        // Write Link Configuration
        write<id2_t>(flux, action.first.source());
        write<id1_t>(flux, static_cast<id1_t>(action.first.type()));
        write<id1_t>(flux, static_cast<id1_t>(action.second.first));
        write<id2_t>(flux, action.second.second);

        // Write Link end bits
        write<flag_t>(flux, flagLinkEnd);
    }

    // Write number of sequencies
    CommandSequencer::CommandSequenceList seqList = seq->getSequenceList();
    write<usize_t>(flux, seqList.size());

    // Write each Sequence
    for ( auto & sequency : seqList ){

        // Write Sequence begin bits
        write<flag_t>(flux, flagCommandBegin);

        // Write sequence ID and size
        write<id2_t>(flux, sequency.first);
        write<usize_t>(flux, sequency.second.size());

        // Write each command
        for ( auto & command : sequency.second ){

            write<std::string>(flux, command);
        }

        // Write sequence end bits
        write<flag_t>(flux, flagCommandEnd);
    }

    // Write Sequencer end bits
    write<flag_t>(flux, flagSequencerEnd);

    // Flush Stream
    flux.flush();
    
    return 0;
}
}
