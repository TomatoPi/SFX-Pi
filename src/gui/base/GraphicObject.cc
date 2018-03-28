/**********************************************************************
 * @file GraphicObject.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui related objects
 **********************************************************************/
#include "GraphicObject.h"
#include "gui/utils/render.h"
#include "gui/colors.h"

/**
 * @brief GraphicObject constructor, initialize attributes
 * @param r, rectanle representing the Object colision box
 * @param visible, true if the object has to be drawn
 * @param clic, true if the object is clickable
 * @param contain, true if the object is a container
 * @param parent, Pointer to the object that contains this one
 **/
GraphicObj::GraphicObj(SDL_Rect r, bool visible, bool clic, bool contain
        , bool isV, GraphiContext* ctx):
    _context(ctx)
    ,_parent(nullptr)
    ,_box(r)
    ,_visible(visible)
    ,_clickable(clic)
    ,_isContainer(contain)
    ,_isVolatile(isV)
    ,_doDestroy(false)
    ,_backg()
    ,_bsize(2)
    ,_bcolor(guicst::DEFAULT_BORDER)
    ,_lcolor(guicst::DEFAULT_LIGHT)
{
}
GraphicObj::~GraphicObj()
{
}

/**
 * @brief context, get the context associated with this object
 **/
GraphiContext* GraphicObj::context() {

    return _context;
}

/**
 * @brief move, function to change position or size of the object
 * @param r, new rectangle representing the object
 **/
void GraphicObj::setBox(SDL_Rect r) {

    _box = r;
}
/**
 * @brief move, function to move the object to given position
 **/
void GraphicObj::move(int x, int y) {
/*
    printf("GraphicOBJ : Move Object from : %i %i -- to %i %i\n"
            ,_box.x, _box.y, x, y);*/

    _box.x = x;
    _box.y = y;
}
/**
 * @brief box, function to get the position and size of the object
 **/
SDL_Rect GraphicObj::box() const {

    return _box;
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void GraphicObj::render(SDL_Surface* screen) {

    gutils::drawShadowedRectangle(_box, _backg, _bsize, _bcolor, _lcolor, screen);
}
void GraphicObj::render(SDL_Surface* screen, SDL_Rect rect) {

    gutils::drawPartialRectangle(rect, _box, _backg, screen);
}

/**
 * @brief setVisible, change isVisible attribute
 **/
void GraphicObj::setVisible(bool v) {

    _visible = v;
}
/**
 * @brief isVisible, function that return true if the object
 *  has to be drawn on the screen
 **/
bool GraphicObj::isVisible() const {

    return _visible;
}

/**
 * @brief setClickable, change isClickable attribute
 **/
void GraphicObj::setClickable(bool c) {

    _clickable = c;
}
/**
 * @brief isClicable, function that return true if the object
 *  raise events on click
 **/
bool GraphicObj::isClickable() const {

    return _clickable;
}

/**
 * @brief isClicked, function that verify if given coordinates
 *  are inside the object
 * @param x, click abscissa
 * @param y, click ordinates
 * @return True if coordinates are within the Object
 **/
bool GraphicObj::isClicked(int x, int y) const {

    return !(x < _box.x || x > _box.x + _box.w
            || y < _box.y || y > _box.y + _box.h);
}

/**
 * @brief isContainer, functino that return true if the object
 *  is a container
 **/
bool GraphicObj::isContainer() const {

    return _isContainer;
}

/**
 * @brief setParent, change the object's parent
 **/
void GraphicObj::setParent(Container* parent) {

    _parent = parent;
}

/**
 * @brief parent, function to get the GraphicObject that contains
 * this one
 **/
Container* GraphicObj::parent() const {

    return _parent;
}

/**
 * @brief isVolatile, return true if the object has to be removed
 *  from the top if it is not longer used, or when clicking outside
 **/
bool GraphicObj::isVolatile() const {

    return _isVolatile;
}

/**
 * @brief isMarkedForDestruction, return true is the object has to be
 *  destroyed.
 * When objects are on the top, this method is used when an object
 *  is clicked to know if the object is no longer usefull and has
 *  to be freed
 **/
bool GraphicObj::isMarkedForDestruction() const {

    return _doDestroy;
}

/**
 * @brief markForDestruction(), method that mark the object for destroy
 **/
void GraphicObj::markForDestruction() {

    _doDestroy = true;
}

/**
 * @brief setBackgroundColor, method to change the background
 *  color of the Object
 **/
void GraphicObj::setBackgroundColor(SDL_Color c) {

    _backg = c;
}

/**
 * @brief getBackgroundColor, method to get the background color
 *  of the object
 **/
SDL_Color GraphicObj::getBackgroundColor() const {

    return _backg;
}

/**
 * @brief setBorderSize, method to change the size of the border
 **/
void GraphicObj::setBorderSize(int size) {

    _bsize = size;
}
/**
 * @brief getBorderSize, method to get the size of the border
 **/
int GraphicObj::getBorderSize() const {

    return _bsize;
}

/**
 * @brief setBorderColor, method to change the color of the border
 *  Change the shadow and the reflection to the same color
 **/
void GraphicObj::setBorderColor(SDL_Color c) {

    _bcolor = c;
    _lcolor = c;
}

/**
 * @brief setShadowColor, method to change the color of the shadow
 **/
void GraphicObj::setShadowColor(SDL_Color c) {

    _bcolor = c;
}
/**
 * @brief getShadowColor, method to get the color of the shadow
 **/
SDL_Color GraphicObj::getShadowColor() const {

    return _bcolor;
}

/**
 * @brief setReflectColor, method to change the color of the reflection
 **/
void GraphicObj::setReflectColor(SDL_Color c) {

    _lcolor = c;
}
/**
 * @brief getReflectColor, method to get the color of the reflection
 **/
SDL_Color GraphicObj::getReflectColor() const {

    return _lcolor;
}

/**
 * @brief reverseShadow, method to reverse positions of shadow
 *  and lighting on this object
 **/
void GraphicObj::reverseShadow() {

    SDL_Color tmp = _lcolor;
    _lcolor = _bcolor;
    _bcolor = tmp;
}
