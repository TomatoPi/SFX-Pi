/**********************************************************************
 * @file Layout.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing a specific container that arrange it content according
 *  to a given layout
 **********************************************************************/
#ifndef DEF_LAYOUT_H
#define DEF_LAYOUT_H

#include <list>
#include "gui/base/Container.h"
#include "gui/base/Button.h"

class LayoutInterface {

    public :

        /**
         * @brief Layout interface constructor.
         * initialize layout orientation, margin and alignement
         **/
        LayoutInterface(int layout, int margin, int align);

        static const int V_LAYOUT  = 1; /**< Arrange elements verticaly */
        static const int H_LAYOUT  = 2; /**< Arrange elements horizontaly */

        static const int ALIGN_C    = 10;/**< Align object at the center */
        static const int ALIGN_TL   = 11;/**< Align at left or top */
        static const int ALIGN_BR   = 12;/**< Align at bottom or right */

    protected :

        int _layout; /**< Orientation of this Layout */
        int _margin; /**< Margin Inserted between objects */
        int _align;  /**< Alignement for objects smaller that layout */
};

class FlowLayout : public Container, public LayoutInterface {

    public :

        /**
         * @brief Container Constructor, initialise attributes
         * @param r, rectangle that represent boundaries of the object
         * @param visible, true if the object has to be drawn
         * @param isv, true if the object is volatile
         * @param layout, code of the wanted layout orientation
         * @param margin, margin between objects inside the layout
         * @param align, code of the wanted alignement for objects
         * @param ctx, Pointer to the graphic Context
         **/
        FlowLayout(SDL_Rect r, bool visible, bool isv
                    , int layout, int margin, int align
                    , GraphiContext* ctx);
        virtual ~FlowLayout();

        /**
         * @brief addChild, function to add a child to this container
         * @param child, pointer to the Object to add
         * @param resize, ignored with layouts, they will always resize
         **/
        virtual int addChild(GraphicObj* child, bool resize=false);

        /**
         * @brief removeChild, function to remove a child from this
         *  container
         * @param id, Id of the child to remove
         **/
        virtual int removeChild(GraphicObj* child, bool resize=false);

    protected :

        /**
         * @brief rearrange, function called when a child is added to
         *  this Layout
         * This function move all childs of the Layout according to their
         *  order
         **/
        virtual void rearrange(bool resize);
};

/**
 * @warning Non Working Class
 **/
class ScrollLayout : public Container, public LayoutInterface {

    public :
    
        /**
         * @brief Container Constructor, initialise attributes
         * @param r, rectangle that represent boundaries of the object
         * @param visible, true if the object has to be drawn
         * @param isv, true if the object is volatile
         * @param layout, code of the wanted layout orientation
         * @param margin, margin between objects inside the layout
         * @param align, code of the wanted alignement for objects
         * @param ctx, Pointer to the graphic Context
         **/
        ScrollLayout(SDL_Rect r, bool visible, bool isv
                    , int layout, int margin, int align
                    , GraphiContext* ctx);
        virtual ~ScrollLayout();

        /**
         * @brief addChild, method to add a child to this container
         * @param child, pointer to the Object to add
         * @param resize, ignored with layouts, they will always resize
         **/
        virtual int addChild(GraphicObj* child, bool resize=false);

        /**
         * @brief removeChild, method to remove a child from this
         *  container
         * @param id, Id of the child to remove
         **/
        virtual int removeChild(GraphicObj* child, bool resize=false);

        /**
         * @brief render, method called when the window is beeing
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
         * @brief scroll, method used to change the viewable portion
         *  of the layout
         * @param delta, deplacement in pixels
         **/
        void scroll(int delta);

        /**
         * @brief canScrollNegative, method that return true if there
         *  is more stuff to view in positive direction
         **/
        bool canScrollNegative() const;
        /**
         * @brief canScrollPositive, method that return true if there
         *  is more stuff to view in negative direction
         **/
        bool canScrollPositive() const;

        /**
         * @brief clear, method to delete all childs contained inside
         *  the container
         **/
        virtual void clear(bool doDelete=true);

    protected :

        /**
         * @brief rearrange, function called when a child is added to
         *  this Layout
         * This function move all childs of the Layout according to their
         *  order
         **/
        virtual void rearrange(bool resize);

        /**
         * @brief clearTabs, method called to clear the tabs list without
         *  destroying all child objects
         **/
        void clearTabs();

        /**
         * @brief updateScollersColor, method called to give special color
         *  to buttons that cannot be used
         **/
        void updateScollersColor();

        /**
         * List of objects that were added to this layout
         **/
        std::list<GraphicObj*> _objs;
        /**
         * List of tabs on this layout
         **/
        std::list<Container*> _tabs;
        /**
         * Iterator on current tab
         **/
        std::list<Container*>::iterator _curentTab;
        /**
         * Pointer to the viewable container of the layout
         **/
        Container* _content;

        GButton *_spos, *_sneg;
    protected :

        static SFXPEvent* callbackScrollNeg(Clickable* obj);
        static SFXPEvent* callbackScrollPos(Clickable* obj);
};

#endif
