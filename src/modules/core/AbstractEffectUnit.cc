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
AbstractEffectUnit::AbstractEffectUnit(uint8_t id, uint8_t type, uint8_t bankSize, uint8_t paramSize):
    m_id( id ),
    m_type( type ),
    m_jackU( NULL ),
    m_bankArray( ModBankArray() ),
    m_currentBank( m_bankArray.begin() ),
    m_bankSize( bankSize + EUCST::C_PARAM_COUNT ),
    m_paramArray( new float[paramSize + EUCST::C_PARAM_COUNT] ),
    m_paramSize( paramSize + EUCST::C_PARAM_COUNT )
{
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
void AbstractEffectUnit::setParam(uint8_t idx, float value){

    if ( idx < m_bankSize ){

        if ( m_bankArray.size() == 0 ){
            
            m_bankArray[0] = new float[m_bankSize];
            m_currentBank = m_bankArray.find(0);
        }

        m_currentBank->second[idx] = value;

        this->update();
    }
}

void AbstractEffectUnit::setParam(uint8_t size, const float *values){
    
    if ( size == m_bankSize ){

        std::cout << "Change Param" << std::endl;
        if ( m_bankArray.size() == 0 ){
            
            m_bankArray[0] = new float[m_bankSize];
            m_currentBank = m_bankArray.find(0);
            std::cout << "Bank Created" << std::endl;
        }
        
        std::memcpy( m_currentBank->second, values, size * sizeof(float) );
        std::cout << "Param Applied" << std::endl;

        this->update();
    }
    else{

        std::cout << "Err Param :" << std::to_string( size ) << " vs " << std::to_string( m_bankSize ) << std::endl;
    }
}

float AbstractEffectUnit::getParam(uint8_t idx) const{

    if ( idx < m_bankSize && m_bankArray.size() != 0 ){

        return m_currentBank->second[idx];
    }
    return 0.0f;
}

/* ****************************************************************** */
/* ************************* Bank Managment ************************* */
/* ****************************************************************** */
void AbstractEffectUnit::addBank(uint8_t id, uint8_t size, const float *values){

    if ( size == m_bankSize && m_bankArray.find(id) == m_bankArray.end() ){

        m_bankArray[id] = new float[m_bankSize];
        std::memcpy( m_bankArray[id], values, size * sizeof(float) );
    }
}

void AbstractEffectUnit::removeBank(uint8_t id){

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

void AbstractEffectUnit::changeBank(uint8_t id){

    if ( m_bankArray.find(id) != m_bankArray.end() ){

        m_currentBank = m_bankArray.find(id);

        this->update();
    }
}

/* ****************************************************************** */
/* *********************** Getters and Setters ********************** */
/* ****************************************************************** */
void AbstractEffectUnit::setBypass(bool state){

    if ( m_bankArray.size() == 0 ){
        
        m_bankArray[0] = new float[m_bankSize];
        m_currentBank = m_bankArray.find(0);
    }

    m_currentBank->second[EUCST::C_IDX_BYPASS] = (float)state;

    this->update();
}

bool AbstractEffectUnit::getBypass() const{

    return m_paramArray[EUCST::C_IDX_BYPASS];
}

uint8_t AbstractEffectUnit::getType() const{

    return m_type;
}

uint8_t AbstractEffectUnit::getID() const{

    return m_id;
}

JACKUnit* AbstractEffectUnit::getJACKUnit(){

    return m_jackU;
}
