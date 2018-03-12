/**********************************************************************
 * @file AbstractEffectUnit.h
 * @author TomatoPi
 * @version 1.1
 *
 * Core File for all effects
 **********************************************************************/
#ifndef DEF_ABSTRACT_EFFECT_UNIT_H
#define DEF_ABSTRACT_EFFECT_UNIT_H

#include <iostream>
#include <string>
#include <map>

#include "lib/unit/JACKUnit.h"
#include "lib/unit/EffectPrototype.h"

#include "Const.h"

/**
 * Core class of Effects
 **/
class AbstractEffectUnit : public EffectPrototype {

    public :

        /**
         * EffectUnit Constructor
         * Subclasses Constructor must build the JACKUnit
         *
         * @throw string if a construction step has failed
         * @see JACKUnit.h
         **/
        AbstractEffectUnit(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t bankSize, SFXP::usize_t paramSize);
        virtual ~AbstractEffectUnit();

        /**
         * Function to load parameters of an effect from an EffectPrototype
         * EffectPrototype's type must match AbstractEffectUnit's type
         * return 0 on success
         **/
        int loadFromPrototype(EffectPrototype* proto);

        /**
         * Virtual predestructor that must be called before deleting the
         *  effect unit
         * Prevent segmentation fault by killing all JACK clients related
         *  to this object
         **/
        virtual void deactivate();
        
        /**
         * Change Effects Parameters.
         * Changed effect will affect current Bank, or create a new one
         * if effect bank array is empty
         * The two functions provided change a single param or all
         * params in a single row
         * Do Nothing if given index is too big or length differs from
         * bank size
         * @return 0 on success
         **/
        virtual int setParam(SFXP::usize_t idx, float value);
        virtual int setParam(SFXP::usize_t size, const float *values);

        /**
         * Add or Delete Given Bank.
         * For add a new Bank, given ID must not be already present
         * Given float array must be the same size as bank Size
         *
         * @return 0 on success
         *
         * @warning Some ID values are reserved, @see EUCST
         **/
        virtual int addBank(SFXP::id1_t id);
        virtual int addBank(SFXP::id1_t id, SFXP::usize_t size, const float *values);

        /**
         * Change Current Bank.
         * Prev and NextBank functions returns true if incrementation
         * has looped to the array beginning or end
         * Change Bank function take target bank id as parameter
         **/
        virtual bool nextBank();
        virtual bool prevBank();
        virtual int changeBank(SFXP::id1_t id);
        
        /**
         * Return JACK part of the EffectUnit
         * given number is index of an unit's port
         * return JACKUnit that own given port
         **/
        virtual JACKUnit* getOwner(JACKUnit::PortType type, SFXP::usize_t idx);

        /**
         * Print Current Effect Unit Content
         **/
        void printEffect() const;
        void printPool() const;

    /* *** Effect Unit Stuff *** */
    protected :

        /**
         * Function Called When a parameter is modified or bank changed
         * It will update the Param Array passed to callback function
         * Pure Virtual function, must be redefined by every effect type
         **/
        virtual void update() = 0;

    /* *** JACK Stuff *** */
    protected :

        JACKUnit *m_jackU;

    /* *** Bank Related Stuff *** */
    protected :

        float*          m_paramArray;
        SFXP::usize_t   m_paramSize;
};

#endif
