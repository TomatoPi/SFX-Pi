/**********************************************************************
 * @file PresetHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * Object that will store and manage current preset
 **********************************************************************/
#ifndef DEF_PRESET_HANDLER_H
#define DEF_PRESET_HANDLER_H

#include "handler/AbstractHandler.h"

#include "lib/preset/Preset.h"
#include "lib/preset/PresetParser.h"

class PresetHandler : public AbstractHandler {

    public :

        PresetHandler();
        ~PresetHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent& event);

    private :

        /**
         * Function called when a load event is received
         **/
        void eventLoadPreset(SFXPEvent& event);
        /**
         * Function called when a save event is received
         **/
        void eventSavePreset(SFXPEvent& event);

        Preset* _curentPreset;
};

#endif
