/**********************************************************************
 * @file PresetParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to read and write preset finary files
 **********************************************************************/
#include "PresetParser.h"

#define NAME ("PresetParser")

namespace PresetParser{

    /* **** Preset File Flags **** */

    const flag_t flagPreset         = 0xf0; // Preset file Begin
    const flag_t flagPresetEnd      = 0xff; // Preset file End

    const flag_t flagEffect         = 0xfa; // Next object is an Effect
    const flag_t flagConnection     = 0xfc; // Next object is a connection
    const flag_t flagSequencer      = 0xfb; // Next object is a Sequencer
    const flag_t flagAnalogLink     = 0xfd; // Next object is an AnalogLink

    /* **** Effect Object Flags **** */

    const flag_t flagEffectBegin    = 0xa0; // Effect object begin
    const flag_t flagEffectEnd      = 0xaf; // Effect object end

    const flag_t flagBankBegin  = 0xa1; // Effect's bank begin
    const flag_t flagBankEnd    = 0xae; // Effect's bank end

    /* **** Connection Object Flags **** */

    const flag_t flagConnectionBegin    = 0xc0; // Connection object begin
    const flag_t flagConnnectionEnd     = 0xcf; // Connection object end

    /* **** Sequencer Object Flags **** */

    const flag_t flagSequencerBegin     = 0xb0; // Sequencer object begin
    const flag_t flagSequencerEnd       = 0xbf; // Sequencer object end

    const flag_t flagLinkBegin          = 0xb1; // Sequencer's link begin
    const flag_t flagLinkEnd            = 0xb2; // Sequencer's link end

    const flag_t flagEventSeqBegin      = 0xb3; // Event Sequence begin
    const flag_t flagEventSeqEnd        = 0xb4; // Event Sequence end

    /* **** Event Object Flags **** */

    const flag_t flagSFXPEventBegin     = 0x10; // SFXPEvent object begin
    const flag_t flagSFXPEventEnd       = 0x1f; // SFXPEvent object end

    /* **** Potentiometer Flags **** */

