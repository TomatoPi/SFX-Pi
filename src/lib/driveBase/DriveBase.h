/**********************************************************************
 * @file DriveBase.h
 * @author TomatoPi
 * @version 1.1
 *
 * Core Stuff For Disortion / Overdrive / Fuzz effect
 **********************************************************************/
#ifndef DEF_EFFECT_DRIVE_BASE_H
#define DEF_EFFECT_DRIVE_BASE_H

#include <math.h>
#include <iostream>
#include "types.h"

class AbstractDriveBase{
    
    protected :
    
        virtual ~AbstractDriveBase()=0;
        
        /** Typedef Clipping function **/
        typedef SFXP::sample_t (*clip_f)( SFXP::sample_t, float, float );

        enum TYPE{
            SOFT=0,
            HARD=1
        };
        
        /** Convert an TYPE to correct function pointer **/
        static clip_f castClipper( TYPE );
        
        /** HardClipping Function **/
        static inline SFXP::sample_t clipHard( SFXP::sample_t in, float p1, float p2 ){

            return (in<-1)?-1:(in>1)?1:in;
        }
        
        /** SoftClipping Function **/
        static inline SFXP::sample_t clipSoft( SFXP::sample_t in, float soft, float shape ){

            return (1 - shape)*tanh( in ) + shape*tanh( in / soft );
        }
};

#endif
