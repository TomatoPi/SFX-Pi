/**********************************************************************
 * @file EventHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * Object that manage EventSequencers
 **********************************************************************/
#ifndef DEF_EVENT_HANDLER_H
#define DEF_EVENT_HANDLER_H

#include "log.h"
#include "types.h"
#include "preset/Preset.h"

#include "core/Event.h"
#include "core/AbstractHandler.h"

class EventHandler : public AbstractHandler {

    public :
    
        EventHandler();
        virtual ~EventHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent& event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run();

        void clearEvents();
        void printHandler();

    private :

        void eventIOChanged(SFXPEvent& event);
        void eventPresetChanged(SFXPEvent& event);

        void eventAddSeq(SFXPEvent& event);
        void eventRemoveSeq(SFXPEvent& event);
        void eventPrintSeq(SFXPEvent& event);
        void eventAssign(SFXPEvent& event);
        void eventUnassign(SFXPEvent& event);

        Preset* _preset;
    
};

#endif
