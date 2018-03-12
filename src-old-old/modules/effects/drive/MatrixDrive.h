/**********************************************************************
 * @file MatrixDrive.h
 * @author TomatoPi
 * @version 1.0
 *
 * Advanced Distortion Effect
 * Segment audio Signal in several bands ( using Parametric Equalizer )
 * and perform distortion on each band
 * Output a main mixed output and each bands on secondary ports
 * Oposite to Drive effect, MatrixDrive perform the same distortion on
 * positive and Negatives Half-waves, moreover no output EQ is present
 **********************************************************************/
#ifndef DEF_EFFECT_MATRIX_DRIVE_H
#define DEF_EFFECT_MATRIX_DRIVE_H

#include <string>

#include "../../core/AbstractEffectUnit.h"
#include "../../core/filter/ParametricEQ.h"

#include "../../RegistrableUnit.h"

#include "DriveBase.h"

class MatrixDriveEffect : public AbstractEffectUnit, public AbstractDriveBase,
    public RegistrableUnit<MatrixDriveEffect>
{

    public :
    
        /**
         * Constructor and Process Callback
         **/
        MatrixDriveEffect(id1_t id, id1_t type);
        ~MatrixDriveEffect();

        static int process(jack_nframes_t nframes, void* arg);

    private :
    
        virtual void update();

    private :

        static const usize_t MXBC = 5; // Max Band Count

        ParametricEQ *m_eq;

        clip_f m_clip[MXBC];
};

typedef RegistrableUnit<MatrixDriveEffect> MDriveReg;

#endif
