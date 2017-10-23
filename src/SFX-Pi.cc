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

    UnitFactory::registerEffect( SFXP::TC_DRIVE, DriveReg::NAME, DriveReg::BUILDER, DriveReg::PARNAMES, DriveReg::PARCOUNT, DriveReg::PORNAMES, DriveReg::AI, DriveReg::AO, DriveReg::MI, DriveReg::MO);
    
    std::cout << std::endl <<
    "******************************************************************"
    << std::endl <<
    "** Progam Setup Is OK : Let's get Started Mudda Fukkazz         **"
    << std::endl <<
    "******************************************************************"
    << std::endl << std::endl;

    AbstractEffectUnit *test = UnitFactory::createEffect( SFXP::TC_DRIVE, 0);

    const float test2[21] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 300, 1000, 1, 1, 1, 300, 1000, 1, 1, 1};
    test->setParam( 21, test2 );

    test->getJACKUnit()->connect( "system:capture_1", UnitFactory::buildPortName( test, SFXP::TC_DRIVE, 0, 0) );
    test->getJACKUnit()->connect( UnitFactory::buildPortName( test, SFXP::TC_DRIVE, 1, 0), "system:playback_1");
    test->getJACKUnit()->connect( UnitFactory::buildPortName( test, SFXP::TC_DRIVE, 1, 0), "system:playback_2");

    while (1);

    delete test;
    return 0;
}
