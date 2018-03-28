/**********************************************************************
 * @file Label.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for print text on the gui
 **********************************************************************/
#ifndef DEF_GUI_LABEL_H
#define DEF_GUI_LABEL_H

#include <string>
#include "gui/base/GraphicObject.h"

class GLabel : public GraphicObj {

    public :

        /**
         * @brief GLabel constructor, initialize attributes
         * @param text, text that will be printed
         * @param r, rectanle representing the Object colision box
         * @param visible, true if the object has to be drawn
         * @param isV, true if the object is volatile
         * @param align, aligment flags contatenated with | operator
         * @param ctx, Pointer to the graphic Context
         **/
        GLabel(std::string text, SDL_Rect r, bool visible , bool isV
            ,int align
            ,GraphiContext* ctx);
            
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
         * @brief setTextColor, method to change the color of the text
         **/
        void setTextColor(SDL_Color c);

        /**
         * @brief setText, method to change the text displayed by the Label
         **/
        void setText(std::string text);
        /**
         * @brief text, method to get the curent text of the Label
         **/
        std::string text() const;

    private :

        /**
         * @brief updateTextBox, method called when the object has moved
         *  or when the text has changed, update the text box
         **/
        void updateTextBox();

        std::string _text;
        SDL_Rect _textBox;
        SDL_Color _textColor;
        
        int _align;
};

#endif
