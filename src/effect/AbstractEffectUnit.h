/**********************************************************************
 * @file AbstractEffectUnit.h
 * @author TomatoPi
 * @version 2.0
 *
 * File providing base class for all effect Units
 **********************************************************************/
#ifndef DEF_ABSTRACT_EFFECT_UNIT_H
#define DEF_ABSTRACT_EFFECT_UNIT_H

#include "effect/JACKUnit.h"
#include "effect/EffectParamSet.h"
#include "core/Event.h"

#include "log.h"

/**
 * Base class for all effect units
 **/
class AbstractEffectUnit {

    public :

        /**
         * EffectUnit Constructor
         * Subclasses Constructor must build the JACKUnit
         *
         * @throw string if a construction step has failed
         * @see JACKUnit.h
         **/
        AbstractEffectUnit(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t poolSize);
        virtual ~AbstractEffectUnit();

        /**
         * Virtual predestructor that must be called before deleting the
         *  effect unit
         * Prevent segmentation fault by killing all JACK clients related
         *  to this object
         **/
        virtual void deactivate();
        
        /**
         * Function Called When a parameter is modified or bank changed
         * It will update the Param Array passed to callback function
         * Pure Virtual function, must be redefined by every effect type
         **/
        virtual void update(SFXPEvent& event) = 0;
        
        /**
         * Return JACK part of the EffectUnit
         * given number is index of an unit's port
         * return JACKUnit that own given port
         **/
        virtual JACKUnit* getOwner(SFXP::PortType type, SFXP::usize_t idx);

        /**
         * ID of the ParamSet
         **/
        SFXP::id1_t id() const;
        /**
         * Associated TypeCode
         **/
        SFXP::tc_t type() const;
        /**
         * Number of Parameters inside a Bank
         **/
        SFXP::usize_t size() const;

        /* *********************************************** */
        /* ************** Param Set Managment ************ */
        /* *********************************************** */

        /**
         * Attach a Param set to this Effect Unit
         **/
        void attachParamSet(ParamSet* p);

        /**
         * Change current bank
         *  Do nothing if change failed
         **/
        void setBank(SFXP::id1_t id);
        void nextBank();
        void prevBank();

        void printEffect();
        void printPool();
        

    /* *** General Stuff *** */
    protected :

        SFXP::id1_t _id;
        SFXP::tc_t  _type;

    /* *** JACK Stuff *** */
    protected :

        JACKUnit*   _jackU;

    /* *** Bank Related Stuff *** */
    protected :

        /**
         * Function called when all effect parameters has changed
         **/
        virtual void updateAll() = 0;

        ParamSet::BankIterator  _currentBank;
        ParamSet*               _paramSet;

        float*          _poolArray;
        SFXP::usize_t   _poolSize;
};

#endif
