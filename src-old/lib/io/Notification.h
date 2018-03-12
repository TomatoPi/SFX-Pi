/**********************************************************************
 * @file Notification.h
 * @author TomatoPi
 * @version 1.0
 *
 * Message System core file
 **********************************************************************/
#ifndef DEF_MESSAGE_H
#define DEF_MESSAGE_H

#include <vector>
#include "Const.h"

class Notification{

    public :

        enum NotificationType{
            NullNotification            = 0x00,

            PushNotification            = 0x01,
            PullNotification            = 0x02
        };

        Notification(SFXP::id2_t source, SFXP::id2_t target, NotificationType notif);

        SFXP::id2_t source() const;
        SFXP::id2_t target() const;
        NotificationType type() const;

    private :
    
        SFXP::id2_t         m_source;
        SFXP::id2_t         m_target;
        NotificationType    m_type;
};

bool operator< ( const Notification & n1, const Notification & n2 );

typedef std::vector<Notification> NotificationList;

#endif
