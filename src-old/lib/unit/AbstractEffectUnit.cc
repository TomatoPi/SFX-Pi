/**********************************************************************
 * @file AbstractEffectUnit.cpp
 * @author TomatoPi
 * @version 1.1
 *
 * Core File for all effects
 **********************************************************************/
#include "AbstractEffectUnit.h"

using namespace SFXP;

/* ****************************************************************** */
/* ************************** Constructor *************************** */
/* ****************************************************************** */
AbstractEffectUnit::AbstractEffectUnit(id1_t id, tc_t type, usize_t bankSize, usize_t paramSize):
    EffectPrototype(id, type, bankSize),
    m_jackU( NULL ),
    m_paramArray( new float[paramSize] ),
    m_paramSize( paramSize )
{
    memset( m_paramArray, 0, m_paramSize * sizeof(float));
}

AbstractEffectUnit::~AbstractEffectUnit(){

    // Clear Current Param set
    delete m_paramArray;

    // Delete JACK Client
    delete m_jackU;
}

/**
 * Function to load parameters of an effect from an EffectPrototype
 * EffectPrototype's type must match AbstractEffectUnit's type
 * return 0 on success
 **/
int AbstractEffectUnit::loadFromPrototype(EffectPrototype* proto){

    if (SFXP::GlobalIsDebugEnabled)
        cout << "AbstractEffectUnit : Load Config from Prototype" << endl;

    // Verify that given Prototype is not NULL
    if (proto == NULL) {

        cout << "AbstractEffectUnit : Error : Passed Null Prototype" << endl;
        return 1;
    }
    
    // If types doesn't match return an error
    if (proto->getType() != m_type) {

        cout << "AbstractEffectUnit : Error : Prototype's Type don't match : "
            << proto->getType() << " Wanted Type is : " << m_type << endl;
        return 1;
    }

    // If ID doesn't match return an error
    if (proto->getID() != m_id) {

        cout << "AbstractEffectUnit : Error : Prototype's ID don't match : "
            << proto->getID() << " Wanted ID is : " << m_id << endl;
        return 1;
    }

    // If bank's size doesn't match return an error
    if (proto->getBankSize() != m_bankSize) {

        cout << "AbstractEffectUnit : Error : Prototype's Bank Size don't match : "
            << proto->getBankSize() << " Wanted Size is : " << m_bankSize << endl;
        return 1;
    }

    // Clear Effect
    this->clearEffect();

    // Get banks from prototype
    ModBankArray banks = proto->getAllBanks();
    SFXP::usize_t size = proto->getBankSize();

    for ( auto& b : banks ){

        this->addBank( b.first, size, b.second );
    }

    return 0;
}

void AbstractEffectUnit::deactivate(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();
}

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
int AbstractEffectUnit::setParam(SFXP::usize_t idx, float value){

    if (this->EffectPrototype::setParam(idx, value)) return 1;
    
    this->update();
    return 0;
}
int AbstractEffectUnit::setParam(SFXP::usize_t size, const float *values){

    if (this->EffectPrototype::setParam(size, values)) return 1;

    this->update();
    return 0;
}

/**
 * Add or Delete Given Bank.
 * For add a new Bank, given ID must not be already present
 * Given float array must be the same size as bank Size
 *
 * @return 0 on success
 *
 * @warning Some ID values are reserved, @see EUCST
 **/
int AbstractEffectUnit::addBank(SFXP::id1_t id){

    if (this->EffectPrototype::addBank(id)) return 1;

    this->update();
    return 0;
}
int AbstractEffectUnit::addBank(SFXP::id1_t id, SFXP::usize_t size, const float *values){

    if (this->EffectPrototype::addBank(id, size, values)) return 1;

    this->update();
    return 0;
}

/**
 * Change Current Bank.
 * Prev and NextBank functions returns true if incrementation
 * has looped to the array beginning or end
 * Change Bank function take target bank id as parameter
 **/
bool AbstractEffectUnit::nextBank(){

    bool rtn = this->EffectPrototype::nextBank();

    this->update();
    return rtn;
}
bool AbstractEffectUnit::prevBank(){

    bool rtn = this->EffectPrototype::prevBank();

    this->update();
    return rtn;
}
int AbstractEffectUnit::changeBank(SFXP::id1_t id){

    if (this->EffectPrototype::changeBank(id)) return 1;

    this->update();
    return 0;
}

/* ****************************************************************** */
/* *********************** Getters and Setters ********************** */
/* ****************************************************************** */

JACKUnit* AbstractEffectUnit::getOwner(JACKUnit::PortType type, usize_t pidx){

    return m_jackU;
}

void AbstractEffectUnit::printEffect() const{

    printf("Effect : Type( %2u ) Id( %2u ) UniqueName( %-15s )\n",
        m_type, m_id, m_jackU->getName().c_str());

    printf("   Banks :\n");
    
    for ( auto& cur : m_bankArray ){

        if ( cur.first == m_currentBank->first ){
            
            printf("     >> Bank ( %2u ) :", cur.first);
        }
        else{

            printf("      - Bank ( %2u ) :", cur.first);
        }

        for ( usize_t i = 0; i < m_bankSize; i++ ){
            printf(" %3.5f", cur.second[i]);
        }
        printf("\n");
    }
    printf("End\n");
}

void AbstractEffectUnit::printPool() const{

    printf("Effect : Type( %2u ) Id( %2u ) UniqueName( %-15s )\n",
        m_type, m_id, m_jackU->getName().c_str());

    printf("Pool :\n");
    
    for ( usize_t i = 0; i < m_paramSize; i++ ){
        printf(" %3.5f", m_paramArray[i]);
    }
    
    printf("\nEnd\n");
}
