/**********************************************************************
 * @file Container.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui objects that contains other
 *  objects
 **********************************************************************/
#ifndef DEF_CONTAINER_H
#define DEF_CONTAINER_H

#include <list>

#include "gui/base/GraphicObject.h"

class Container : public GraphicObj
{

    public :

        /**
         * @brief Container Constructor, initialise attributes
         * @param parent, Pointer to the object that contains this one
         **/
        Container(SDL_Rect r, bool visible, bool isv, GraphiContext* ctx);
        virtual ~Container();
        
        /**
         * @brief move, function to move the object to given position
         **/
        virtual void move(int x, int y);

        /**
         * @brief childAt, function that returns a pointer to the child
         *  Object at given coordinates
         *  Returned object is garanteed to be clickable
         * @return nullptr if no object found or if founded object is
         *  not clickable
         **/
        GraphicObj* childAt(int x, int y) const;

        /**
         * @brief addChild, function to add a child to this container
         * @param child, pointer to the Object to add
         * @param resize, true for resize the container if added object
         *  is out of bounds
         * @return 0 on success
         **/
        virtual int addChild(GraphicObj* child, bool resize=false);

        /**
         * @brief removeChild, function to remove a child from this
         *  container
         * @param id, Id of the child to remove
         * @warning the user is responsible to free the child after remove
         * @return 0 on success
         **/
        virtual int removeChild(GraphicObj* child, bool resize=false);

        /**
         * @brief childs, method to get list of childs in this container
         **/
        std::list<GraphicObj*> childs() const;

        /**
         * @brief render, function called when the window is beeing
         *  updated, draw the Object on given Surface
         * This default function draw a blank Rect on the screen
         * @param screen, Surface witchon the object will be drawn
         **/
        virtual void render(SDL_Surface* screen);
        virtual void render(SDL_Surface* screen, SDL_Rect rect);

        /**
         * @brief clear, method to delete all childs contained inside
         *  the container
         **/
        virtual void clear(bool doDelete=true);

    protected :

        /**
         * @brief renderChilds, function called to render all containers
         *  childs
         **/
        void renderChilds(SDL_Surface* screen);
        void renderChilds(SDL_Surface* screen, SDL_Rect rect);

        /** Vector of all contained childs */
        std::list<GraphicObj*> _childs;
};

#endif
