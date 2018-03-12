/**********************************************************************
 * @file Connection.h
 * @author TomatoPi
 * @version 1.0
 *
 * File provididing connection object, used to store a connection between
 *  two effects
 **********************************************************************/
#ifndef DEF_CONNECTION_H
#define DEF_CONNECTION_H

#include <iostream>
#include "types.h"

struct Connection{

    Connection(SFXP::id1_t si, SFXP::usize_t sp, SFXP::id1_t ti, SFXP::usize_t tp, bool midi=false):
        m_si(si), m_sp(sp), m_ti(ti), m_tp(tp), m_midi(midi) {}
    
    Connection():Connection(SFXP::ErrId1, 0, SFXP::ErrId1, 0) {}

    friend std::ostream& operator<<(std::ostream& os, const Connection& c)
    {
        return os << c.m_si << ":" << c.m_sp
            << ":" << c.m_ti << ":" << c.m_tp
            << ":" << ((c.m_midi)?"MIDI":"Audio");
    }

    friend bool operator<(const Connection& l, const Connection& r)
    {
        return l.m_si < r.m_si || l.m_sp < r.m_sp || l.m_ti < r.m_ti || l.m_tp < r.m_tp || l.m_midi < r.m_midi;
    }

    bool isInvalid() const {

        return m_si == SFXP::ErrId1 || m_ti == SFXP::ErrId1;
    }

    SFXP::id1_t     m_si;
    SFXP::usize_t   m_sp;
    SFXP::id1_t     m_ti;
    SFXP::usize_t   m_tp;

    bool m_midi;
};

#endif
