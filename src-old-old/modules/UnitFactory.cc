/**********************************************************************
 * @file UnitFactory.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Common stuff about effect units
 **********************************************************************/
#include "UnitFactory.h"

std::map<id1_t, UnitFactory::UnitFactoryReg> UnitFactory::m_reg = std::map<id1_t, UnitFactory::UnitFactoryReg>();

/* ****************************************************************** */
/* ******************* Registration and Creation ******************** */
/* ****************************************************************** */
void UnitFactory::registerEffect(id1_t type,
                    std::string name, builder_f builder,
                    const std::string* paramNames, usize_t paramCount,
                    const std::string* portNames, 
                    usize_t ai, usize_t ao, usize_t mi, usize_t mo){

    if ( m_reg.find(type) == m_reg.end() ){

        m_reg[type] = UnitFactory::UnitFactoryReg(name, builder,
                                        paramNames, paramCount,
                                        portNames, ai, ao, mi, mo );

        printf("UnitFactory : Registered : TC( %3u ) : Name( %-18s ) : ParamCount( %2lu ) : PortCount( %lu %lu %lu %lu )\n",
            type,
            name.c_str(),
            m_reg[type].m_paramNames.size(),
            ai, ao, mi, mo
            );
    }
    else{

        printf("UnitFactory : Error : TC( %3u ) is already used\n", type);
    }
}

AbstractEffectUnit* UnitFactory::createEffect(id1_t type, id1_t id){

    if ( m_reg.find(type) != m_reg.end() ){

        return (*m_reg[type].m_builder)(id, type);
    }
    return NULL;
}

/* ****************************************************************** */
/* ************************ Getters and Setters ********************* */
/* ****************************************************************** */
std::string UnitFactory::getName(id1_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_name;
    }
    return std::string();
}

UnitFactory::NameArray UnitFactory::getParamNames(id1_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_paramNames;
    }
    return UnitFactory::NameArray();
}
usize_t UnitFactory::getParamCount(id1_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_paramNames.size();
    }
    return 0;
}

UnitFactory::NameArray UnitFactory::getPortNames(id1_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_portNames;
    }
    return UnitFactory::NameArray();
}
usize_t UnitFactory::getPortCount(id1_t type, id1_t ptype){

    if ( m_reg.find(type) != m_reg.end() && ptype < 4 ){

        return m_reg[type].m_portCount[ptype];
    }
    return 0;
}

std::string UnitFactory::buildPortName(AbstractEffectUnit* source,
                                    id1_t ptype, usize_t idx){

    id1_t type = source->getType();
    
    if ( m_reg.find(type) != m_reg.end() ){

        usize_t pidx = 0;
        for( usize_t i = 0; i < ptype; i++){

            pidx += m_reg[type].m_portCount[i];
        }
        
        std::string ename = source->getJACKUnit(pidx+idx)->getName();
        
        return ename + ":" + m_reg[type].m_portNames[pidx+idx];
    }
    return std::string();
}

/* ****************************************************************** */
/* ******************* UnitFactoryRegister Entry ******************** */
/* ****************************************************************** */
UnitFactory::UnitFactoryReg::UnitFactoryReg():
    m_name(""),
    m_builder(NULL),
    m_paramNames(NameArray()),
    m_portNames(NameArray())
{
    m_portCount[0] = 0;
    m_portCount[1] = 0;
    m_portCount[2] = 0;
    m_portCount[3] = 0;
}
UnitFactory::UnitFactoryReg::UnitFactoryReg(std::string name, builder_f builder,
                    const std::string* paramNames, usize_t paramCount,
                    const std::string* portNames, 
                    usize_t ai, usize_t ao, usize_t mi, usize_t mo):
    m_name(name),
    m_builder(builder),
    m_paramNames(NameArray()),
    m_portNames(NameArray())
{
    m_portCount[0] = ai;
    m_portCount[1] = ao;
    m_portCount[2] = mi;
    m_portCount[3] = mo;
    
    // Copy Common param names and effect's param names
    const usize_t cpc = EUCST::C_PARAM_COUNT;
    
    for ( usize_t i = 0; i < cpc; i++ ){

        m_paramNames.push_back(EUCST::C_NAME[i]);
    }
    for ( usize_t i = cpc; i < paramCount + cpc ; i++ ){

        m_paramNames.push_back(paramNames[i-cpc]);
    }

    // Copy Ports Names
    for ( usize_t i = 0; i < ai + ao + mi + mo; i++ ){

        m_portNames.push_back(portNames[i]);
    }
}

void UnitFactory::printRegister(){

    printf("Available Effects : \n");
    for ( auto& cur : m_reg ){

        printf("   - %3u : %-13s\n", cur.first, cur.second.m_name.c_str());
    }
    printf("End\n");
}
