/**********************************************************************
 * @file Button.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for intercept user clicks
 **********************************************************************/
#include "Button.h"
#include "gui/colors.h"

/**
 * @brief GButton constructor, initialize attributes
 * @param text, text that will be printed
 * @param r, rectanle representing the Object colision box
 * @param visible, true if the object has to be drawn
 * @param isV, true if the object is volatile
 * @param ctx, Pointer to the graphic Context
 * @param callback, pointer to the onclick callback function
 **/
GButton::GButton(std::string text, SDL_Rect r, bool visible , bool isV
        ,GraphiContext* ctx
        ,onClicked_f callback):
    Clickable(r, visible, isV, ctx, callback)
    ,_label(new GLabel(text
            ,gutils::alignRectangle(
                gutils::getRelativeRectangle(r, 0, 0, r.w - 2*_bsize, r.h - 2*_bsize)
                ,r, 0)
            , visible, isV, 0, ctx))
{
    this->setBackgroundColor(guicst::BUTTON_COLOR);
    _label->setTextColor(guicst::BUTTON_TXT_COLOR);
    _label->setBorderSize(0);
}
    
GButton::~GButton() {

    delete _label;
}
    
/**
 * @brief move, function to move the object to given position
 **/
void GButton::move(int x, int y) {

    this->GraphicObj::move(x, y);
    _label->move(x + _bsize, y + _bsize);
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void GButton::render(SDL_Surface* screen) {

    this->GraphicObj::render(screen);
    _label->render(screen);
}
void GButton::render(SDL_Surface* screen, SDL_Rect rect) {

    this->GraphicObj::render(screen, rect);
    _label->render(screen, rect);
}

/**
 * @brief setBackgroundColor, method to change the background
 *  color of the Object
 * @note may not work on objects that completly override the
 *  render method
 **/
void GButton::setBackgroundColor(SDL_Color c) {

    this->GraphicObj::setBackgroundColor(c);
    _label->setBackgroundColor(c);
}

/**
 * @brief label, method to get the GLabel of the button
 **/
GLabel* GButton::label() const {

    return _label;
}

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
IntButton::IntButton(std::string text, SDL_Rect r, bool visible , bool isV
            ,int value,GraphiContext* ctx
            ,onClicked_f callback):
    GButton(text, r, visible, isV, ctx, callback)
    ,_value(value)
{
}

/**
 * @brief setValue, method to change the stored value of the button
 **/
void IntButton::setValue(int v) {

    _value = v;
}
/**
 * @brief value, method that return stored value of the button
 **/
int IntButton::value() const {

    return _value;
}
