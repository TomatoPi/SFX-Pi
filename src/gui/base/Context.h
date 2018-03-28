/**********************************************************************
 * @file Context.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing struct that store current graphical informations
 **********************************************************************/
#ifndef DEF_GRAPHIC_CONTEXT_H
#define DEF_GRAPHIC_CONTEXT_H

#include <list>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class GraphicObj;
struct GraphiContext {

    SDL_Window* _window;
    TTF_Font* _font;
    std::list<GraphicObj*> _top;
};

#endif
