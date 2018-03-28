/**********************************************************************
 * @file Label.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for print text on the gui
 **********************************************************************/
#include "Label.h"
#include "gui/utils/utils.h"
#include "gui/utils/render.h"

/**
 * @brief GLabel constructor, initialize attributes
 * @param text, text that will be printed
 * @param r, rectanle representing the Object colision box
 * @param visible, true if the object has to be drawn
 * @param isV, true if the object is volatile
 * @param ctx, Pointer to the graphic Context
 **/
GLabel::GLabel(std::string text, SDL_Rect r, bool visible , bool isV
            ,int align
            ,GraphiContext* ctx):
    GraphicObj(r, visible, false, false, isV, ctx)
    ,_text(text)
    ,_textBox()
    ,_textColor()
    ,_align(align)
{
    this->updateTextBox();
}

/**
 * @brief move, function to move the object to given position
 **/
void GLabel::move(int x, int y) {

    this->GraphicObj::move(x, y);
    this->updateTextBox();
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void GLabel::render(SDL_Surface* screen) {

    this->GraphicObj::render(screen);
    gutils::drawText(_context->_font, _text.c_str(), _textBox, _textColor, screen);
}
void GLabel::render(SDL_Surface* screen, SDL_Rect rect) {

    this->GraphicObj::render(screen, rect);
    gutils::drawText(_context->_font, _text.c_str(), _textBox, _textColor, screen);
}

/**
 * @brief setTextColor, method to change the color of the text
 **/
void GLabel::setTextColor(SDL_Color c) {

    _textColor = c;
}

/**
 * @brief setText, method to change the text displayed by the Label
 **/
void GLabel::setText(std::string text) {

    _text = text;
    this->updateTextBox();
}
/**
 * @brief text, method to get the curent text of the Label
 **/
std::string GLabel::text() const {

    return _text;
}

/**
 * @brief updateTextBox, method called when the object has moved
 *  or when the text has changed, update the text box
 **/
void GLabel::updateTextBox() {

    _textBox =  gutils::alignRectangle(
                    gutils::textRectangle(_context->_font, _text.c_str())
                    ,_box, _align);
}
