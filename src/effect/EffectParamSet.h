/**********************************************************************
 * @file EffectParamSet.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object that store a set of param banks for effects
 **********************************************************************/
#ifndef DEF_EFFECT_PARAM_SET_H
#define DEF_EFFECT_PARAM_SET_H

#include "types.h"
#include <string>
#include <cstring>
#include <map>

/**
 * Object storing several list of floats used by effect units as parameters
 **/
class ParamSet {

    public :

        typedef std::map<SFXP::id1_t, float*> BankList;
        typedef BankList::iterator BankIterator;

        /**
         * Constructor, set id, associated type and bank size
         **/
        ParamSet(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t size);
        ~ParamSet();

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

        /**
         * Name of the effect associated with this param set
         **/
        std::string name();
        /**
         * Set the name of the associated effect
         **/
        void setName(std::string name);

        /**
         * Get Add or Edit Given Bank
         **/
        float* getBank(SFXP::id1_t id, bool create=false);
        void addBank(SFXP::id1_t id, SFXP::usize_t size=0, const float* bank=0);
        void editBank(SFXP::id1_t id, SFXP::usize_t idx, float value);
        void removeBank(SFXP::id1_t id);
        
        BankList getBanks() const;
        BankIterator firstBank();
        BankIterator endBank();
        BankIterator findBank(SFXP::id1_t id);

    private :

        std::string _name;

        SFXP::id1_t _id;    /**< ID of the Param Set */
        SFXP::tc_t _type;   /**< TypeCode of associated effect */

        BankList _banks;    /**< List of stored banks */
        SFXP::usize_t _bankSize;/**< Number of floats inside a bank */
};

#endif
