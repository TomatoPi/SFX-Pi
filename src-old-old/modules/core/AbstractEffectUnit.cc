/**********************************************************************
 * @file AbstractEffectUnit.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * Core File for all effects
 **********************************************************************/
#include "AbstractEffectUnit.h"

/* ****************************************************************** */
/* ************************** Constructor *************************** */
/* ****************************************************************** */
AbstractEffectUnit::AbstractEffectUnit(id1_t id, id1_t type, usize_t bankSize, usize_t paramSize):
    m_id( id ),
    m_type( type ),
    m_jackU( NULL ),
    m_bankArray( ModBankArray() ),
    m_currentBank( m_bankArray.begin() ),
    m_bankSize( bankSize + EUCST::C_PARAM_COUNT ),
    m_paramArray( new float[paramSize + EUCST::C_PARAM_COUNT] ),
    m_paramSize( paramSize + EUCST::C_PARAM_COUNT )
{

    std::memset( m_paramArray, 0, m_paramSize * sizeof(float));
}

AbstractEffectUnit::~AbstractEffectUnit(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    // Clear Bank Array
    for( m_currentBank = m_bankArray.begin();
        m_currentBank != m_bankArray.end();
        m_currentBank++ )
    {
        delete m_currentBank->second;
    }
    m_bankArray.clear();

    // Clear Current Param set
    delete m_paramArray;

    // Delete JACK Client
    delete m_jackU;
}

/* ****************************************************************** */
/* ************************* Param Managment ************************ */
/* ****************************************************************** */
void AbstractEffectUnit::setParam(usize_t idx, float value){

    if ( idx < m_bankSize ){
        
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Change Parameter ... ");
        }

        if ( m_bankArray.size() == 0 ){
            
            m_bankArray[0] = new float[m_bankSize];
            m_currentBank = m_bankArray.find(0);
        }

        m_currentBank->second[idx] = value;

        this->update();

        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Done\n");
        }
    }
}

void AbstractEffectUnit::setParam(usize_t size, const float *values){
    
    if ( size == m_bankSize ){

        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Change Parameters ... ");
        }
        
        if ( m_bankArray.size() == 0 ){
            
            m_bankArray[0] = new float[m_bankSize];
            m_currentBank = m_bankArray.find(0);
            
            if ( SFXP::GlobalIsDebugEnabled ){
                printf("First Bank Created ... ");
            }
        }
        
        std::memcpy( m_currentBank->second, values, size * sizeof(float) );
        
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Done\n");
        }

        this->update();
    }
    else{
        printf("ERROR : Given (%lu) Params Array : Effect has (%lu)\n", size, m_bankSize);
    }
}

float AbstractEffectUnit::getParam(usize_t idx) const{

    if ( idx < m_bankSize && m_bankArray.size() != 0 ){

        return m_currentBank->second[idx];
    }
    return 0.0f;
}

/* ****************************************************************** */
/* ************************* Bank Managment ************************* */
/* ****************************************************************** */
int AbstractEffectUnit::addBank(id1_t id){

    if ( m_bankArray.find(id) == m_bankArray.end() ){

        m_bankArray[id] = new float[m_bankSize];
        std::memset( m_bankArray[id], 0, m_bankSize * sizeof(float) );

        m_currentBank = m_bankArray.find(id);

        this->update();

        return 0;
    }
    return EUCST::BANK_ID_ERR;
}

int AbstractEffectUnit::addBank(id1_t id, usize_t size, const float *values){

    if ( size == m_bankSize && m_bankArray.find(id) == m_bankArray.end() ){

        m_bankArray[id] = new float[m_bankSize];
        std::memcpy( m_bankArray[id], values, size * sizeof(float) );

        m_currentBank = m_bankArray.find(id);

        this->update();
    
        return 0;
    }

    printf("AddBank : Error : size : n:%lu p:%lu : id : n:%u\n",
        size, m_bankSize, id);
    return EUCST::BANK_ID_ERR;
}

void AbstractEffectUnit::removeBank(id1_t id){

    if ( m_bankArray.find(id) != m_bankArray.end() ){

        delete m_bankArray[id]; // Free float array
        m_bankArray.erase(id);  // Remove Array entry
    }
}

uint8_t AbstractEffectUnit::getCurrentBank() const{

    if ( m_bankArray.size() != 0 ){

        return m_currentBank->first;
    }
    return EUCST::BANK_ID_ERR;
}

AbstractEffectUnit::ModBankArray AbstractEffectUnit::getAllBanks() const{

    return m_bankArray;
}

usize_t AbstractEffectUnit::getBankSize() const{

    return m_bankSize;
}
/* ****************************************************************** */
/* ********************** Bank Managment Next *********************** */
/* ****************************************************************** */
bool AbstractEffectUnit::nextBank(){

    bool rtn = false;
    if ( m_bankArray.size() != 0 ){

        ++m_currentBank;
        if ( m_currentBank == m_bankArray.end() ){

            rtn = true;
            m_currentBank = m_bankArray.begin();
        }

        this->update();
    }
    return rtn;
}

bool AbstractEffectUnit::prevBank(){

    bool rtn = false;
    if ( m_bankArray.size() != 0 ){

        if ( m_currentBank == m_bankArray.begin() ){

            rtn = true;
            m_currentBank = m_bankArray.end();
        }
        --m_currentBank;

        this->update();
    }
    return rtn;
}

int AbstractEffectUnit::changeBank(id1_t id){

    if ( m_bankArray.find(id) != m_bankArray.end() ){

        m_currentBank = m_bankArray.find(id);

        this->update();
        
        return 0;
    }
    return 1;
}

/* ****************************************************************** */
/* *********************** Getters and Setters ********************** */
/* ****************************************************************** */
void AbstractEffectUnit::setBypass(bool state){

    if ( m_bankArray.size() == 0 ){
        
        m_bankArray[0] = new float[m_bankSize];
        m_currentBank = m_bankArray.find(0);
    }

    m_currentBank->second[EUCST::IDXBYP] = (float)state;

    this->update();
}

bool AbstractEffectUnit::getBypass() const{

    return m_paramArray[EUCST::IDXBYP];
}

uint8_t AbstractEffectUnit::getType() const{

    return m_type;
}

uint8_t AbstractEffectUnit::getID() const{

    return m_id;
}

JACKUnit* AbstractEffectUnit::getJACKUnit(usize_t pidx){

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

void AbstractEffectUnit::clearEffect(){

    for ( auto& cur : m_bankArray ){

        delete cur.second;
    }
    m_bankArray.clear();

    m_currentBank == m_bankArray.begin();
}
