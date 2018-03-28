/**********************************************************************
 * @file Clickable.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui objects that generate events
 *  when clicked
 **********************************************************************/
#include "Clickable.h"

/**
 * @brief Clickable Constructor, initialise attributes
 * @param r, rectangle that represent boundaries of the object
 * @param visible, true if the object has to be drawn
 * @param isv, true if the object is volatile
 * @param ctx, Pointer to the graphic Context
 * @param callback, pointer to the onclick callback function
 **/
Clickable::Clickable(SDL_Rect r, bool visible, bool isv, GraphiContext* ctx
                    ,onClicked_f callback):
    GraphicObj(r, visible, true, false, isv, ctx)
    ,_clickCallback(callback)
{
}
Clickable::~Clickable()
{
}

/**
 * @brief setClickCallback, function to set the clickCallback
 *  for this object
 **/
void Clickable::setClickCallback(onClicked_f callback) {

    _clickCallback = callback;
}

/**
 * @brief clicked, function to call when the object has been
 *  clicked.
 * @return Event_ErrorEvent if no callback has been set
 **/
SFXPEvent* Clickable::clicked() {

    if (_clickCallback) return (*_clickCallback)(this);
    return nullptr;
}
