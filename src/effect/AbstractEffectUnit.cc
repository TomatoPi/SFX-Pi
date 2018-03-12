/**********************************************************************
 * @file AbstractEffectUnit.h
 * @author TomatoPi
 * @version 2.0
 *
 * File providing base class for all effect Units
 **********************************************************************/
#include "AbstractEffectUnit.h"

using namespace std;

#define NAME (_jackU->getName())

/**
 * EffectUnit Constructor
 * Subclasses Constructor must build the JACKUnit
 *
 * @throw string if a construction step has failed
 * @see JACKUnit.h
 **/
AbstractEffectUnit::AbstractEffectUnit(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t poolSize):
_id(id),
_type(type),

_jackU(nullptr),

_currentBank(nullptr),
_paramSet(nullptr),

_poolArray(new float[poolSize]),
_poolSize(poolSize)
{
    memset(_poolArray, 0, poolSize*sizeof(float));
}
AbstractEffectUnit::~AbstractEffectUnit() {

    delete[]    _poolArray;
    delete      _jackU;
}

/**
 * Virtual predestructor that must be called before deleting the
 *  effect unit
 * Prevent segmentation fault by killing all JACK clients related
 *  to this object
 **/
void AbstractEffectUnit::deactivate() {
    
    _jackU->deactivateClient();
}

JACKUnit* AbstractEffectUnit::getOwner(SFXP::PortType type, SFXP::usize_t pidx){

    return _jackU;
}

/**
 * ID of the ParamSet
 **/
SFXP::id1_t AbstractEffectUnit::id() const {

    return _id;
}
/**
 * Associated TypeCode
 **/
SFXP::tc_t AbstractEffectUnit::type() const {

    return _type;
}
/**
 * Number of Parameters inside a Bank
 **/
SFXP::usize_t AbstractEffectUnit::size() const {

    return _poolSize;
}

/* *********************************************** */
/* ************** Param Set Managment ************ */
/* *********************************************** */

/**
 * Attach a Param set to this Effect Unit
 **/
void AbstractEffectUnit::attachParamSet(ParamSet* p) {

    if (p) {
        
        _paramSet = p;
        _currentBank = p->firstBank();

        this->updateAll();
    }
}

/**
 * Change current bank
 *  Do nothing if change failed
 **/
void AbstractEffectUnit::setBank(SFXP::id1_t id) {

    if (_paramSet) {

        ParamSet::BankIterator bi = _paramSet->findBank(id);

        if (bi != _paramSet->endBank()) {

            _currentBank = bi;
            this->updateAll();
        }
    }
}
void AbstractEffectUnit::nextBank() {

    if (_paramSet) {

        ++_currentBank;
        if (_currentBank == _paramSet->endBank())
            _currentBank = _paramSet->firstBank();

        this->updateAll();
    }
}
void AbstractEffectUnit::prevBank() {

    if (_paramSet) {

        if (_currentBank == _paramSet->firstBank())
            _currentBank = _paramSet->endBank();

        --_currentBank;
        this->updateAll();
    }
}

void AbstractEffectUnit::printEffect() {

    SFXPlog::log(NAME) << "Type : " << _type << " ID : " << _id << endl;

    if (_paramSet) {
        
        for (auto& b : _paramSet->getBanks()) {

            if (b.first == _currentBank->first)
                cout << "    >> ";
            else
                cout << "     - ";

            cout << "Bank ( " << b.first << " ) : ";

            for (SFXP::usize_t i = 0; i < _paramSet->size(); i++) {

                printf(" %3.5f", b.second[i]);
            }
            cout << endl;
        }
    }
    else {

        SFXPlog::err(NAME) << "No Param Set Attached" << endl;
    }
}

void AbstractEffectUnit::printPool() {

    SFXPlog::log(NAME) << "Type : " << _type << " ID : " << _id << endl;

    cout << "Pool :";
    for (SFXP::usize_t i = 0; i < _poolSize; i++) {

        printf(" %3.5f", _poolArray[i]);
    }
    cout << endl;
}
