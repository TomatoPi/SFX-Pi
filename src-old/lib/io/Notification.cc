/**********************************************************************
 * @file Notification.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Message System core file
 **********************************************************************/
#include "Notification.h"

Notification::Notification(SFXP::id2_t source, SFXP::id2_t target, NotificationType notif):
    m_source(source),
    m_target(target),
    m_type(notif)
{
}

SFXP::id2_t Notification::source() const{

    return m_source;
}
SFXP::id2_t Notification::target() const{

    return m_target;
}
Notification::NotificationType Notification::type() const{

    return m_type;
}

bool operator< ( const Notification & n1, const Notification & n2 ){

    return n1.source() < n2.source() || n1.target() < n2.target();
}
