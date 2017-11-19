/**********************************************************************
 * @file UIManager.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Core object managing footswitches leds and all front pannel of SFXPi
 **********************************************************************/
#include "UIManager.h"
/* ****************************** Const ***************************** */
NotificationList UIManager::m_notif = NotificationList();
/**
 * Function that setup all io Managers
 * Must'n be called before io registration
 **/
int UIManager::create(){

    printf("UI-Manager : Start ...\n");
    
    CommandManager::create();

    printf("\n");
    
    LogicManager::create();

    return 0;
}
int UIManager::kill(){

    printf("UI-Manager : Stop ...\n");
    
    CommandManager::kill();

    printf("\n");
    
    LogicManager::kill();

    return 0;
}

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
void UIManager::update(){

    // Update Logical inputs
    LogicManager::updateInputs(m_notif);

    // Update Analog inputs

    // Perform registered Notifications
    for ( auto & notif : m_notif ){

        id2_t tid = notif.target() & SFXP::ID_MASK;

        if ( tid == (SFXP::ID_SEQ & SFXP::ID_MASK) ){

            printf("Notification for CommandManager\n");
            CommandManager::pushNotification(notif);
        }
        else if ( tid == (SFXP::ID_LED & SFXP::ID_MASK) ){

            printf("Notification for LogicManager\n");
            LogicManager::pushNotification(notif);
        }
        else{

            printf("UI-Manager : Error : Unknown Notification target : 0x%04x\n"
                ,tid);
        }
    }

    LogicManager::updateOutputs();
    
    m_notif.clear();
}
