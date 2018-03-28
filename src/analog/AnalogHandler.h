/**********************************************************************
 * @file AnalogHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing program structure for Analog Inputs
 **********************************************************************/
#ifndef DEF_ANALOG_HANDLER_H
#define DEF_ANALOG_HANDLER_H

#ifdef __ARCH_LINUX__
    #include <wiringPi.h>
    #include <mcp3004.h>
#endif

#include "core/AbstractHandler.h"
#include "core/Event.h"

#include "analog/Potentiometer.h"
#include "preset/Preset.h"

class AnalogHandler : public AbstractHandler {

    public :

        AnalogHandler(bool headlessRun);
        virtual ~AnalogHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent* event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run();

        void clearAnalog();

    private :

        void eventPresetChanged(SFXPEvent* event);

        bool _headless;
        Preset* _preset;
        Potentiometer* _potars[SFXP::MAX_POT];
};

#endif
