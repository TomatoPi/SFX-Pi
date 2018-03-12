/**********************************************************************
 * @file EffectPrototype.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing effect object base shared by SFX-Pi and SFX-PE
 **********************************************************************/
#include "EffectPrototype.h"

using namespace SFXP;

/**
 * Constructor and Destructor
 **/
EffectPrototype::EffectPrototype(SFXP::id1_t id, SFXP::tc_t type, SFXP::usize_t bankSize):
    m_id( id ),
    m_type( type ),

    m_bankArray( ModBankArray() ),
    m_currentBank( m_bankArray.begin() ),
    m_bankSize( bankSize )
{
}
EffectPrototype::~EffectPrototype(){

    // Clear Bank Array
    for( m_currentBank = m_bankArray.begin();
        m_currentBank != m_bankArray.end();
        m_currentBank++ )
    {
        delete m_currentBank->second;
    }
    m_bankArray.clear();
}

/* ****************************************************************** */
/* ************************* Param Managment ************************ */
/* ****************************************************************** */
int EffectPrototype::setParam(usize_t idx, float value){

    if ( idx < m_bankSize ){
        
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Change Parameter ... ");
        }

        if ( m_bankArray.size() == 0 ){
            
            m_bankArray[0] = new float[m_bankSize];
            m_currentBank = m_bankArray.find(0);
        }

        m_currentBank->second[idx] = value;

        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Done\n");
        }

        return 0;
    }
    return 1;
}

int EffectPrototype::setParam(usize_t size, const float *values){
    
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
        
        memcpy( m_currentBank->second, values, size * sizeof(float) );
        
        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Done\n");
        }

        return 0;
    }
    else{
        printf("ERROR : Given (%u) Params Array : Effect has (%u)\n", size, m_bankSize);
        return 1;
    }
}

float EffectPrototype::getParam(usize_t idx) const{

    if ( idx < m_bankSize && m_bankArray.size() != 0 ){

        return m_currentBank->second[idx];
    }
    return 0.0f;
}

/* ****************************************************************** */
/* ************************* Bank Managment ************************* */
/* ****************************************************************** */
int EffectPrototype::addBank(id1_t id){

    if ( m_bankArray.find(id) == m_bankArray.end() ){

        m_bankArray[id] = new float[m_bankSize];
        memset( m_bankArray[id], 0, m_bankSize * sizeof(float) );

        m_currentBank = m_bankArray.find(id);

        return 0;
    }
    return EUCST::BANK_ID_ERR;
}

int EffectPrototype::addBank(id1_t id, usize_t size, const float *values){

    if ( size == m_bankSize && m_bankArray.find(id) == m_bankArray.end() ){

        m_bankArray[id] = new float[m_bankSize];
        memcpy( m_bankArray[id], values, size * sizeof(float) );

        m_currentBank = m_bankArray.find(id);
    
        return 0;
    }

    printf("AddBank : Error : size : n:%u p:%u : id : n:%u\n",
        size, m_bankSize, id);
    return EUCST::BANK_ID_ERR;
}

void EffectPrototype::removeBank(id1_t id){

    if ( m_bankArray.find(id) != m_bankArray.end() ){

        delete m_bankArray[id]; // Free float array
        m_bankArray.erase(id);  // Remove Array entry
    }
}

id1_t EffectPrototype::getCurrentBank() const{

    if ( m_bankArray.size() != 0 ){

        return m_currentBank->first;
    }
    return EUCST::BANK_ID_ERR;
}

EffectPrototype::ModBankArray EffectPrototype::getAllBanks() const{

    return m_bankArray;
}

usize_t EffectPrototype::getBankSize() const{

    return m_bankSize;
}
/* ****************************************************************** */
/* ********************** Bank Managment Next *********************** */
/* ****************************************************************** */
bool EffectPrototype::nextBank(){

    bool rtn = false;
    if ( m_bankArray.size() != 0 ){

        ++m_currentBank;
        if ( m_currentBank == m_bankArray.end() ){

            rtn = true;
            m_currentBank = m_bankArray.begin();
        }

    }
    return rtn;
}

bool EffectPrototype::prevBank(){

    bool rtn = false;
    if ( m_bankArray.size() != 0 ){

        if ( m_currentBank == m_bankArray.begin() ){

            rtn = true;
            m_currentBank = m_bankArray.end();
        }
        --m_currentBank;

    }
    return rtn;
}

int EffectPrototype::changeBank(id1_t id){

    if ( m_bankArray.find(id) != m_bankArray.end() ){

        m_currentBank = m_bankArray.find(id);
        
        return 0;
    }
    return 1;
}

/* ****************************************************************** */
/* *********************** Getters and Setters ********************** */
/* ****************************************************************** */

tc_t EffectPrototype::getType() const{

    return m_type;
}

id1_t EffectPrototype::getID() const{

    return m_id;
}

void EffectPrototype::clearEffect(){

    for ( auto& cur : m_bankArray ){

        delete cur.second;
    }
    m_bankArray.clear();

    m_currentBank == m_bankArray.begin();
}
