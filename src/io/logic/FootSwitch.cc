/**********************************************************************
 * @file FootSwitch.cc
 * @author TomatoPi
 * @version 1.0
 *
 * FootSwitch file
 * A footswitch can have three modes : push, pull or push-pull
 * defining when button's callback is activated
 **********************************************************************/
#include "FootSwitch.h"

/**
 * Create footswitch at given adress
 * Created footswitch with this function will not have visual
 * indicator
 **/
FootSwitch::FootSwitch(id2_t id, std::string name, Mode mode):
    m_id(id),
    m_name(name),
    m_mode(mode),
    m_iid(0),
    m_hasi(false),
    m_targets(std::vector<id2_t>())
{
}
/**
 * Create a footswitch with a visual indicator
 **/
FootSwitch::FootSwitch(id2_t id, std::string name, id2_t lid, Mode mode):
    m_id(id),
    m_name(name),
    m_mode(mode),
    m_iid(lid),
    m_hasi(true),
    m_targets(std::vector<id2_t>())
{
}

/**
 * Add or Remove a target to this footswitch
 **/
int FootSwitch::addTarget(id2_t id){

    if ( SFXP::GlobalIsDebugEnabled ){
        printf("Add Target to given Footswitch : 0x%04x\n", id);
    }

    for ( auto& cur : m_targets ){

        if ( cur == id ){
            return 1;
        }
    }

    m_targets.push_back(id);
    return 0;
}
int FootSwitch::removeTarget(id2_t id){

    for ( size_t i = 0; i < m_targets.size(); i++ ){

        if ( m_targets[i] == id ){

            m_targets.erase(m_targets.begin() + i);
            return 0;
        }
    }
    return 1;
}
std::vector<id2_t> FootSwitch::getTargets() const{

    return m_targets;
}

/**
 * Get visual indicator ID associated with this footswitch
 **/
bool FootSwitch::hasVisual() const{

    return m_hasi;
}
id2_t FootSwitch::getVisualID() const{

    return m_iid;
}

/**
 * Change and get current footswitch operating mode
 **/
void FootSwitch::setMode(Mode mode){

    m_mode = mode;
}
FootSwitch::Mode FootSwitch::getMode() const{

    return m_mode;
}

std::string FootSwitch::getName() const{

    return m_name;
}

id2_t FootSwitch::getID() const{

    return m_id;
}

void FootSwitch::print() const{

    printf("FootSwitch : ID( 0x%04x ) : \"%s\"\n", m_id, m_name.c_str());


    for ( auto & tgt : m_targets ){

        printf("   - Target : 0x%04x\n", tgt);
    }

    printf("End\n");
}
