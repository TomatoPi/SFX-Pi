/**********************************************************************
 * @file SFX-Pi.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * Main
 **********************************************************************/
#include "SFX-Pi.h"

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

    Listener::Create();

    UnitFactory::registerEffect( SFXP::TC_DRIVE, DriveEffect::NAME, DriveEffect::BUILDER, DriveEffect::PARNAMES, DriveEffect::PARCOUNT, DriveEffect::PORNAMES, DriveEffect::AI, DriveEffect::AO, DriveEffect::MI, DriveEffect::MO);
    UnitFactory::registerEffect( SFXP::TC_MDRIVE, MatrixDriveEffect::NAME, MatrixDriveEffect::BUILDER, MatrixDriveEffect::PARNAMES, MatrixDriveEffect::PARCOUNT, MatrixDriveEffect::PORNAMES, MatrixDriveEffect::AI, MatrixDriveEffect::AO, MatrixDriveEffect::MI, MatrixDriveEffect::MO);
    
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam Setup Is OK : Let's get Started Mudda Fukkazz         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    while (1){

        std::string cmd = Listener::Get().getBuffer();
        if ( cmd.size() != 0 ){

            std::cout << "Héhéhéhéhé : " << cmd << std::endl;
            Listener::Get().clearBuffer();

            if ( !cmd.compare( "exit" ) ){

                break;
            }
        }
    };
    
    return 0;
}
