/**********************************************************************
 * @file FootSwitch.cc
 * @author TomatoPi
 * @version 1.2
 *
 * FootSwitch file
 * A footswitch can have three modes : push, pull or push-pull
 * defining when button's callback is activated
 **********************************************************************/
#include "Footswitch.h"

#define NAME ("Footswitch")
using namespace SFXP;

/**
 * Create footswitch at given adress
 * Created footswitch with this function will not have visual
 * indicator
 **/
Footswitch::Footswitch(id2_t id, std::string name):
    _id(id),
    _name(name),
    _iid(SFXP::ErrId2),
    _hasi(false)
{
}
/**
 * Create a footswitch with a visual indicator
 **/
Footswitch::Footswitch(id2_t id, std::string name, id2_t lid):
    _id(id),
    _name(name),
    _iid(lid),
    _hasi(true)
{
}

/**
 * Get visual indicator ID associated with this footswitch
 **/
bool Footswitch::hasVisual() const{

    return _hasi;
}
id2_t Footswitch::getVisualID() const{

    return _iid;
}

std::string Footswitch::getName() const{

    return _name;
}

id2_t Footswitch::getID() const{

    return _id;
}

void Footswitch::print() const{

    SFXPlog::log(NAME);
    printf(" ID( 0x%04x ) : \"%s\"\n", _id, _name.c_str());
}
