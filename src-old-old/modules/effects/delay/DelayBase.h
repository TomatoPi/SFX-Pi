/**********************************************************************
 * @file DelayBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core Stuff For Delay / Chorus and time based effects
 **********************************************************************/
#ifndef DEF_DELAY_BASE_H
#define DEF_DELAY_BASE_H

#include <math.h>

#include "../../core/JACKUnit.h" // Get sample_t typedef

#include "../../core/buffer/Buffer.h"

class AbstractDelayBase{

    protected :

        virtual ~AbstractDelayBase()=0;
};

#endif
