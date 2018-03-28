/**********************************************************************
 * @file MenuItem.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing an object representing a menuItem
 **********************************************************************/
#ifndef DEF_MENU_ITEM_H
#define DEF_MENU_ITEM_H

#include "gui/base/Clickable.h"
#include "gui/base/Context.h"
#include "gui/base/Label.h"

class MenuItem : public Clickable{

    public :

        /**
         * @brief MenuItem constructor
         **/
        MenuItem(std::string text, SDL_Rect r, bool visible
            , GraphiContext* context, GraphicObj* child
            , onClicked_f callback);
        virtual ~MenuItem();
        
        /**
         * @brief move, function to move the object to given position
         **/
        virtual void move(int x, int y);

        /**
         * @brief subMenu, function to get the submenu relative to this
         *  item, nullptr if no menu is linked
         **/
        GraphicObj* subMenu() const;

        /**
         * @brief setSubMenu, function to link a submenu to this Item.
         * This function automatically set the onClick Callback to
         *  openSubMenu() callback
         **/
        void setSubMenu(GraphicObj* subMenu);

        /**
         * @brief openSubMenu, Callback function for MenuItem that own a
         *  subMenu
         **/
        static SFXPEvent* openSubMenu(Clickable* obj);

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

    protected :

        /**
         * @brief function called when the Item is moved
         **/
        void moveSubMenu();

        /** Name of the Item */
        GLabel* _text;
        
        /** Pointer to the related subMenu */
        GraphicObj* _subMenu;
};

#endif
