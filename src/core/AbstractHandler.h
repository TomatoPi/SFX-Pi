/**********************************************************************
 * @file AbstractHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing base object for program's handlers
 **********************************************************************/
#ifndef DEF_ABSTRACT_HANDLER_H
#define DEF_ABSTRACT_HANDLER_H

#include <string>
#include "core/Event.h"

class AbstractHandler {

    public :

        static const int HANDLER_OK = 0;
        static const int UNBUILDED  = 1;
        static const int ERRORED    = 2;

        AbstractHandler(std::string name);
        ~AbstractHandler();

        /**
         * Function to know if the handler contruction is successfull
         * @return non-zero error code if not
         **/
        virtual int errored();

        /**
         * @brief Function to get the current status of an Handler
         * @return status code
         */
        int status();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent* event) = 0;

        /**
         * Function used to push an event to the targeted handler
         * The event will be imediatly processed via pushEvent method
         **/
        void sendEvent(SFXPEvent* event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run() = 0;

        /**
         * Functions to connect the Handlers together to permit direct
         *  event pushing and to avoid slow queueing system
         **/
        void attachLogicHandler(AbstractHandler* l);
        void attachEventHandler(AbstractHandler* e);
        void attachEffectHandler(AbstractHandler* e);
        void attachCommandHandler(AbstractHandler* c);
        void attachPresetHandler(AbstractHandler* p);
        void attachAnalogHandler(AbstractHandler* a);
        void attachGuiHandler(AbstractHandler* g);

        AbstractHandler* logicHandler();
        AbstractHandler* eventHandler();
        AbstractHandler* effectHandler();
        AbstractHandler* commandHandler();
        AbstractHandler* presetHandler();
        AbstractHandler* analogHandler();
        AbstractHandler* guiHandler();

    protected :

        std::string _name;
        int _status;

        AbstractHandler* _logicHandler;
        AbstractHandler* _eventHandler;
        AbstractHandler* _effectHandler;
        AbstractHandler* _commandHandler;
        AbstractHandler* _presetHandler;
        AbstractHandler* _analogHandler;
        AbstractHandler* _guiHandler;
};

#endif
