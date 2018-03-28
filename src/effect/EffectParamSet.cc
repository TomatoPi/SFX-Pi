/**********************************************************************
 * @file EffectParamSet.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object that store a set of param banks for effects
 **********************************************************************/
#include "EffectParamSet.h"

/**
 * Constructor, set id, associated type and bank size
 **/
ParamSet::ParamSet(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t size):
_name(""),
_id(id),
_type(type),
_banks(BankList()),
_bankSize(size)
{
    _banks[0] = new float[size];
    memset(_banks[0], 0, _bankSize*sizeof(float));
}
ParamSet::~ParamSet() {

    for (auto& bank : _banks) {

        delete[] bank.second;
    }
    _banks.clear();
}

/**
 * ID of the ParamSet
 **/
SFXP::id1_t ParamSet::id() const {

    return _id;
}
/**
 * Associated TypeCode
 **/
SFXP::tc_t ParamSet::type() const {

    return _type;
}
/**
 * Number of Parameters inside a Bank
 **/
SFXP::usize_t ParamSet::size() const {

    return _bankSize;
}

/**
 * Name of the effect associated with this param set
 **/
std::string ParamSet::name() {

    return _name;
}
/**
 * Set the name of the associated effect
 **/
void ParamSet::setName(std::string name) {

    _name = name;
}

/**
 * Get Add or Edit Given Bank
 **/
float* ParamSet::getBank(SFXP::id1_t id, bool create) {

    if (_banks.find(id) != _banks.end()) {

        return _banks[id];
    }
    else if (create) {

        _banks[id] = new float[_bankSize];
        memset(_banks[0], 0, _bankSize*sizeof(float));
        return _banks[id];
    }
    
    return nullptr;
}
void ParamSet::addBank(SFXP::id1_t id, SFXP::usize_t size, const float* bank) {

    if (size == _bankSize || (size == 0 && bank == nullptr)) {

        // If bank doesn't exist create it
        if (_banks.find(id) == _banks.end()) {

            _banks[id] = new float[_bankSize];
            memset(_banks[0], 0, _bankSize*sizeof(float));
        }

        // if a bank has been given copy it
        if (bank) {

            memcpy(_banks[id], bank, size*sizeof(float));
        }
    }
}
void ParamSet::editBank(SFXP::id1_t id, SFXP::usize_t idx, float value) {

    if (idx < _bankSize) {

        // If bank doesn't exist create it
        if (_banks.find(id) == _banks.end()) {

            _banks[id] = new float[_bankSize];
            memset(_banks[0], 0, _bankSize*sizeof(float));
        }

        _banks[id][idx] = value;
    }
}
void ParamSet::removeBank(SFXP::id1_t id) {

    if (_banks.find(id) != _banks.end()) {

        delete[] _banks[id];
        _banks.erase(id);
    }

    // Always keep one bank inside the ParamSet
    if (_banks.size() == 0) {

        _banks[0] = new float[_bankSize];
    }
}
ParamSet::BankList ParamSet::getBanks() const{

    return _banks;
}
ParamSet::BankIterator ParamSet::firstBank() {

    return _banks.begin();
}
ParamSet::BankIterator ParamSet::endBank() {

    return _banks.end();
}
ParamSet::BankIterator ParamSet::findBank(SFXP::id1_t id) {

    return _banks.find(id);
}
