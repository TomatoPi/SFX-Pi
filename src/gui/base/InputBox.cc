/**********************************************************************
 * @file InputBox.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Object to get user text inputs from keyboard
 **********************************************************************/
#include "InputBox.h"
#include "gui/colors.h"

/**
 * @brief Clickable Constructor, initialise attributes
 * @param r, rectangle that represent boundaries of the object
 * @param visible, true if the object has to be drawn
 * @param isv, true if the object is volatile
 * @param ctx, Pointer to the graphic Context
 **/
GIntIBox::GIntIBox(SDL_Rect r, bool visible , bool isv
        ,int min, int max
        , GraphiContext* ctx):
    IntButton(std::to_string(min), r, visible, isv, min, ctx, intInputCallback)
    ,_min(min)
    ,_max(max)
{
    this->setBackgroundColor(guicst::INPUT_INACTIVE_BG);
    _label->setTextColor(guicst::INPUT_INACTIVE_TXT);
}

/**
 * @brief setMin, change minimal accepted value for this box
 **/
void GIntIBox::setMin(int m) {

    _min = m;
    if (_value < m) this->setValue(m);
}
/**
 * @brief min, get current minimal accepted value
 **/
int GIntIBox::min() const {

    return _min;
}

/**
 * @brief setMax, change maximal accepted value for this box
 **/
void GIntIBox::setMax(int m) {

    _max = m;
    if (_value > m) this->setValue(m);
}
/**
 * @brief max, get current maximal accepted value
 **/
int GIntIBox::max() const {

    return _max;
}

/**
 * @brief setValue, method to change the stored value of the button
 **/
void GIntIBox::setValue(int v) {

    if (v >= _min && v <= _max) {

        _value = v;
        _label->setText(std::to_string(v));
    }
}

/**
 * @brief intInputCallback, callback called when an int input is
 *  clicked
 * @warning this callback will stop the main thread until the
 *  input is finished
 **/
SFXPEvent* GIntIBox::intInputCallback(Clickable* obj) {

    SDL_StartTextInput();
    bool quit(false);

    GIntIBox* box = dynamic_cast<GIntIBox*>(obj);
    box->setBackgroundColor(guicst::INPUT_ACTIVE_BG);
    box->label()->setTextColor(guicst::INPUT_ACTIVE_TXT);

    SFXPEvent* resEvent = new SFXPEvent(SFXPEvent::Type::Event_GuiInputInt);
    resEvent->_gui._strDialog = std::to_string(box->value());
    
    while (!quit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.state == SDL_PRESSED) {
                
                if (!box->isClicked(event.button.x, event.button.y)) {

                    quit = true;
                }
            }
        }
        else if (event.type == SDL_KEYDOWN) {

            // Backspace
            if (event.key.keysym.sym == SDLK_BACKSPACE
            && resEvent->_gui._strDialog.size() > 0)
            {
                resEvent->_gui._strDialog.pop_back();
            }
        }
        else if (event.type == SDL_TEXTINPUT) {

            // Character input
            try {
                
                resEvent->_gui._strDialog += event.text.text;
                box->setValue(std::stoi(resEvent->_gui._strDialog));
            }
            catch(const std::invalid_argument& ia) {

                resEvent->_gui._strDialog.pop_back();
                box->setValue(std::stoi(resEvent->_gui._strDialog));
            }
        }
        }

        box->render(SDL_GetWindowSurface(box->context()->_window));
        SDL_UpdateWindowSurface(box->context()->_window);
    }
    SDL_StopTextInput();
    box->setBackgroundColor(guicst::INPUT_INACTIVE_BG);
    box->label()->setTextColor(guicst::INPUT_INACTIVE_TXT);

    // Write the resulting event
    resEvent->_gui._intDialog = box->value();
    return resEvent;
}
