/**********************************************************************
 * @file DriveBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core Stuff For Disortion / Overdrive / Fuzz effect
 **********************************************************************/
#ifndef DEF_EFFECT_DRIVE_BASE_H
#define DEF_EFFECT_DRIVE_BASE_H

#include "../core/JACKUnit.h" // Get sample_t typedef

class AbstractDriveBase{
    
    protected :
    
        virtual ~AbstractDriveBase()=0;
        
        /** Typedef Clipping function **/
        typedef sample_t (*clip_f)( sample_t, float, float );

        typedef enum{
            SOFT,
            HARD
        }TYPE;
        
        /** Convert an TYPE to correct function pointer **/
        static clip_f castClipper( TYPE );
        
        /** HardClipping Function **/
        static inline sample_t clipHard( sample_t in, float p1, float p2 ){

            return (in<-1)?-1:(in>1)?1:in;
        }
        
        /** SoftClipping Function **/
        static inline sample_t clipSoft( sample_t in, float soft, float shape ){

            return (1 - shape)*tanh( in ) + shape*tanh( in / soft );
        }
};

#endif
