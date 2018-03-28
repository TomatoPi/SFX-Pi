/**********************************************************************
 * @file Container.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing the SuperClass of all Gui objects that contains other
 *  objects
 **********************************************************************/
#include "Container.h"

/**
 * @brief Container Constructor, initialise attributes
 * @param parent, Pointer to the object that contains this one
 **/
Container::Container(SDL_Rect r, bool visible, bool isv
        , GraphiContext* ctx):
    GraphicObj(r, visible, false, true, isv, ctx)
    ,_childs()
{
}
Container::~Container() {

    for (auto& child : _childs) {

        delete child;
    }
}

/**
 * @brief move, function to move the object to given position
 **/
void Container::move(int x, int y) {

    int dx = x - _box.x;
    int dy = y - _box.y;
/*
    printf("Move Container : target : %i %i : offsets : %i %i\n"
            , x, y, dx, dy);*/
    
    for (auto& child : _childs) {

        //printf("Move Child ");

        SDL_Rect r = child->box();
        child->move(r.x+dx, r.y+dy);
    }
    this->GraphicObj::move(x, y);
}

/**
 * @brief childAt, function that returns a pointer to the child
 *  Object at given coordinates
 * @return nullptr if no object found
 **/
GraphicObj* Container::childAt(int x, int y) const {

    for (auto& child : _childs) {

        if (child->isClicked(x, y)) {

            if (child->isContainer())
                return dynamic_cast<Container*>(child)->childAt(x, y);
                
            if (child->isClickable())
                return child;
        }
    }
    return nullptr;
}

/**
 * @brief addChild, function to add a child to this container
 * This function will resize the container if added object is
 *  out of bounds
 * @return 0 on success
 **/
int Container::addChild(GraphicObj* child, bool resize) {

    _childs.push_back(child);
    child->setParent(this);

    if (resize) {

        SDL_Rect cbox = child->box();

        // If child exceed from left
        if (cbox.x < _box.x) {

            _box.w += _box.x - cbox.x;
            _box.x = cbox.x;
        }
        
        // If child exceed from top
        if (cbox.y < _box.y) {

            _box.h += _box.y - cbox.y;
            _box.y = cbox.y;
        }

        // If child exceed from right
        if ((cbox.x + cbox.w) > (_box.x + _box.w)) {

            _box.w = cbox.x + cbox.w - _box.x;
        }

        // If child exceed from bottom
        if ((cbox.y + cbox.h) > (_box.y + _box.h)) {

            _box.h = cbox.y + cbox.h - _box.y;
        }
    }
    return 0;
}

/**
 * @brief removeChild, function to remove a child from this
 *  container
 * @param id, Id of the child to remove
 * @warning the user is responsible to free the child after remove
 * @return 0 on success
 **/
int Container::removeChild(GraphicObj* child, bool resize) {

    if (child->parent() == this) {

        _childs.remove(child);
        return 0;
    }
    return 1;
}

/**
 * @brief childs, method to get list of childs in this container
 **/
std::list<GraphicObj*> Container::childs() const {

    return _childs;
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void Container::render(SDL_Surface* screen) {

    this->GraphicObj::render(screen);
    this->renderChilds(screen);
}
void Container::render(SDL_Surface* screen, SDL_Rect rect) {

    this->GraphicObj::render(screen, rect);
    this->renderChilds(screen, rect);
}

/**
 * @brief clear, method to delete all childs contained inside
 *  the container
 **/
void Container::clear(bool doDelete) {

    for (auto& child : _childs) {
        
        if(doDelete)delete child;
    }
    _childs.clear();
}

/**
 * @brief renderChilds, function called to render all containers
 *  childs
 **/
void Container::renderChilds(SDL_Surface* screen) {

    for (auto& child : _childs) {
        
        if (child->isVisible()) child->render(screen);
    }
}
void Container::renderChilds(SDL_Surface* screen, SDL_Rect rect) {

    for (auto& child : _childs) {
        
        if (child->isVisible()) child->render(screen, rect);
    }
}