    const flag_t flagAnalogLinkBegin    = 0xd0; // AnalogLink begin
    const flag_t flagAnalogLinkEnd      = 0xdf; // AnalogLink end

/**
 * Function to read and write a Preset File
 **/
int savePreset(std::string file, Preset* preset) {

    if (preset == NULL) {

        SFXPlog::err(NAME) << "Null Preset Passed" << endl;
        return 1;
    }

    std::string filename = SFXP::DIR_PRST + file;
    std::ofstream flux;

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Save Preset : " << filename << endl;
    #endif
    
    try {

        flux.open(filename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

        if (flux) {

            // First write Header
            writeHeader(flux, ParserVersion, SFXP::VERSION, flagPreset);

            // Write each ParamSet
            for (auto & effect : preset->_paramSets) {

                // Write effect flag
                write<flag_t>(flux, flagEffect);

                // Write the effect
                if (writeEffectParamSet(flux, effect.second))
                    throw string("Failed Write An Effect");
            }

            // Write each Connections
            for (auto& con : preset->_connections) {

                // Write Connection Flag
                write<flag_t>(flux, flagConnection);

                // Write the Connection
                if (writeConnection(flux, con))
                    throw string("Failed Write A Connection");
            }

            // Write each EventSequencers
            for (auto& seq : preset->_sequencers) {

                // Write Sequencer Flag
                write<flag_t>(flux, flagSequencer);

                // Write the Sequencer
                if (writeSequencer(flux, seq.second))
                    throw string("Failed Write A Sequencer");
            }

            // Write each AnalogLinks
            for (SFXP::usize_t i = 0; i < SFXP::MAX_POT; i++) {

                // If there are links for this pot, save them
                if (preset->_links[i]) {
                for (auto& l : (*preset->_links[i])) {

                    // Write AnalogLink Flag
                    write<flag_t>(flux, flagAnalogLink);

                    // Write the Link
                    if (writeAnalogLink(flux, l))
                        throw string("Failed Write An Analog Link");
                }
                }
            }

            write<flag_t>(flux, flagPresetEnd);
            flux.flush();

        }
        else throw string("Can't Open File");
        
        flux.close();
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << e << endl;
        
        return 1;
    }
    catch ( ... ) {

        SFXPlog::err(NAME) << "Catched Exception while Writing Preset" << endl;
        return 1;
    }
    
    return 0;
}
Preset* loadPreset(std::string file) {

    std::string filename = SFXP::DIR_PRST + file;
    std::ifstream flux;
    Preset* preset = NULL;

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Load Preset : " << filename << endl;
    #endif
    
    try {

        flux.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

        if (flux) {

            // Read File Header
            if (readHeader(flux, ParserVersion, SFXP::VERSION, flagPreset))
                throw string("Invalid Header");

            preset = new Preset();

            while (flag_t flag = read<flag_t>(flux)) {

                if (flag == flagEffect) {

                    ParamSet* effect = readEffectParamSet(flux);

                    if (effect == NULL)
                        throw string("Failed Parse Effect");

                    if (preset->_paramSets.find(effect->id())
                        != preset->_paramSets.end())
                        delete preset->_paramSets[effect->id()];
                    
                    preset->_paramSets[effect->id()] = effect;
                }
                else if (flag == flagConnection) {

                    Connection c = readConnection(flux);

                    if (c.isInvalid())
                        throw string("Failed Parse Connection");

                    preset->_connections.insert(c);
                }
                else if (flag == flagSequencer) {

                    EventSequencer* s = readSequencer(flux);

                    if (s == nullptr)
                        throw string("Failed Parse A Sequencer");

                    if (preset->_sequencers.find(s->id())
                        != preset->_sequencers.end())
                        delete preset->_sequencers[s->id()];

                    preset->_sequencers[s->id()] = s;
                }
                else if (flag == flagAnalogLink) {

                    AnalogLink* l = readAnalogLink(flux);

                    if (!l)
                        throw string("Failed Parse An AnalogLink");

                    if (l->index() < SFXP::MAX_POT) {

                        if (!preset->_links[l->index()])
                            preset->_links[l->index()] = new Preset::ALinkList();

                        preset->_links[l->index()]->push_back(l);
                    }
                    else {

                        SFXPlog::wrn(NAME) << "Invalid AnalogLink Index : "
                            << l->index() << endl;
                        delete l;
                    }
                }
                else if (flag == flagPresetEnd) {

                    break;
                }
                else throw string("Unknown Flag " + std::to_string(flag));
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
 * Function to write File's header
 * Write Parser version, and Program Version and file's type
 **/
void writeHeader(std::ofstream& flux, const std::string& parser, const std::string& program, flag_t type) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write File Header" << endl;
    #endif
        
    //Write File type
    write<flag_t>(flux, type);

    // Write Program Version
    write<std::string>(flux, program);

    // Write Parser Version
    write<std::string>(flux, parser);
    
    flux.flush();
}
/**
 * Function to read File's header
 * Verify that parser version and type flag match
 * return 0 is header is valid
 **/
int readHeader(std::ifstream& flux, const std::string& parser, const std::string& program, flag_t type) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read File Header" << endl;
    #endif

    try {

        // Verify File Type
        controlValue<flag_t>(flux, type, "Invalid File Type");

        // Verify Program Version
        std::string version = read<std::string>(flux);
        if (version.compare(program))
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
ParamSet* readEffectParamSet(std::ifstream& flux) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read Effect Prototype" << endl;
    #endif

    ParamSet* effect = NULL;

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
        effect = new ParamSet(id, type, size);

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
            effect->addBank( bid, size, bank );
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
int writeEffectParamSet(std::ofstream& flux, ParamSet* effect) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write Effect Prototype" << endl;
    #endif

    if (effect == NULL){

        cout << "PresetParser : Error : Passed Null effect" << endl;
        return 1;
    }

    // Write effect Begin Flag
    write<flag_t>(flux, flagEffectBegin);

    // Write Effect Type and ID
    write<tc_t>(flux, effect->type());
    write<id1_t>(flux, effect->id());

    ParamSet::BankList banks = effect->getBanks();
    usize_t bankSize = effect->size();

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

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Read Connection" << endl;
    #endif

    Connection c;

    try {

        // Verify Begin Flag
        controlValue<flag_t>(flux, flagConnectionBegin, "Invalid Connection Begin Flag");

        // Get datas
        c.m_si = read<id1_t>(flux);
        c.m_sp = read<usize_t>(flux);
        c.m_ti = read<id1_t>(flux);
        c.m_tp = read<usize_t>(flux);
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

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        cout << "PresetParser : Write Connection" << endl;
    #endif

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
 * Parse an EventSequencer from a binary flux
 * Return parsed sequencer or null pointer if failed
 *
 * Binary format is :
 * 
 * >>SequencerBeginFlag     ( flag_t )
 *  >>Sequencer ID          ( id2_t )
 * 
 *  >>Number of Links       ( usize_t )
 *  For each Link :
 *      >>LinkBeginFlag     ( flag_t )
 *      >>Trigger           ( IOEvent )
 *      >>ActionType        ( flag_t )
 *      >>TargetSeqID       ( id2_t )
 *      >>LinkEndFlag       ( flag_t )
 * 
 *  >>Number of Sequencies  ( usize_t )
 *  For each Sequence :
 *      >>SequenceBeginFlag ( flag_t )
 *      >>Sequence ID       ( id2_t )
 *      >>Sequence Size     ( usize_t )
 *      List of Events :
 *          >>Events        ( SFXPEvent )
 *      >>SequenceEndFlag   ( flag_t )
 * 
 * >>SequencerEndFlag       ( flag_t )
 **/
EventSequencer* readSequencer(std::ifstream& flux) {

    EventSequencer* seq(nullptr);

    try {

        controlValue<flag_t>(flux, flagSequencerBegin, "Invalid Sequencer Begin Flag");

        // Read ID
        id2_t id = read<id2_t>(flux);
        seq = new EventSequencer(id);

        // Read Links
        usize_t linkCount = read<usize_t>(flux);
        for (usize_t i = 0; i < linkCount; i++) {

            controlValue<flag_t>(flux, flagLinkBegin, "Invalid Link Begin Flag");

            IOEvent e = readIOEvent(flux);
            EventSequencer::Action::Type t =
                static_cast<EventSequencer::Action::Type>(
                    read<flag_t>(flux));
            id2_t tar = read<id2_t>(flux);

            seq->assign(e, t, tar);
            
            controlValue<flag_t>(flux, flagLinkEnd, "Invalid Link End Flag");
        }

        // Read Event Sequencies
        usize_t seqCount = read<usize_t>(flux);
        for (usize_t i = 0; i < seqCount; i++) {

            controlValue<flag_t>(flux, flagEventSeqBegin, "Invalid EventSequence Begin Flag");

            id2_t sid = read<id2_t>(flux);
            usize_t ssize = read<usize_t>(flux);
            for (usize_t k = 0; k < ssize; k++) {

                SFXPEvent* e = readSFXPEvent(flux);
                
                if (!e)
                    throw string("Failed Parse SFXPEvent");
                    
                seq->addEvent(sid, e);
            }
            
            controlValue<flag_t>(flux, flagEventSeqEnd, "Invalid EventSequence End Flag");
        }
        
        controlValue<flag_t>(flux, flagSequencerEnd, "Invalid Sequencer End Flag");
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << e << endl;

        if (seq != nullptr) delete seq;
        return nullptr;
    }
    return seq;
}
int writeSequencer(std::ofstream& flux, EventSequencer* seq) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAME) << "Write Event Sequencer" << endl;
    #endif
    
    if (!seq) {

        SFXPlog::err(NAME) << "Null Event Sequencer Passed" << endl;
        return 1;
    }

    write<flag_t>(flux, flagSequencerBegin);

    write<id2_t>(flux, seq->id());

    EventSequencer::ActionMap links = seq->getActionList();
    write<usize_t>(flux, links.size());
    for (auto& l : links) {

        write<flag_t>(flux, flagLinkBegin);

        writeIOEvent(flux, l.first);
        write<flag_t>(flux, static_cast<flag_t>(l.second._type));
        write<id2_t>(flux, l.second._target);

        write<flag_t>(flux, flagLinkEnd);
    }

    EventSequencer::EventMap events = seq->getSequenceList();
    write<usize_t>(flux, events.size());
    for (auto& e : events) {

        write<flag_t>(flux, flagEventSeqBegin);

        write<id2_t>(flux, e.first);
        write<usize_t>(flux, e.second.size());
        for (auto& s : e.second) {

            writeSFXPEvent(flux, s);
        }
        write<flag_t>(flux, flagEventSeqEnd);
    }

    write<flag_t>(flux, flagSequencerEnd);

    // Flush Stream
    flux.flush();
    
    return 0;
}

/**
 * Parse an event from a binary flux
 *
 * Binary format is :
 *
 * >>EventBeginFlag         ( flag_t )
 * >>EventType              ( flag_t )
 * >>EventTarget            ( flag_t )
 * >>IOEvent                ( IOEvent )
 * >>PresetEvent            ( PresetEvent )
 * >>EffectEvent            ( EffectEvent )
 * >>GraphEvent             ( GraphEvent )
 * >>EditionEvent           ( EditionEvent )
 * >>EVHEvent               ( EVHEvent )
 * >>EventEndFlag           ( flag_t )
 **/
SFXPEvent* readSFXPEvent(std::ifstream& flux) {

    SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_ErrorEvent);

    try {

        controlValue<flag_t>(flux, flagSFXPEventBegin, "Invalid SFXPEvent Begin Flag");

        event->_type = static_cast<SFXPEvent::Type>(read<flag_t>(flux));
        event->_target = static_cast<SFXPEvent::Target>(read<flag_t>(flux));

        // Read Events datas
        event->_io = readIOEvent(flux);
        event->_preset = readPresetEvent(flux);
        event->_effect = readEffectEvent(flux);
        event->_graph = readGraphEvent(flux);
        event->_edit = readEditionEvent(flux);
        event->_evh = readEVHEvent(flux);
        
        controlValue<flag_t>(flux, flagSFXPEventEnd, "Invalid SFXPEvent End Flag");
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << e << endl;
        delete event;
        return nullptr;
    }
    catch (...) {

        delete event;
        return nullptr;
    }
    return event;
}
int writeSFXPEvent(std::ofstream& flux, SFXPEvent* event) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAME) << "Write SFXPEvent" << endl;
    #endif
    
    if (event->_type == SFXPEvent::Type::Event_ErrorEvent) {

        SFXPlog::err(NAME) << "Invalid SXPEvent Passed" << endl;
        return 1;
    }

    write<flag_t>(flux, flagSFXPEventBegin);

    write<flag_t>(flux, static_cast<flag_t>(event->_type));
    write<flag_t>(flux, static_cast<flag_t>(event->_target));

    writeIOEvent(flux, event->_io);
    writePresetEvent(flux, event->_preset);
    writeEffectEvent(flux, event->_effect);
    writeGraphEvent(flux, event->_graph);
    writeEditionEvent(flux, event->_edit);
    writeEVHEvent(flux, event->_evh);

    write<flag_t>(flux, flagSFXPEventEnd);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse an IOEvent data field to a binary flux
 *
 * >>Status     ( int )
 * >>Value      ( float )
 * >>ID         ( id2_t )
 **/
IOEvent readIOEvent(std::ifstream& flux) {

    int s = read<int>(flux);
    float v = read<float>(flux);
    id2_t i = read<id2_t>(flux);

    return IOEvent(s, v, i);
}
int writeIOEvent(std::ofstream& flux, IOEvent event) {

    write<int>(flux, event._status);
    write<float>(flux, event._value);
    write<id2_t>(flux, event._id);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse a PresetEvent data field to a binary flux
 *
 * >>File       ( string )
 **/
PresetEvent readPresetEvent(std::ifstream& flux) {

    return PresetEvent(read<std::string>(flux), nullptr);
}
int writePresetEvent(std::ofstream& flux, PresetEvent event) {

    write<std::string>(flux, event._file);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse an EffectEvent data field to a binary flux
 *
 * >>ID         ( id1_t )
 * >>IDX        ( usize_t )
 * >>Value      ( float )
 **/
EffectEvent readEffectEvent(std::ifstream& flux) {

    id1_t id = read<id1_t>(flux);
    usize_t idx = read<usize_t>(flux);
    float v = read<float>(flux);

    return EffectEvent(id, idx, v);
}
int writeEffectEvent(std::ofstream& flux, EffectEvent event) {

    write<id1_t>(flux, event._id);
    write<usize_t>(flux, event._idx);
    write<float>(flux, event._value);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse a GraphEvent data field to a binary flux
 *
 * >>Connection ( Connection )
 **/
GraphEvent readGraphEvent(std::ifstream& flux) {

    Connection c = readConnection(flux);
    return GraphEvent(c.m_si, c.m_sp, c.m_ti, c.m_tp, c.m_midi);
}
int writeGraphEvent(std::ofstream& flux, GraphEvent event) {

    return writeConnection(flux, Connection(
        event._source, event._sport, event._target, event._tport,
        event._midi ));
}

/**
 * Parse a EditionEvent data field to a binary flux
 *
 * >>ID         ( id1_t )
 * >>Type       ( tc_t )
 **/
EditionEvent readEditionEvent(std::ifstream& flux) {

    id1_t i = read<id1_t>(flux);
    tc_t t = read<tc_t>(flux);

    return EditionEvent(i, t);
}
int writeEditionEvent(std::ofstream& flux, EditionEvent event) {

    write<id1_t>(flux, event._id);
    write<tc_t>(flux, event._type);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse an EVHEvent data field to a binary flux
 *
 * >>ID         ( id2_t )
 * >>Trigger    ( id2_t )
 * >>Status     ( int )
 * >>Action     ( int )
 * >>Target     ( id2_t )
 **/
EVHEvent readEVHEvent(std::ifstream& flux) {

    id2_t id = read<id2_t>(flux);
    id2_t tr = read<id2_t>(flux);
    int   st = read<int>(flux);
    int   a  = read<int>(flux);
    id2_t ta = read<id2_t>(flux);

    return EVHEvent(id, tr, st, a, ta);
}
int writeEVHEvent(std::ofstream& flux, EVHEvent event) {

    write<id2_t>(flux, event._id);
    write<id2_t>(flux, event._trigger);
    write<int>(flux, event._status);
    write<int>(flux, event._action);
    write<id2_t>(flux, event._target);

    // Flush Stream
    flux.flush();

    return 0;
}

/**
 * Parse an AnalogLink from a binary flux
 *
 * Binary format is :
 *
 * >>AnalogBeginFlag    ( flag_t )
 *  >>Index             ( usize_t )
 *  >>Curve             ( flag_t )
 *  >>Tmin              ( float )
 *  >>Tmax              ( float )
 *  >>IsdB              ( bool )
 *  >>IsRelative        ( bool )
 * >>AnalogEndFlag      ( flag_t )
 **/
AnalogLink* readAnalogLink(std::ifstream& flux) {

    AnalogLink* link(nullptr);

    try {

        controlValue<flag_t>(flux, flagAnalogLinkBegin, "Invalid AnalogLink Begin Flag");

        usize_t idx = read<usize_t>(flux);
        link = new AnalogLink(idx);

        utils::Curve curve = static_cast<utils::Curve>(read<flag_t>(flux));
        float tmin = read<float>(flux);
        float tmax = read<float>(flux);
        bool isdb  = read<bool>(flux);
        bool isrlt = read<bool>(flux);

        link->setTransfert(curve, tmin, tmax, isdb, isrlt);

        controlValue<flag_t>(flux, flagAnalogLinkEnd, "Invalid AnalogLink End Flag");
    }
    catch (const std::string& e) {

        SFXPlog::err(NAME) << e << endl;

        if (link) delete link;
        return nullptr;
    }
    return link;
}
int writeAnalogLink(std::ofstream& flux, AnalogLink* link) {

    #ifdef __DEBUG__
    if (SFXP::GlobalIsDebugEnabled)
        SFXPlog::debug(NAME) << "Write Analog Link" << endl;
    #endif
    
    if (!link) {

        SFXPlog::err(NAME) << "Null Analog Link Passed" << endl;
        return 1;
    }

    write<flag_t>(flux, flagAnalogLinkBegin);

    write<usize_t>(flux, link->index());
    write<flag_t>(flux, static_cast<flag_t>(link->curve()));
    write<float>(flux, link->tmin());
    write<float>(flux, link->tmax());
    write<bool>(flux, link->isdb());
    write<bool>(flux, link->isRelative());

    write<flag_t>(flux, flagAnalogLinkEnd);

    // Flush Stream
    flux.flush();

    return 0;
}
}
