/**********************************************************************
 * @file DriveBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core Stuff For Disortion / Overdrive / Fuzz effect
 **********************************************************************/
#include "DriveBase.h"
 
AbstractDriveBase::~AbstractDriveBase()=default;

AbstractDriveBase::clip_f AbstractDriveBase::castClipper( AbstractDriveBase::TYPE type ){

    if ( type == HARD ){

        std::cout << "HardClipper" << std::endl;
        return AbstractDriveBase::clipHard;
    }
    else if ( type == SOFT ){

        std::cout << "SoftClipper" << std::endl;
        return AbstractDriveBase::clipSoft;
    }
    std::cout << "Err : HardClipper" << std::endl;
    return AbstractDriveBase::clipHard;
}
