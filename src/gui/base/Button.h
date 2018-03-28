/**********************************************************************
 * @file Button.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for intercept user clicks
 **********************************************************************/
#ifndef DEF_GUI_BUTTON_H
#define DEF_GUI_BUTTON_H

#include "gui/base/Clickable.h"
#include "gui/base/Label.h"

class GButton : public Clickable {

    public :

        /**
         * @brief GButton constructor, initialize attributes
         * @param text, text that will be printed
         * @param r, rectanle representing the Object colision box
         * @param visible, true if the object has to be drawn
         * @param isV, true if the object is volatile
         * @param ctx, Pointer to the graphic Context
         * @param callback, pointer to the onclick callback function
         **/
        GButton(std::string text, SDL_Rect r, bool visible , bool isV
            ,GraphiContext* ctx
            ,onClicked_f callback);
        ~GButton();
            
        /**
         * @brief move, function to move the object to given position
         **/
        virtual void move(int x, int y);

        /**
         * @brief render, function called when the window is beeing
         *  updated, draw the Object on given Surface
         * This default function draw a blank Rect on the screen
         * @param screen, Surface witchon the object will be drawn
         **/
        virtual void render(SDL_Surface* screen);
        virtual void render(SDL_Surface* screen, SDL_Rect rect);

        /**
         * @brief setBackgroundColor, method to change the background
         *  color of the Object
         * @note may not work on objects that completly override the
         *  render method
         **/
        virtual void setBackgroundColor(SDL_Color c);

        /**
         * @brief label, method to get the GLabel of the button
         **/
        GLabel* label() const;

    protected :

        GLabel* _label;
};

class IntButton : public GButton {

    public :
    
        /**
         * @brief IntButton constructor, initialize attributes
         * @param text, text that will be printed
         * @param r, rectanle representing the Object colision box
         * @param visible, true if the object has to be drawn
         * @param isV, true if the object is volatile
         * @param value, a value that is stored within the button
         * @param ctx, Pointer to the graphic Context
         * @param callback, pointer to the onclick callback function
         **/
        IntButton(std::string text, SDL_Rect r, bool visible , bool isV
            ,int value,GraphiContext* ctx
            ,onClicked_f callback);

        /**
         * @brief setValue, method to change the stored value of the button
         **/
        virtual void setValue(int v);
        /**
         * @brief value, method that return stored value of the button
         **/
        int value() const;

    protected :

        int _value;
};

#endif
