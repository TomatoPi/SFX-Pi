/**********************************************************************
 * @file UIManager.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core object managing footswitches leds and all front pannel of SFXPi
 **********************************************************************/
#ifndef DEF_UI_MANAGER_H
#define DEF_UI_MANAGER_H

#include <map>

#include "../Const.h"

#include "./command/CommandManager.h"
#include "./logic/LogicManager.h"

class UIManager{

    public :

            
    public :

        /**
         * Function that setup all io Managers
         * Must'n be called before io registration
         **/
        static int create();
        static int kill();

        /**
         * Function called at each program cycle
         * Update SFXP front pannel
         * 
         * - Update LogicManager Footswitches
         * - Update AnalogManager Potentiometers
         * - Compute command Notifications
         * - Compute Led update Notifications
         * - Compute Display Notifications
         * - End
         **/
        static void update();

    private :

        /* **** General Stuff **** */
        static NotificationList m_notif;
};

#endif
