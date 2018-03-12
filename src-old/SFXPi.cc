/**********************************************************************
 * @file SFXPi.cc
 * @author TomatoPi
 * @version 2.0
 *
 * Main
 **********************************************************************/
#include "SFXPi.h"

using namespace SFXP;

#ifdef __DEBUG__
bool SFXP::GlobalIsDebugEnabled = true;
#else
bool SFXP::GlobalIsDebugEnabled = false;
#endif
bool SFXP::GlobalNeedToExit     = false;
int  SFXP::GlobalErrState       = 0;

int main(int argc, char* argv[]){

    cout << endl <<
    "******************************************************************"
    << endl <<
    "******************************************************************"
    << endl <<
    "   SFX-Pi : The Embedded System"
    << endl <<
    "   Program Version : ";
    cout.width(5); cout << left << SFXP::VERSION << endl <<
    "   Build : " << SFXP::BUILD << endl <<
    "******************************************************************"
    << endl <<
    "******************************************************************"
    << endl << endl;

    // REGISTER COMMANDS

    // General Commands
    CommandFactory::registerCommand(new CommandFactory::CmdHelp()   );
    CommandFactory::registerCommand(new CommandFactory::CmdPrevCmd());
    CommandFactory::registerCommand(new CommandFactory::CmdDebug()  );
    
//    std::cout << std::endl;

    // Process Graph edition
    CommandFactory::registerCommand(new CmdAddEffect()       );
    CommandFactory::registerCommand(new CmdRemoveEffect()    );
    CommandFactory::registerCommand(new CmdAddConnection()   );
    CommandFactory::registerCommand(new CmdRemoveConnection());
    
//    std::cout << std::endl;

    // Process graph View
    CommandFactory::registerCommand(new CmdShowGraph()       );
    CommandFactory::registerCommand(new CmdClearGraph()      );
    CommandFactory::registerCommand(new CmdListAvailable()   );
    
//   std::cout << std::endl;

    // Effects Edition and View
    CommandFactory::registerCommand(new CmdShowEffect()  );
    CommandFactory::registerCommand(new CmdAddBank()     );
    CommandFactory::registerCommand(new CmdRemoveBank()  );
    CommandFactory::registerCommand(new CmdEditParam()   );
    CommandFactory::registerCommand(new CmdShowParam()   );
    CommandFactory::registerCommand(new CmdShowPool()    );
    CommandFactory::registerCommand(new CmdChangeBank()  );
/*
    std::cout << std::endl;

    // Presset Managment
    CommandFactory::registerCommand(new CmdListPreset()  );
    CommandFactory::registerCommand(new CmdSavePreset()  );
    CommandFactory::registerCommand(new CmdLoadPreset()  );
    CommandFactory::registerCommand(new CmdSaveEffect()  );
    CommandFactory::registerCommand(new CmdLoadEffect()  );
*/
//    std::cout << std::endl;

    // Command Manager
    CommandFactory::registerCommand(new CmdPrintCMDManager() );
    CommandFactory::registerCommand(new CmdAddSequencer()    );
    CommandFactory::registerCommand(new CmdDelSequencer()    );
    
//    std::cout << std::endl;
    
    CommandFactory::registerCommand(new CmdPrintSequencer()  );
    CommandFactory::registerCommand(new CmdAddSeqLink()      );
    CommandFactory::registerCommand(new CmdDelSeqLink()      );
    CommandFactory::registerCommand(new CmdAddSeqCmd()       );
    CommandFactory::registerCommand(new CmdDelSeqCmd()       );
    
//    std::cout << std::endl;

    // Footswitch Managment
    CommandFactory::registerCommand(new CmdPrintMCPConfig()      );
    CommandFactory::registerCommand(new CmdPrintLogicManager()   );
    CommandFactory::registerCommand(new CmdAddFootTarget()       );
    CommandFactory::registerCommand(new CmdDelFootTarget()       );
    CommandFactory::registerCommand(new CmdShowFoot()            );

    cout << "CommandFactory : Registered : " << CommandFactory::registrySize()
        << " Commands"<< endl << endl;

    //// DECLARE HANDLERS ////
    PresetHandler* PRESET_HANDLER = nullptr;

    //// BUILD PROGRAM ////
    try {

        // START UI MANAGER
        UIManager::create();
    
        cout << endl;

        // REGISTER PLUGIN LIST
        if (EffectFactory::buildRegistry())
            throw string("Failed Build Pugin Registry");
    
        cout << endl;
        
        // BUILD PROCESS GRAPH
        if (ProcessGraph::buildProcessGraph())
            throw string("Failed Build Process Graph");

        cout << endl <<
        "******************************************************************"
        << endl <<
        "   Progam Setup Is OK : Let's get Started Mudda Fukkazz"
        << endl <<
        "******************************************************************"
        << endl << endl;
    }
    catch (const string & e) {

        cout << "SFXPi : FATAL ERROR : " << e << endl;
        cout << "SFXPi : Abort Program Launch" << endl;

        SFXP::GlobalNeedToExit = true;
        SFXP::GlobalErrState = 1;
    }

    // MAIN LOOP
    while (!SFXP::GlobalNeedToExit){

        // Look for command to perform
        std::string cmd = CommandListener::getBuffer();
        if ( cmd.size() != 0 ){

            CommandFactory::performCommand(Parser::parseSimpleString( cmd, " ", '"' ));
            CommandListener::clearBuffer();
        }

        UIManager::update();
        
        // Sleep for 10ms ( 100Hz )
        usleep(10000);
    }

    cout << endl <<
    "******************************************************************"
    << endl <<
    "   Progam End : Deconstruct Everything"
    << endl <<
    "******************************************************************"
    << endl << endl;

    // TELL THE PROGRAM TO STOP
    SFXP::GlobalNeedToExit = true;

    // DESTROY UI MANAGER
    UIManager::kill();

    cout << endl;

    // DESTROY PROCESS GRAPH
    ProcessGraph::killProcessGraph();

    cout << endl <<
    "******************************************************************"
    << endl <<
    "   Progam End : GoodBye"
    << endl <<
    "******************************************************************"
    << endl << endl;
    
    return SFXP::GlobalErrState;
}
