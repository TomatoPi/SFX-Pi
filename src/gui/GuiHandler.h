/**********************************************************************
 * @file GuiHandler.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing main editor's handler that create the program's GUI
 **********************************************************************/
#ifndef DEF_GUI_HANDLER_H
#define DEF_GUI_HANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "core/AbstractHandler.h"
#include "core/Event.h"

#include "preset/Preset.h"

#include "gui/base/Base.h"
#include "gui/comps/main/MainContainer.h"

class GuiHandler : public AbstractHandler {

    public :

        GuiHandler(bool nogui);
        virtual ~GuiHandler();

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

    private :
    
        void eventPresetChanged(SFXPEvent* event);

        void SDLE_MouseButtonEvent(SDL_MouseButtonEvent& event);

        void loadPresetEffects(Preset* p);
        void loadPresetConnections(Preset* p);
        void loadPresetSequencers(Preset* p);
        void loadPresetALink(Preset* p);

        bool _nogui;
        Preset* _preset;

        /* ****** SDL Objects ****** */
        GraphiContext* _context;
        MainContainer* _mainContainer;

        /* ****** SFXP Objects ***** */
        int _curentEditableType;
};

#endif
