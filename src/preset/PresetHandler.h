/**********************************************************************
 * @file PresetHandler.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing program structure for handling presets
 **********************************************************************/
#ifndef DEF_PRESET_HANDLER_H
#define DEF_PRESET_HANDLER_H

#include "log.h"

#include "preset/Preset.h"
#include "preset/PresetParser.h"

#include "core/Event.h"
#include "core/AbstractHandler.h"

class PresetHandler : public AbstractHandler {

    public :

        PresetHandler();
        virtual ~PresetHandler();

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

    private :

        void eventSavePreset(SFXPEvent& event);
        void eventLoadPreset(SFXPEvent& event);

        Preset* _preset;
};

#endif
