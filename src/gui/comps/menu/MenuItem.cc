/**********************************************************************
 * @file MenuItem.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing an object representing a menuItem
 **********************************************************************/
#include "MenuItem.h"
#include "gui/utils/utils.h"
#include "gui/colors.h"

/**
 * @brief MenuItem constructor
 **/
MenuItem::MenuItem(std::string text, SDL_Rect r, bool visible
            , GraphiContext* context, GraphicObj* child
            , onClicked_f callback):
    Clickable(r, visible, false, context, callback)
    ,_text(new GLabel(text
                    , gutils::alignRectangle(
                        gutils::getRelativeRectangle(r, 0, 0, r.w-2*_bsize, r.h-2*_bsize)
                        ,r, 0)
                    , visible, false
                    , gutils::HA_LEFT|gutils::VA_CENTER
                    , context))
    ,_subMenu(child)
{
    this->setBackgroundColor(guicst::MENU_ITEM_COLOR);
    this->setBorderColor(guicst::MENU_ITEM_BORDER);
    _text->setTextColor(guicst::MENU_TEXT_COLOR);
    _text->setBorderSize(0);
}
MenuItem::~MenuItem() {

    delete _text;
    if (_subMenu) delete _subMenu;
}

/**
 * @brief move, function to move the object to given position
 **/
void MenuItem::move(int x, int y) {

    this->GraphicObj::move(x, y);
    _text->move(x + _bsize, y + _bsize);
    this->moveSubMenu();
}

/**
 * @brief subMenu, function to get the submenu relative to this
 *  item, nullptr if no menu is linked
 **/
GraphicObj* MenuItem::subMenu() const {

    return _subMenu;
}

/**
 * @brief setSubMenu, function to link a submenu to this Item.
 * This function automatically set the onClick Callback to
 *  openSubMenu() callback
 **/
void MenuItem::setSubMenu(GraphicObj* subMenu) {

    _subMenu = subMenu;
    this->moveSubMenu();
    _clickCallback = MenuItem::openSubMenu;
}

/**
 * @brief openSubMenu, Callback function for MenuItem that own a
 *  subMenu
 **/
SFXPEvent* MenuItem::openSubMenu(Clickable* obj) {

    MenuItem* item = dynamic_cast<MenuItem*>(obj);
    GraphicObj* sub = item->subMenu();
    if (sub) {

        //printf("Open Submenu of : %s\n", item->_text->text().c_str());
        sub->setVisible(true);
        
        item->context()->_top.push_back(sub);
    }

    return nullptr;
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void MenuItem::render(SDL_Surface* screen) {

    this->GraphicObj::render(screen);
    _text->render(screen);
}
void MenuItem::render(SDL_Surface* screen, SDL_Rect rect) {

    this->GraphicObj::render(screen, rect);
    _text->render(screen, rect);
}

/**
 * @brief setBackgroundColor, method to change the background
 *  color of the Object
 * @note may not work on objects that completly override the
 *  render method
 **/
void MenuItem::setBackgroundColor(SDL_Color c) {

    this->GraphicObj::setBackgroundColor(c);
    _text->setBackgroundColor(c);
}

/**
 * @brief function called when the Item is moved
 **/
void MenuItem::moveSubMenu() {

    if (_subMenu) {
        /*
    printf("SubMenu moved from : %i %i\n"
            , _subMenu->box().x, _subMenu->box().y);
*/
    _subMenu->move(_box.x, _box.y + _box.h);
   // printf("   -   To : %i %i\n", _subMenu->box().x, _subMenu->box().y);
    }
}
