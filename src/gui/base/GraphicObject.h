/**********************************************************************
 * @file GraphicObject.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui related objects
 **********************************************************************/
#ifndef DEF_GRAPHIC_OBJECT_H
#define DEF_GRAPHIC_OBJECT_H

#include <SDL2/SDL.h>
#include "gui/base/Context.h"

class Container;
class GraphicObj {

    public :

        /**
         * @brief GraphicObject constructor, initialize attributes
         * @param r, rectanle representing the Object colision box
         * @param visible, true if the object has to be drawn
         * @param clic, true if the object is clickable
         * @param contain, true if the object is a container
         * @param isV, true if the object is volatile
         * @param ctx, Pointer to the graphic Context
         **/
        GraphicObj(SDL_Rect r, bool visible, bool clic, bool contain
            , bool isV, GraphiContext* ctx);
        virtual ~GraphicObj() = 0;

        /**
         * @brief context, get the context associated with this object
         **/
        GraphiContext* context();

        /**
         * @brief setBox, function to change position and size of the object
         * @param r, new rectangle representing the object
         **/
        void setBox(SDL_Rect r);
        /**
         * @brief move, function to move the object to given position
         **/
        virtual void move(int x, int y);
        /**
         * @brief box, function to get the position and size of the object
         **/
        SDL_Rect box() const;

        /**
         * @brief render, function called when the window is beeing
         *  updated, draw the Object on given Surface
         * This default function draw a blank Rect on the screen
         * @param screen, Surface witchon the object will be drawn
         **/
        virtual void render(SDL_Surface* screen);
        virtual void render(SDL_Surface* screen, SDL_Rect rect);

        /**
         * @brief setVisible, change isVisible attribute
         **/
        void setVisible(bool v);
        /**
         * @brief isVisible, function that return true if the object
         *  has to be drawn on the screen
         **/
        bool isVisible() const;

        /**
         * @brief setClickable, change isClickable attribute
         **/
        void setClickable(bool c);
        /**
         * @brief isClicable, function that return true if the object
         *  raise events on click
         **/
        bool isClickable() const;

        /**
         * @brief isClicked, function that verify if given coordinates
         *  are inside the object
         * @param x, click abscissa
         * @param y, click ordinates
         * @return True if coordinates are within the Object
         **/
        bool isClicked(int x, int y) const;

        /**
         * @brief isContainer, functino that return true if the object
         *  is a container
         **/
        bool isContainer() const;

        /**
         * @brief setParent, change the object's parent
         **/
        void setParent(Container* parent);

        /**
         * @brief parent, function to get the GraphicObject that contains
         * this one
         **/
        Container* parent() const;

        /**
         * @brief isVolatile, return true if the object has to be removed
         *  from the top if it is not longer used, or when clicking outside
         **/
        bool isVolatile() const;

        /**
         * @brief isMarkedForDestruction, return true is the object has
         *  to be destroyed.
         * When objects are on the top, this method is used when an object
         *  is clicked to know if the object is no longer usefull and has
         *  to be freed
         **/
        bool isMarkedForDestruction() const;

        /**
         * @brief markForDestruction(), method that mark the object for destroy
         **/
        void markForDestruction();

        /**
         * @brief setBackgroundColor, method to change the background
         *  color of the Object
         **/
        virtual void setBackgroundColor(SDL_Color c);

        /**
         * @brief getBackgroundColor, method to get the background color
         *  of the object
         **/
        SDL_Color getBackgroundColor() const;

        /**
         * @brief setBorderSize, method to change the size of the border
         **/
        void setBorderSize(int size);
        /**
         * @brief getBorderSize, method to get the size of the border
         **/
        int getBorderSize() const;

        /**
         * @brief setBorderColor, method to change the color of the border
         *  Change the shadow and the reflection to the same color
         **/
        void setBorderColor(SDL_Color c);

        /**
         * @brief setShadowColor, method to change the color of the shadow
         **/
        void setShadowColor(SDL_Color c);
        /**
         * @brief getShadowColor, method to get the color of the shadow
         **/
        SDL_Color getShadowColor() const;

        /**
         * @brief setReflectColor, method to change the color of the reflection
         **/
        void setReflectColor(SDL_Color c);
        /**
         * @brief getReflectColor, method to get the color of the reflection
         **/
        SDL_Color getReflectColor() const;

        /**
         * @brief reverseShadow, method to reverse positions of shadow
         *  and lighting on this object
         **/
        void reverseShadow();

    protected :

        /** Pointer to the context associated with this object */
        GraphiContext* _context;
        
        /** Pointer to the object that contains this one */
        Container* _parent;

        /** SDL_Rect representing the object position on the screen **/
        SDL_Rect _box;

        bool _visible;    /**< True if the object has to be drawn */
        bool _clickable;  /**< True if the object raise an event on clicked */
        bool _isContainer;/**< True if the object is a container */

        bool _isVolatile; /**< True if the object is volatile */
        bool _doDestroy;  /**< True if the object has to be destroyed */

        SDL_Color _backg; /**< Color of the background */

        int       _bsize;   /**< Size of the Border */
        SDL_Color _bcolor;  /**< Color of the Border */
        SDL_Color _lcolor;  /**< Color of the reflection */
};

#endif
