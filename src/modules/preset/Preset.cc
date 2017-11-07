/**********************************************************************
 * @file Preset.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset saving stuff
 **********************************************************************/
#include "Preset.h"

namespace{

    static const uint8_t flagPressetBegin   = 0xf0;
    static const uint8_t flagEffect         = 0xf1;
    static const uint8_t flagConnection     = 0xf2;
    static const uint8_t flagPressetEnd     = 0xff;
    
    static const uint8_t flagEffectBegin    = 0xa0;
    static const uint8_t flagEffectEnd      = 0xaf;

    static const uint8_t flagBankBegin  = 0xb0;
    static const uint8_t flagBankEnd    = 0xbf;

    static const uint8_t flagConnectionBegin    = 0xc0;
    static const uint8_t flagConnnectionEnd     = 0xcf;

    static const std::string TMP_GRAPH = "tmp/bak_graph";
    
    static const std::string EXT_PRESET = ".pst";
    static const std::string EXT_EFFECT = ".eff";
}

int Preset::loadPreset(std::string file, bool bak){

    std::string filename = SFXP::PRESET_PATH + file + EXT_PRESET;

    try{

        std::ifstream flux(filename.c_str());
        printf("Load preset file : %s ... ", filename.c_str());

        if ( flux ){

            bool success = true;
            uint8_t c;

            // Verify begin bits and programm version
            flux.read((char*)&c, sizeof(uint8_t));
            if ( c != flagPressetBegin ){

                printf("PressedLoad : ERROR : Invalid begin bits : %x\n", c);
                flux.close();
                return 1;
            }

            uint8_t version;
            flux.read((char*)&version, sizeof(SFXP::VERSION));
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
            ProcessGraph::Get().clearGraph();

            // Read each effects contained inside file
            while( flux.read((char*)&c, sizeof(uint8_t)) ){

                // If flag is effect flag
                if ( c == flagEffect ){

                    AbstractEffectUnit* unit = parseEffectUnit(flux);

                    if ( unit == NULL ){

                        printf("PressedLoad : ERROR : Failed parse effect\n");
                        success = false;
                        break;
                    }

                    if ( ProcessGraph::Get().addEffect(unit) ){

                        printf("PressetLoad : Warning : Failed add Parsed effect to Graph\n");
                        delete unit;
                    }
                }
                // If flag is connection flag
                else if ( c == flagConnection ){

                    ProcessGraph::Connection con = parseConnection(flux);
                    if ( con.m_si == 255 && con.m_sp == 255 ){

                        printf("PressedLoad : ERROR : Failed parse connection\n");
                        success = false;
                        break;
                    }

                    if ( ProcessGraph::Get().addConnection(con) ){

                        printf("PressetLoad : Warning : Failed add Parsed connection to Graph\n");
                    }
                }
                else if ( c == flagPressetEnd ){

                    break;
                }
                // If flag doesn't correspond to anything an error occured
                else{

                    printf("PressedLoad : ERROR : Invalid bits : %x\n", c);
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
            flux.write((char*)&flagPressetBegin, sizeof(uint8_t));

            // Write program version
            flux.write((char*)&SFXP::VERSION, sizeof(SFXP::VERSION));

            // Write Each Effects inside Process Graph
            std::vector<uint8_t> ids = ProcessGraph::Get().getEffectList();

            for ( auto& cur : ids ){

                // Write effect flag
                flux.write((char*)&flagEffect, sizeof(uint8_t));

                // Write effect
                writeEffectUnit(flux, ProcessGraph::Get().getEffect(cur));
            }

            // Write Each connections inside Process Graph
            std::vector<ProcessGraph::Connection> con = ProcessGraph::Get().getConnection();

            for ( auto& cur : con ){

                // Write Connection flag
                flux.write((char*)&flagConnection, sizeof(uint8_t));

                // Write Connection
                writeConnection(flux, cur);
            }

            // Write Presset file end bit
            flux.write((char*)&flagPressetEnd, sizeof(uint8_t));
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

                printf("Error : Failed write effect file\n");
                flux.close();
                return unit;
            }
        }
        else{

            printf("Error : Can't open file\n");
            return unit;
        }
        
        flux.close();
        printf("Effect successfully saved\n");
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
 **/
AbstractEffectUnit* Preset::parseEffectUnit(std::ifstream& flux){

    AbstractEffectUnit* unit = NULL;
    bool success = true;

    uint8_t c;

    //Verify that first bits are effect begin bits
    flux.read((char*)&c, sizeof(uint8_t));
    if ( c != flagEffectBegin ){

        printf("EffectParsing : ERROR : Invalid Start Bit : %x\n", c);
        return NULL;
    }

    // Read Type and ID
    uint8_t type, id;
    flux.read((char*)&type, sizeof(uint8_t));
    flux.read((char*)&id, sizeof(uint8_t));

    unit = UnitFactory::createEffect( type, id );
    if ( unit == NULL ){

        printf("EffectParsing : ERROR : Invalid Effect Type : %x\n", type);
        return NULL;
    }
    
    uint8_t size;
    size_t count;
    flux.read((char*)&count, sizeof(size_t));
    flux.read((char*)&size, sizeof(uint8_t));

    // Read Each Banks
    uint8_t curID;
    float curVal[size];

    for ( size_t i = 0; i < count; i++ ){

        // Verify that first bit are bank begin
        flux.read((char*)&c, sizeof(uint8_t));
        if ( c != flagBankBegin ){

            success = false;
            printf("EffectParsing : ERROR : Bank starting bit Invalid : %x\n", c);
            break;
        }

        // Get Bank id
        flux.read((char*)&curID, sizeof(uint8_t));

        // Get all Parameters
        for ( uint8_t k = 0; k < size; k++ ){

            flux.read((char*)&curVal[k], sizeof(float));
        }

        // Verify that last bits are bank end
        flux.read((char*)&c, sizeof(uint8_t));
        if ( c != flagBankEnd ){

            success = false;
            printf("EffectParsing : ERROR : Bank Ending bit Invalid : %x\n", c);
            break;
        }

        // Then add new Bank
        if ( unit->addBank( curID, size, curVal ) ){

            printf("EffectParsing : Warning : Failed Add Parsed Bank\n");
        }
    }

    // Verify that last bit are effect End
    flux.read((char*)&c, sizeof(uint8_t));
    if ( c != flagEffectEnd ){

        success = false;
        printf("EffectParsing : ERROR : Effect end bits are Invalid : %x\n", c);
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

    printf("Save Given Effect ... ");

    //Write effect begin bits
    flux.write((char*)&flagEffectBegin, sizeof(uint8_t));

    // Write Effect Type and ID
    uint8_t c = unit->getType();
    flux.write((char*)&c, sizeof(uint8_t));

    c = unit->getID();
    flux.write((char*)&c, sizeof(uint8_t));

    std::map<uint8_t,float*> banks = unit->getAllBanks();
    uint8_t bankSize = UnitFactory::getParamCount(unit->getType());

    // Write banks count
    size_t bc = banks.size();
    flux.write((char*)&bc, sizeof(size_t));

    // Write number of parameters inside a bank
    flux.write((char*)&bankSize, sizeof(uint8_t));

    // Write each Banks
    for ( auto& cur : banks ){

        // Write Bank Begin Bits
        flux.write((char*)&flagBankBegin, sizeof(uint8_t));

        // Write Bank's ID
        flux.write((char*)&(cur.first), sizeof(uint8_t));

        // Write Params
        for( uint8_t i = 0; i < bankSize; i++ ){
            
            flux.write((char*)&(cur.second[i]), sizeof(float));
        }

        // Write Bank End Bits
        flux.write((char*)&flagBankEnd, sizeof(uint8_t));
    }

    // Write Effect End Bits
    flux.write((char*)&flagEffectEnd, sizeof(uint8_t));

    // Flush Stream
    flux.flush();
    printf("Done\n");

    return 0;
}

ProcessGraph::Connection Preset::parseConnection(std::ifstream& flux){

    ProcessGraph::Connection rtn(255,255,255,255);
    bool success = true;
    uint8_t c;
    
    // Verify first bits
    flux.read((char*)&c, sizeof(uint8_t));
    success = c == flagConnectionBegin;

    // Get connection back
    if ( success ){
            
        flux.read((char*)&rtn.m_si, sizeof(rtn.m_si));
        flux.read((char*)&rtn.m_sp, sizeof(rtn.m_sp));
        flux.read((char*)&rtn.m_ti, sizeof(rtn.m_ti));
        flux.read((char*)&rtn.m_tp, sizeof(rtn.m_tp));
    }

    // Verify End bits
    flux.read((char*)&c, sizeof(uint8_t));
    success = c == flagConnnectionEnd;

    // Return created Connection
    return rtn;
}
int Preset::writeConnection(std::ofstream& flux, ProcessGraph::Connection c){

    // Write Connection Begin Bits
    flux.write((char*)&flagConnectionBegin, sizeof(uint8_t));

    // Write Connection Datas
    flux.write((char*)&c.m_si, sizeof(c.m_si));
    flux.write((char*)&c.m_sp, sizeof(c.m_sp));
    flux.write((char*)&c.m_ti, sizeof(c.m_ti));
    flux.write((char*)&c.m_tp, sizeof(c.m_tp));

    // Write Connection End Bits
    flux.write((char*)&flagConnnectionEnd, sizeof(uint8_t));

    // Flush Stream
    flux.flush();

    return 0;
}
