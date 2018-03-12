/**********************************************************************
 * @file Preset.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File provididing Preset Object, storing all informations loaded from
 *  a preset file
 **********************************************************************/
#include "Preset.h"

using namespace std;

Preset::Preset()
{
}
Preset::~Preset() {
    
    for (auto & effect : m_effectList) {

        delete effect.second;
        m_effectList.erase(effect.first);
    }

    m_effectList.clear();
    m_connectionList.clear();
}

/**
 * Add and remove an effect from preset
 **/
int Preset::addEffect(EffectPrototype* e) {

    // Prevent Null Pointer
    if (e == NULL) {

        cout << "Preset : Error : Null Effect Passed" << endl;
        return 1;
    }

    // Prevent ID duplication
    if (m_effectList.find(e->getID()) != m_effectList.end()) {

        cout << "Preset : Error : Id : " << e->getID() << " already exist"
            << endl;
        return 1;
    }

    m_effectList[e->getID()] = e;
    return 0;
}
int Preset::removeEffect(SFXP::id1_t id) {

    // Verify that Id exist
    if (m_effectList.find(id) == m_effectList.end()) {

        cout << "Preset : Error : Id Not Found" << endl;
        return 1;
    }

    delete m_effectList[id];
    m_effectList.erase(id);
    
    return 0;
}
Preset::EffectList Preset::getEffectList() const {

    return m_effectList;
}

/**
 * Add and remove an effect from preset
 **/
int Preset::addConnection(Connection c) {

    // Verify that Connection is valid
    if (c.isInvalid()) {

        cout << "Preset : Error : Invalid Connection" << endl;
        return 1;
    }

    // Prevent Connection Duplication
    if (m_connectionList.find(c) != m_connectionList.end()) {

        cout << "Preset : Error : Connection already Exist" << endl;
        return 1;
    }

    m_connectionList.insert(c);
    return 0;
}
int Preset::removeConnection(Connection c) {

    // Verify that Connection Exist
    if (m_connectionList.find(c) == m_connectionList.end()) {

        cout << "Preset : Error : Connection doesn't Exist" << endl;
        return 1;
    }

    m_connectionList.erase(c);
    return 0;
}
Preset::ConnectionList Preset::getConnectionList() const {

    return m_connectionList;
}

/**
 * Add and remove a command sequencer from preset
 **//*
int Preset::addSequencer(CommandSequencer* s) {

    // Prevent Null Pointer
    if (s == NULL) {

        cout << "Preset : Error : Null Sequencer Passed" << endl;
        return 1;
    }

    // Prevent Id Duplication
    if (m_seqList.find(s->getID()) != m_seqList.end()) {

        cout << "Preset : Error : Id : " << s->getID() << " Is Already Used" << endl;
        return 1;
    }

    m_seqList[s->getID()] = s;
    return 0;
}
int Preset::removeSequencer(SFXP::id2_t id) {

    // Verify that Sequencer Exist
    if (m_seqList.find(id) == m_seqList.end()) {

        cout << "Preset : Error : Given Sequencer Doesn't Exist" << endl;
        return 1;
    }
    delete m_seqList[id];
    m_seqList.erase(id);
    
    return 0;
}
Preset::SequencerList Preset::getSequencerList() const {

    return m_seqList;
}*/

/**
 * Add and remove a Footswitch from preset
 **//*
int Preset::addFootSwitch(FootSwitch* f) {

    // Prevent Null Pointer
    if (f == NULL) {

        cout << "Preset : Error : Null FootSwitch Passed" << endl;
        return 1;
    }

    // Prevent Id Duplication
    if (m_footArray.find(f->getID()) != m_footArray.end()) {

        cout << "Preset : Error : Id : " << f->getID() << " Is Already Used" << endl;
        return 1;
    }

    m_footArray[f->getID()] = f;
    return 0;
}
int Preset::removeFootSwitch(SFXP::id2_t id) {

    // Verify that Sequencer Exist
    if (m_footArray.find(id) == m_footArray.end()) {

        cout << "Preset : Error : Given FootSwitch Doesn't Exist" << endl;
        return 1;
    }
    delete m_footArray[id];
    m_footArray.erase(id);
    
    return 0;
}
Preset::FootArray Preset::getFootSwitchList() const {

    return m_footArray;
}
*/
