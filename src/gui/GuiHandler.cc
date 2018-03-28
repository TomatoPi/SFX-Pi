/**********************************************************************
 * @file GuiHandler.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing main editor's handler that create the program's GUI
 **********************************************************************/
#include "GuiHandler.h"
#include "gui/base/GraphicObject.h"

GuiHandler::GuiHandler(bool nogui):AbstractHandler("GuiHandler")
    ,_nogui(nogui)
    ,_preset(nullptr)

    ,_context(nullptr)
    ,_mainContainer(nullptr)

    ,_curentEditableType(GuiEvent::PARAMSET)
{
    if (_nogui) {
        SFXPlog::log(_name) << "NoGuiRun : Doesn't Build the Handler" << '\n';
    }
    else {

        try {

            // Initialize SDL
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {

                throw std::string("Failed Initialize SDL : ")
                        + std::string(SDL_GetError());
            }

            // Initialise TTF
            if (TTF_Init() < 0) {

                throw std::string("Failed Initialize TTF : ")
                        + std::string(TTF_GetError());
            }

            // Get the screen Size
            SDL_Rect display;
            if (SDL_GetDisplayUsableBounds(0, &display)) {

                display.x = SDL_WINDOWPOS_CENTERED;
                display.y = SDL_WINDOWPOS_CENTERED;
                display.w = 800;
                display.h = 600;
            }

            // Create the Graphic Context
            _context = new GraphiContext();
            
            // Create the main Window
            _context->_window = SDL_CreateWindow("SFX-Pi : The Software"
                            ,display.x
                            ,display.y
                            ,display.w
                            ,display.h
                            ,SDL_WINDOW_SHOWN);

            if (!_context->_window) {

                throw std::string("Failed Build MainWindow : ")
                        + std::string(SDL_GetError());
            }

            // Open the Font
            _context->_font = TTF_OpenFont(SFXP::FONT_FILE.c_str(), 18);

            if (!_context->_font) {

                throw std::string("Failed Build Font : \'")
                        + SFXP::FONT_FILE + std::string("\' ")
                        + std::string(TTF_GetError());
            }

            _mainContainer = new MainContainer(_context);

            SDL_UpdateWindowSurface(_context->_window);
            
            _status = HANDLER_OK;
            SFXPlog::log(_name) << "Builded Handler" << '\n';
        }
        catch (std::string const& e) {

            SFXPlog::err(_name) << e << '\n';
            _status = ERRORED;

            // Quit TTF
            TTF_Quit();
            // Quit SDL
            SDL_Quit();
        }
    }
}
GuiHandler::~GuiHandler()
{
    if (!_nogui) {

        // Destroy MainContainer
        if(_mainContainer) delete _mainContainer;

        // Destroy MainFrame
        if (_context->_window) SDL_DestroyWindow(_context->_window);

        if (_context->_font) TTF_CloseFont(_context->_font);

        // Quit TTF
        TTF_Quit();
        // Quit SDL
        SDL_Quit();
    }
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void GuiHandler::pushEvent(SFXPEvent* event)
{
    if (_status == HANDLER_OK) {
    if (_nogui) {
        SFXPlog::log(_name) << "NoGuiRun : GuiHandler Desactivated" << '\n';
    }
    else {
    if (event->_type == SFXPEvent::Type::Event_PresetChanged) {

        this->eventPresetChanged(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (!_preset) {

            SFXPlog::err(_name) << "No Preset Given to the Handler" << '\n';
        }
        else {
                
            SFXPlog::log(_name) << "Handler Initialised and Active" << '\n';
        }
    }
    else {
        switch (event->_type) {

            case SFXPEvent::Type::Event_GuiEditTypeChanged :

                if (_preset) {
                _mainContainer->clearEditableList();
                switch (event->_gui._editableType) {
                    
                    case GuiEvent::PARAMSET :
                        this->loadPresetEffects(_preset);
                        break;
                        
                    case GuiEvent::CONNECTION :
                        this->loadPresetConnections(_preset);
                        break;
                        
                    case GuiEvent::SEQUENCER :
                        this->loadPresetSequencers(_preset);
                        break;
                        
                    case GuiEvent::ALINK :
                        this->loadPresetALink(_preset);
                        break;
                }
                }
                else {

                    SFXPlog::err(_name) << "No Preset Loaded" << '\n';
                }
                break;

            case SFXPEvent::Type::Event_GuiQuit :

                SFXPlog::todo(_name) << "Ask for Save Preset" << '\n';
                SFXPlog::log (_name) << "Quit Button Pressed, Exit" << '\n';
                SFXP::GlobalNeedToExit = true;
                return;
                
            default :
                SFXPlog::err(_name) << "Unhandled Event : " << (*event)
                                    << '\n';
        }
    }
    } // If Gui
    } // If HANDLER_OK
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void GuiHandler::run()
{
    if (!_nogui && _status == HANDLER_OK) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        switch(event.type) {

            case SDL_QUIT :

                SFXPlog::log(_name) << "SDL_Quit Event Received, Exit" << '\n';
                SFXP::GlobalNeedToExit = true;
                return;

            case SDL_MOUSEBUTTONDOWN :

                SDLE_MouseButtonEvent(event.button);
                break;
                
            case SDL_MOUSEBUTTONUP :

                SDLE_MouseButtonEvent(event.button);
                break;
        }
        }

        _mainContainer->render(SDL_GetWindowSurface(_context->_window));

        for (auto& obj : _context->_top) {

            if (obj->isVisible())
            {
                obj->render(SDL_GetWindowSurface(_context->_window));
            }
        }
        
        SDL_UpdateWindowSurface(_context->_window);
    }
}

/* ****************************************************************** */
/* ************************** SFX-Pi Events ************************* */
/* ****************************************************************** */

void GuiHandler::eventPresetChanged(SFXPEvent* event) {

    if (Preset* np = (Preset*)event->_preset._preset) {
        
        _mainContainer->clearEditableList();
        this->loadPresetEffects(np);

        _preset = np;
    }
}

void GuiHandler::loadPresetEffects(Preset* p) {

    for (auto& e : p->_paramSets) {

        GuiEvent event;
        event._editable = (void*)e.second;
        event._editableType = GuiEvent::PARAMSET;

        _mainContainer->addEditable(event);
    }
}
void GuiHandler::loadPresetConnections(Preset* p) {

}
void GuiHandler::loadPresetSequencers(Preset* p) {

}
void GuiHandler::loadPresetALink(Preset* p) {

}

/* ****************************************************************** */
/* *************************** SDL Events *************************** */
/* ****************************************************************** */
void GuiHandler::SDLE_MouseButtonEvent(SDL_MouseButtonEvent& event) {

    if (event.state == SDL_PRESSED) {

        GraphicObj* clicked(nullptr);
        
        if (_context->_top.size() != 0) {

            GraphicObj* obj(nullptr);
            
            // Remove objects from top if they are not clicked
            while ((obj = _context->_top.back()) != 0
                    && !(clicked))
            {
                if (obj->isContainer())
                {
                    clicked = dynamic_cast<Container*>(obj)
                            ->childAt(event.x, event.y);
                }
                if (!(clicked) && obj->isClickable())
                {
                    clicked = dynamic_cast<Clickable*>(obj)
                            ->isClicked(event.x, event.y)
                            ?obj:nullptr;
                }
                if (!clicked && obj->isVolatile())
                {
                    #ifdef __DEBUG__
                    SFXPlog::debug(_name) << "Object Removed From Top"
                                        << '\n';
                    #endif
                    
                    obj->setVisible(false);
                    _context->_top.pop_back();
                }
            }
        }
        if (!clicked) {

            clicked = _mainContainer->childAt(event.x, event.y);
        }
        
        if (clicked) {

            //printf("Click Intercepeted\n");

            Clickable* obj = dynamic_cast<Clickable*>(clicked);
            SFXPEvent* e = obj->clicked();

            while (e) {
                
                /*
                 * Event_GuiInternal are processed directly inside the click
                 *  callback function, so no need to send an event
                 */
                if (e->_type != SFXPEvent::Type::Event_GuiInternal)
                    this->sendEvent(e);

                /*
                 * Check wich event has to be processed next
                 */
                bool next(false);
                if (e->_alwaysOk || e->_successed) next = true;
                else next = false;

                /*
                 * Process the correct event queue
                 */
                if (next) {
                    SFXPEvent::eraseEventChain(e->_nextFail, true);
                    e = e->_nextOk;
                }
                else {
                    SFXPEvent::eraseEventChain(e->_nextOk, true);
                    e = e->_nextFail;
                }
                
                delete e;
            };// While e
        }// If Clicked
    }// If SDL_PRESSED
}// void SDLE_MouseButtonEvent
