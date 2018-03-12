/**********************************************************************
 * @file EndUnit.h
 * @author TomatoPi
 * @version 1.0
 *
 * Specific effect unit placed at the beginning and at the end of the
 * process graph
 * Provide Unique functions for monitoring io
 **********************************************************************/
#ifndef DEF_END_UNIT_H
#define DEF_END_UNIT_H

#include <iostream>
#include <unistd.h>

#include "../../Const.h"

#include "../core/AbstractEffectUnit.h"
#include "../RegistrableUnit.h"

#include "../midi/MIDIBase.h"

class EndUnit : public AbstractEffectUnit, public RegistrableUnit<EndUnit>
{
    public :

        EndUnit( id1_t id, id1_t type );
        ~EndUnit();

        static int process(jack_nframes_t nframes, void* arg);

        static const std::string CAPTURE;
        static const std::string PLAYBACK;

        bool isSaturated(bool chanel) const;
        void resetSaturated(bool chanel);
        
        sample_t getMax(bool chanel) const;

    private :
    
        virtual void update();

        bool m_isSatL;
        bool m_isSatR;

        sample_t m_maxL;
        sample_t m_maxR;
};

typedef RegistrableUnit<EndUnit> EndReg;

#endif
