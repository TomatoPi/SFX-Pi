/**********************************************************************
 * @file Clickable.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui objects that generate events
 *  when clicked
 **********************************************************************/
#ifndef DEF_CLICKABLE_H
#define DEF_CLICKABLE_H

#include "gui/base/GraphicObject.h"
#include "core/Event.h"

class Clickable : public GraphicObj {

    public :

        typedef SFXPEvent* (*onClicked_f)(Clickable*);

        /**
         * @brief Clickable Constructor, initialise attributes
         * @param r, rectangle that represent boundaries of the object
         * @param visible, true if the object has to be drawn
         * @param isv, true if the object is volatile
         * @param ctx, Pointer to the graphic Context
         * @param callback, pointer to the onclick callback function
         **/
        Clickable(SDL_Rect r, bool visible , bool isv, GraphiContext* ctx
                , onClicked_f callback=nullptr);
        virtual ~Clickable();

        /**
         * @brief setClickCallback, function to set the onClickCallback
         *  for this object
         **/
        void setClickCallback(onClicked_f callback);

        /**
         * @brief clicked, function to call when the object has been
         *  clicked.
         * @return Event_ErrorEvent if no callback has been set
         **/
        SFXPEvent* clicked();

        /**
         * @brief render, function called when the window is beeing
         *  updated, draw the Object on given Surface
         * This default function draw a blank Rect on the screen
         * @param screen, Surface witchon the object will be drawn
         **/
        virtual void render(SDL_Surface* screen) = 0;

    protected :

        onClicked_f _clickCallback;
};

#endif
