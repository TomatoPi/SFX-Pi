/**********************************************************************
 * @file SFX-Pi.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * Main
 **********************************************************************/
#include "SFX-Pi.h"

bool SFXP::GlobalNeedToExit = false;
bool SFXP::GlobalIsDebugEnabled = false;

int main(int argc, char* argv[]){

    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** SFX-Pi : The Embedded System                                 **"
    << std::endl <<
    "******************************************************************"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    /* **** Setup Command Listening **** */

    // General Commands
    CommandParser::registerCommand( 0, new CommandParser::CmdHelp()     );
    CommandParser::registerCommand(22, new CommandParser::CmdAlias()    );
    CommandParser::registerCommand( 1, new CommandParser::CmdPrevCmd()  );
    CommandParser::registerCommand( 2, new CommandParser::CmdDebug()    );
    
    std::cout << std::endl;

    // Process Graph edition
    CommandParser::registerCommand( 3, new CmdAddEffect()       );
    CommandParser::registerCommand( 4, new CmdRemoveEffect()    );
    CommandParser::registerCommand( 5, new CmdAddConnection()   );
    CommandParser::registerCommand( 6, new CmdRemoveConnection());
    
    std::cout << std::endl;

    // Process graph View
    CommandParser::registerCommand( 7, new CmdShowGraph()       );
    CommandParser::registerCommand( 8, new CmdClearGraph()      );
    CommandParser::registerCommand( 9, new CmdListAvailable()   );
    
    std::cout << std::endl;

    // Effects Edition and View
    CommandParser::registerCommand(10, new CmdShowEffect()  );
    CommandParser::registerCommand(11, new CmdAddBank()     );
    CommandParser::registerCommand(12, new CmdRemoveBank()  );
    CommandParser::registerCommand(13, new CmdEditParam()   );
    CommandParser::registerCommand(14, new CmdShowParam()   );
    CommandParser::registerCommand(15, new CmdShowPool()    );
    CommandParser::registerCommand(16, new CmdChangeBank()  );
    
    std::cout << std::endl;

    // Presset Managment
    CommandParser::registerCommand(17, new CmdListPreset()  );
    CommandParser::registerCommand(18, new CmdSavePreset()  );
    CommandParser::registerCommand(19, new CmdLoadPreset()  );
    CommandParser::registerCommand(20, new CmdSaveEffect()  );
    CommandParser::registerCommand(21, new CmdLoadEffect()  );
    
    std::cout << std::endl;

    // Command Manager
    CommandParser::registerCommand(23, new CmdPrintCMDManager() );
    CommandParser::registerCommand(24, new CmdAddSequencer()    );
    CommandParser::registerCommand(25, new CmdDelSequencer()    );
    
    std::cout << std::endl;
    
    CommandParser::registerCommand(26, new CmdPrintSequencer()  );
    CommandParser::registerCommand(27, new CmdAddSeqLink()      );
    CommandParser::registerCommand(28, new CmdDelSeqLink()      );
    CommandParser::registerCommand(29, new CmdAddSeqCmd()       );
    CommandParser::registerCommand(30, new CmdDelSeqCmd()       );
    
    std::cout << std::endl;

    // Footswitch Managment
    CommandParser::registerCommand(31, new CmdPrintMCPConfig()      );
    CommandParser::registerCommand(32, new CmdPrintLogicManager()   );
    CommandParser::registerCommand(33, new CmdAddFootTarget()       );
    CommandParser::registerCommand(34, new CmdDelFootTarget()       );
    CommandParser::registerCommand(35, new CmdShowFoot()            );
    
    std::cout << std::endl;
    
    /* **** Setup User Interface **** */

    LogicManager::registerFootSwitch( 0x0000, "PrevBank", 0x0010, 0, 0 );
    LogicManager::registerFootSwitch( 0x0001, "NextBank", 0x0020, 0, 0 );

    std::cout << std::endl;

    /* **** Setup Effect Graph **** */
    
    UnitFactory::registerEffect( SFXP::TC_CAPTURE,  EndUnit::CAPTURE,  EndUnit::BUILDER, EndUnit::PARNAMES, EndUnit::PARCOUNT, EndUnit::PORNAMES, EndUnit::AI, EndUnit::AO, EndUnit::MI, EndUnit::MO);
    UnitFactory::registerEffect( SFXP::TC_PLAYBACK, EndUnit::PLAYBACK, EndUnit::BUILDER, EndUnit::PARNAMES, EndUnit::PARCOUNT, EndUnit::PORNAMES, EndUnit::AI, EndUnit::AO, EndUnit::MI, EndUnit::MO);

    std::cout << std::endl;

    UnitFactory::registerEffect( SFXP::TC_DRIVE,    DriveEffect::NAME,          DriveEffect::BUILDER,       DriveEffect::PARNAMES,          DriveEffect::PARCOUNT,          DriveEffect::PORNAMES,          DriveEffect::AI,        DriveEffect::AO,        DriveEffect::MI,        DriveEffect::MO);
    UnitFactory::registerEffect( SFXP::TC_MDRIVE,   MatrixDriveEffect::NAME,    MatrixDriveEffect::BUILDER, MatrixDriveEffect::PARNAMES,    MatrixDriveEffect::PARCOUNT,    MatrixDriveEffect::PORNAMES,    MatrixDriveEffect::AI,  MatrixDriveEffect::AO,  MatrixDriveEffect::MI,  MatrixDriveEffect::MO);
    
    std::cout << std::endl;
    
    UnitFactory::registerEffect( SFXP::TC_FFTR,     FFTRReg::NAME,              FFTRReg::BUILDER,           FFTRReg::PARNAMES,              FFTRReg::PARCOUNT,              FFTRReg::PORNAMES,              FFTRReg::AI,            FFTRReg::AO,            FFTRReg::MI,            FFTRReg::MO);
    UnitFactory::registerEffect( SFXP::TC_FFTC,     FFTCReg::NAME,              FFTCReg::BUILDER,           FFTCReg::PARNAMES,              FFTCReg::PARCOUNT,              FFTCReg::PORNAMES,              FFTCReg::AI,            FFTCReg::AO,            FFTCReg::MI,            FFTCReg::MO);
    UnitFactory::registerEffect( SFXP::TC_FFTFILTER,FFTFilterReg::NAME,         FFTFilterReg::BUILDER,      FFTFilterReg::PARNAMES,         FFTFilterReg::PARCOUNT,         FFTFilterReg::PORNAMES,         FFTFilterReg::AI,       FFTFilterReg::AO,       FFTFilterReg::MI,       FFTFilterReg::MO);
    UnitFactory::registerEffect( SFXP::TC_FFTDELAY, FFTDelayReg::NAME,          FFTDelayReg::BUILDER,       FFTDelayReg::PARNAMES,          FFTDelayReg::PARCOUNT,          FFTDelayReg::PORNAMES,          FFTDelayReg::AI,        FFTDelayReg::AO,        FFTDelayReg::MI,        FFTDelayReg::MO);

    std::cout << std::endl;

    UnitFactory::registerEffect( SFXP::TC_DELAY,    DelayEffect::NAME,          DelayEffect::BUILDER,       DelayEffect::PARNAMES,          DelayEffect::PARCOUNT,          DelayEffect::PORNAMES,          DelayEffect::AI,        DelayEffect::AO,        DelayEffect::MI,        DelayEffect::MO);
    UnitFactory::registerEffect( SFXP::TC_REVERB,   ReverbEffect::NAME,         ReverbEffect::BUILDER,      ReverbEffect::PARNAMES,         ReverbEffect::PARCOUNT,         ReverbEffect::PORNAMES,         ReverbEffect::AI,       ReverbEffect::AO,       ReverbEffect::MI,       ReverbEffect::MO);
    UnitFactory::registerEffect( SFXP::TC_CHORUS,   ChorusEffect::NAME,         ChorusEffect::BUILDER,      ChorusEffect::PARNAMES,         ChorusEffect::PARCOUNT,         ChorusEffect::PORNAMES,         ChorusEffect::AI,       ChorusEffect::AO,       ChorusEffect::MI,       ChorusEffect::MO);
    
    std::cout << std::endl;

    UnitFactory::registerEffect( SFXP::TC_LFO,      LFOEffect::NAME,            LFOEffect::BUILDER,         LFOEffect::PARNAMES,            LFOEffect::PARCOUNT,            LFOEffect::PORNAMES,            LFOEffect::AI,          LFOEffect::AO,          LFOEffect::MI,          LFOEffect::MO);
    
    std::cout << std::endl;


    
    std::cout << std::endl;

    UIManager::create();
       
    std::cout << std::endl;
    
    ProcessGraph::create();
    
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam Setup Is OK : Let's get Started Mudda Fukkazz         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    /* ********************* Main Loop *********************** */
    while (!SFXP::GlobalNeedToExit){

        // Look for command to perform
        std::string cmd = CommandListener::getBuffer();
        if ( cmd.size() != 0 ){

            CommandParser::performCommand(Parser::parseSimpleString( cmd, " ", '"' ));
            CommandListener::clearBuffer();
        }

        // Update User Interface
        UIManager::update();

        // Monitor Inputs
        EndUnit* end = ProcessGraph::getCapture();
        if ( end->isSaturated(false) ){

            printf("Warning : Left Capture port is Saturated : %2.5f\n",
                end->getMax(false));
                
            end->resetSaturated(false);
        }
        else if ( end->isSaturated(true) ){

            printf("Warning : Right Capture port is Saturated : %2.5f\n",
                end->getMax(true));
                
            end->resetSaturated(true);
        }

        // Monitor Outputs
        end = ProcessGraph::getPlayback();
        if ( end->isSaturated(false) ){
            
            printf("Warning : Left Playback port is Saturated : %2.5f\n",
                end->getMax(false));
                
            end->resetSaturated(false);
        }
        else if ( end->isSaturated(true) ){

            printf("Warning : Right Playback port is Saturated : %2.5f\n",
                end->getMax(true));
                
            end->resetSaturated(true);
        }

        // Sleep for 10ms ( 100Hz )
        usleep(10000);
    };
    /* ******************************************************* */
        
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam End : Deconstruct Everything                          **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    UIManager::kill();

    std::cout << std::endl;

    ProcessGraph::kill();

    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam End : GoodBye                                         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;
    
    return 0;
}
