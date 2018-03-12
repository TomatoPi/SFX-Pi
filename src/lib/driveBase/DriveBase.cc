/**********************************************************************
 * @file DriveBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core Stuff For Disortion / Overdrive / Fuzz effect
 **********************************************************************/
#include "DriveBase.h"

using namespace SFXP;
 
AbstractDriveBase::~AbstractDriveBase()=default;

AbstractDriveBase::clip_f AbstractDriveBase::castClipper( AbstractDriveBase::TYPE type ){

    if ( type == HARD ){

        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled ){
            std::cout << "HardClipper" << std::endl;
        }
        #endif
        return AbstractDriveBase::clipHard;
    }
    else if ( type == SOFT ){

        #ifdef __DEBUG__
        if ( SFXP::GlobalIsDebugEnabled ){
            std::cout << "SoftClipper" << std::endl;
        }
        #endif
        return AbstractDriveBase::clipSoft;
    }
    #ifdef __DEBUG__
    if ( SFXP::GlobalIsDebugEnabled ){
        std::cout << "Err : HardClipper" << std::endl;
    }
    #endif
    return AbstractDriveBase::clipHard;
}
