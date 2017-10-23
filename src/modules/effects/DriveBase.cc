/**********************************************************************
 * @file DriveBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core Stuff For Disortion / Overdrive / Fuzz effect
 **********************************************************************/
AbstractDriveBase::~AbstractDriveBase()=default;

AbstractDriveBase::clip_f AbstractDriveBase::castClipper( AbstractDriveBase::TYPE type ){

    if ( type == SOFT ){

        return AbstractDriveBase::clipHard;
    }
    else if ( type == HARD ){

        return AbstractDriveBase::clipSoft;
    }
    return AbstractDriveBase::clipHard;
}
