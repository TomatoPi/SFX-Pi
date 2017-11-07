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
    CommandParser::registerCommand(new CommandParser::CmdHelp());
    
    std::cout << std::endl;
    
    CommandParser::registerCommand(new CmdAddEffect()       );
    CommandParser::registerCommand(new CmdRemoveEffect()    );
    CommandParser::registerCommand(new CmdAddConnection()   );
    CommandParser::registerCommand(new CmdRemoveConnection());
    
    std::cout << std::endl;
    
    CommandParser::registerCommand(new CmdShowGraph()       );
    CommandParser::registerCommand(new CmdClearGraph()      );
    CommandParser::registerCommand(new CmdListAvailable()   );
    
    std::cout << std::endl;

    CommandParser::registerCommand(new CmdShowEffect()  );
    CommandParser::registerCommand(new CmdAddBank()     );
    CommandParser::registerCommand(new CmdRemoveBank()  );
    CommandParser::registerCommand(new CmdEditParam()   );
    CommandParser::registerCommand(new CmdShowParam()   );
    CommandParser::registerCommand(new CmdShowPool()    );
    CommandParser::registerCommand(new CmdChangeBank()  );
    
    std::cout << std::endl;

    CommandParser::registerCommand(new CmdListPreset()  );
    CommandParser::registerCommand(new CmdSavePreset()  );
    CommandParser::registerCommand(new CmdLoadPreset()  );
    CommandParser::registerCommand(new CmdSaveEffect()  );
    CommandParser::registerCommand(new CmdLoadEffect()  );
    
    std::cout << std::endl;
    
    CommandListener::Create();

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
    
    std::cout << std::endl;

    UnitFactory::registerEffect( SFXP::TC_LFO,      LFOEffect::NAME,            LFOEffect::BUILDER,         LFOEffect::PARNAMES,            LFOEffect::PARCOUNT,            LFOEffect::PORNAMES,            LFOEffect::AI,          LFOEffect::AO,          LFOEffect::MI,          LFOEffect::MO);
    
    std::cout << std::endl;
    
    ProcessGraph::Create();
    
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam Setup Is OK : Let's get Started Mudda Fukkazz         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    /* ********************* Main Loop *********************** */
    while (!SFXP::GlobalNeedToExit){

        std::string cmd = CommandListener::Get().getBuffer();
        if ( cmd.size() != 0 ){

            CommandParser::performCommand(Parser::parseSimpleString( cmd, " " ));
            CommandListener::Get().clearBuffer();
        }
    };
    /* ******************************************************* */
        
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam End : Deconstruct Everything                          **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    CommandParser::clearRegister();

    std::cout << std::endl;

    CommandListener::Kill();

    std::cout << std::endl;

    ProcessGraph::Kill();

    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam End : GoodBye                                         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;
    
    return 0;
}
