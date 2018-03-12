/**********************************************************************
 * @file EffectPrototype.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing effect object base shared by SFX-Pi and SFX-PE
 **********************************************************************/
#ifndef DEF_EFFECT_PROTOTYPE_H
#define DEF_EFFECT_PROTOTYPE_H

#include <iostream>
#include <cstring>
#include <string>
#include <map>

#include "Const.h"

class EffectPrototype {

    public :

        typedef std::map<SFXP::id1_t, float*> ModBankArray;

        /**
         * Constructor and Destructor
         **/
        EffectPrototype(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t bankSize);
        virtual ~EffectPrototype();

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
        float getParam(SFXP::usize_t idx)  const;

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
        void removeBank(SFXP::id1_t id);
        SFXP::id1_t getCurrentBank() const;
        ModBankArray getAllBanks() const;
        SFXP::usize_t getBankSize() const;

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
         * Get Effect's Type Code
         **/
        SFXP::tc_t getType() const;

        /**
         * Get Effect's ID
         **/
        SFXP::id1_t getID() const;

        /**
         * Clear content of effect unit
         **/
        void clearEffect();

    protected :
    
        SFXP::id1_t m_id;
        SFXP::tc_t  m_type;
        
        ModBankArray            m_bankArray;
        ModBankArray::iterator  m_currentBank;
        SFXP::usize_t           m_bankSize;
};

/**
 * Effect Unit Constants
 **/
namespace EUCST{

    /** Error Code if Unit Contains no Banks **/
    const SFXP::id1_t BANK_ID_ERR = 0xff;
};

#endif
