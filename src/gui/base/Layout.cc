/**********************************************************************
 * @file Layout.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing a specific container that arrange it content according
 *  to a given layout
 **********************************************************************/
#include "Layout.h"
#include "gui/utils/utils.h"
#include "gui/utils/render.h"
#include "gui/colors.h"

/**
 * @brief Layout interface constructor.
 * initialize layout orientation, margin and alignement
 **/
LayoutInterface::LayoutInterface(int layout, int margin, int align):
    _layout(layout)
    ,_margin(margin)
    ,_align(align)
{
}

/* ****************************************************************** */
/* *************************** Flow Layout ************************** */
/* ****************************************************************** */
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
FlowLayout::FlowLayout(SDL_Rect r, bool visible, bool isv
                    , int layout, int margin, int align
                    , GraphiContext* ctx):
    Container(r, visible, isv, ctx)
    ,LayoutInterface(layout, margin, align)
{
}
FlowLayout::~FlowLayout()
{
}

/**
 * @brief addChild, function to add a child to this container
 * @param child, pointer to the Object to add
 * @param resize, ignored with layouts, they will always resize
 * @return 0 on success
 **/
int FlowLayout::addChild(GraphicObj* child, bool resize) {

    child->setParent(this);
    _childs.push_back(child);

    SDL_Rect cbox = child->box();

    // If child is wider that the Layout, ajust layout size
    if (cbox.w > (_box.w - _margin*2))
        _box.w = cbox.w + _margin*2;
    
    // If child is taller that the Layout, ajust layout size
    if (cbox.h > (_box.h - _margin*2))
        _box.h = cbox.h + _margin*2;
        
    this->rearrange(resize);
    
    return 0;
}

/**
 * @brief removeChild, function to remove a child from this
 *  container
 * @param id, Id of the child to remove
 * @return 0 on success
 **/
int FlowLayout::removeChild(GraphicObj* child, bool resize) {

    if (this->Container::removeChild(child, resize)) return 1;

    this->rearrange(resize);
    return 0;
}

/**
 * @brief rearrange, function called when a child is added to
 *  this Layout
 **/
void FlowLayout::rearrange(bool resize) {
    
    int px = _box.x+_margin;
    int py = _box.y+_margin;

    for (auto& child : _childs) {

        int x, y;
        SDL_Rect r = child->box();

        //printf("BOX : %i, %i, %i, %i \n", r.x, r.y, r.w, r.h);
        //printf("LAYOUT : %i, %i, %i, %i \n", _box.x, _box.y, _box.w, _box.h);
    
        if (_align == ALIGN_TL) {

            x = 0;
            y = 0;
        }
        else if (_align == ALIGN_BR) {

            x = _box.w - r.w;
            y = _box.h - r.h;
        }
        else { // (_align == ALIGN_C)

            x = (_box.w - r.w)/2;
            y = (_box.h - r.h)/2;
        }

        if (_layout == V_LAYOUT) {
            
            //printf("VERTICAL_LAYOUT : %i, %i \n", x+_box.x, py);

            child->move(_box.x+x, py);
            py += r.h + _margin;
        }
        else { // (_layout == H_LAYOUT)
            
            //printf("HORIZONTAL_LAYOUT : %i, %i \n", px, _box.y+y);

            child->move(px, _box.y+y);
            px += r.w + _margin;
        }
    }
    
    if (resize) {

        if (_layout == H_LAYOUT) _box.w = px - _box.x;
        if (_layout == V_LAYOUT) _box.h = py - _box.y;
    }
}

/* ****************************************************************** */
/* ************************* Scroll Layout ************************** */
/* ****************************************************************** */

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
ScrollLayout::ScrollLayout(SDL_Rect r, bool visible, bool isv
            , int layout, int margin, int align
            , GraphiContext* ctx):
    Container(r, visible, isv, ctx)
    ,LayoutInterface(layout, margin, align)
    ,_objs()
    ,_tabs()
    ,_curentTab(_tabs.begin())
    ,_content(nullptr)
    ,_spos(nullptr)
    ,_sneg(nullptr)
{
    // This Create the layout sub container
    _content = new Container(
                gutils::alignRectangle(
                    (_layout == V_LAYOUT)?
                    gutils::getRelativeRectangle(r, 0, 0, 1.0f, 0.8f)
                    :gutils::getRelativeRectangle(r, 0, 0, 0.8f, 1.0f)
                    ,r, 0)
                , true, isv, ctx);

    // Create the top and the bottom Buttons
    if (_layout == V_LAYOUT) {

        _sneg = new GButton("/\\/\\",
                gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0, 0, 1.0f, 0.1f)
                    ,r, gutils::VA_TOP)
                ,visible, isv, ctx, callbackScrollNeg);
        _spos = new GButton("\\/\\/",
                gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0, 0, 1.0f, 0.1f)
                    ,r, gutils::VA_BOTTOM)
                ,visible, isv, ctx, callbackScrollPos);
    }
    else {

        _sneg = new GButton("<<",
                gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0, 0, 0.075f, 1.0f)
                    ,r, gutils::HA_LEFT)
                ,visible, isv, ctx, callbackScrollNeg);
        _spos = new GButton(">>",
                gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0, 0, 0.075f, 1.0f)
                    ,r, gutils::HA_RIGHT)
                ,visible, isv, ctx, callbackScrollPos);
    }

    _sneg->reverseShadow();
    _spos->reverseShadow();

    this->Container::addChild(_sneg, false);
    this->Container::addChild(_content, false);
    this->Container::addChild(_spos, false);
}
ScrollLayout::~ScrollLayout()
{
    // Clear the content object to prevent segfault
    _content->clear(false);

    // Clear all tabs and delete them
    clearTabs();

    // Manualy delete all sub childs
    for (auto& child : _objs) {

        delete child;
    }
}

/**
 * @brief addChild, function to add a child to this container
 * @param child, pointer to the Object to add
 * @param resize, ignored with layouts, they will always resize
 **/
int ScrollLayout::addChild(GraphicObj* child, bool resize) {

    this->_objs.push_back(child);
    this->rearrange(false);
    return 0;
}

/**
 * @brief removeChild, function to remove a child from this
 *  container
 * @param id, Id of the child to remove
 **/
int ScrollLayout::removeChild(GraphicObj* child, bool resize) {

    this->_objs.remove(child);
    this->rearrange(false);
    return 0;
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void ScrollLayout::render(SDL_Surface* screen) {

    this->Container::render(screen);
}
void ScrollLayout::render(SDL_Surface* screen, SDL_Rect rect) {

    this->Container::render(screen, rect);
}

/**
 * @brief setBackgroundColor, method to change the background
 *  color of the Object
 * @note may not work on objects that completly override the
 *  render method
 **/
void ScrollLayout::setBackgroundColor(SDL_Color c) {

    this->Container::setBackgroundColor(c);
    _content->setBackgroundColor(c);
}

/**
 * @brief scroll, function used to change the viewable portion
 *  of the layout
 * @param delta, deplacement in pixels
 **/
void ScrollLayout::scroll(int delta) {

    //printf("Scroll %i\n", delta);

    bool scrolled = false;
    if (canScrollNegative() && delta < 0)
    {
        --_curentTab;
        scrolled = true;
    }
    else if (canScrollPositive() && delta > 0)
    {
        if (++_curentTab == _tabs.end()) --_curentTab;
        scrolled = true;
    }
    
    if (scrolled && _curentTab != _tabs.end()) {

        this->updateScollersColor();
        // Clear the content pane
        _content->clear(false);
        _content->Container::addChild((*_curentTab), false);
    }
}

/**
 * @brief canScrollNegative, method that return true if there
 *  is more stuff to view in positive direction
 **/
bool ScrollLayout::canScrollNegative() const {

    return _tabs.size() != 0 && _curentTab != _tabs.begin();
}
/**
 * @brief canScrollPositive, method that return true if there
 *  is more stuff to view in negative direction
 **/
bool ScrollLayout::canScrollPositive() const {

    if (_tabs.size() == 0) return false;
    
    std::list<Container*>::iterator tmp = _curentTab;
    if (++tmp == _tabs.end()) {

        return false;
    }
    return true;
}

/**
 * @brief rearrange, function called when a child is added to
 *  this Layout
 * This function move all childs of the Layout according to their
 *  order
 **/
void ScrollLayout::rearrange(bool resize) {

    //printf("Rearange ScrollLayout : %i childs\n", _objs.size());
    this->clearTabs();

    SDL_Rect box = _content->box();
    
    int px = box.x+_margin;
    int py = box.y+_margin;

    Container* tab(nullptr);

    for (auto& child : _objs) {

        //printf("   Add a child : %i tabs\n", _tabs.size());

        int x, y;
        SDL_Rect r = child->box();

        //printf("BOX : %i, %i, %i, %i \n", r.x, r.y, r.w, r.h);
        //printf("LAYOUT : %i, %i, %i, %i \n", _box.x, _box.y, _box.w, _box.h);
    
        if (_align == ALIGN_TL) {

            x = _margin;
            y = _margin;
        }
        else if (_align == ALIGN_BR) {

            x = box.w - r.w - _margin;
            y = box.h - r.h - _margin;
        }
        else { // (_align == ALIGN_C)

            x = (box.w - r.w)/2;
            y = (box.h - r.h)/2;
        }

        // if box size has been exceded create a new tab
        int tmpy = (_layout == V_LAYOUT)?py:0;
        int tmpx = (_layout == V_LAYOUT)?0 :px;
        
        if (tmpy + r.h > box.h + box.y || tmpx + r.w > box.w + box.x) {

            //printf("Size exceded create a new Tab : %i+%i %i  : %i+%i %i\n"
            //        , tmpy, r.h, box.h + box.y, tmpx, r.w, box.w + box.x);

            px = box.x+_margin;
            py = box.y+_margin;

            if (tab) _tabs.push_back(tab);
            tab = nullptr;
        }
        
        // If current tab is null create it
        if (!tab) {

            tab = new Container(box, true, false, _context);
            tab->setBackgroundColor(_backg);
        }

        if (_layout == V_LAYOUT) {
            
            //printf("VERTICAL_LAYOUT : %i, %i \n", x+_box.x, py);

            child->move(box.x+x, py);
            py += r.h + _margin;
        }
        else { // (_layout == H_LAYOUT)
            
            //printf("HORIZONTAL_LAYOUT : %i, %i \n", px, _box.y+y);

            child->move(px, box.y+y);
            px += r.w + _margin;
        }
        
        tab->Container::addChild(child, false);
    }
    
    if (tab) _tabs.push_back(tab);
    _curentTab = _tabs.begin();
    this->updateScollersColor();
    
    if (_curentTab != _tabs.end()) {

        //printf("Update Current Tab\n");

        _content->clear(false);
        _content->addChild((*_curentTab), false);
    }
}

SFXPEvent* ScrollLayout::callbackScrollNeg(Clickable* obj) {

    ScrollLayout* layout = dynamic_cast<ScrollLayout*>(obj->parent());
    layout->scroll(-1);
    
    return nullptr;
}
SFXPEvent* ScrollLayout::callbackScrollPos(Clickable* obj) {

    ScrollLayout* layout = dynamic_cast<ScrollLayout*>(obj->parent());
    layout->scroll(1);
    
    return nullptr;
}

/**
 * @brief clear, method to delete all childs contained inside
 *  the container
 **/
void ScrollLayout::clear(bool doDelete) {

    this->_content->clear(false);
    this->clearTabs();
    for (auto& child : _objs) {

        if(doDelete)delete child;
    }
    _objs.clear();
}

/**
 * @brief clearTabs, method called to clear the tabs list without
 *  destroying all child objects
 **/
void ScrollLayout::clearTabs() {

    for (auto& tab : _tabs) {

        tab->clear(false);
        delete tab;
    }
    _tabs.clear();
}


/**
 * @brief updateScollersColor, method called to give special color
 *  to buttons that cannot be used
 **/
void ScrollLayout::updateScollersColor() {

    if (canScrollNegative())
        _sneg->setBackgroundColor(guicst::BUTTON_COLOR);
    else _sneg->setBackgroundColor(guicst::BUTTON_CCK_COLOR);
    
    if (canScrollPositive())
        _spos->setBackgroundColor(guicst::BUTTON_COLOR);
    else _spos->setBackgroundColor(guicst::BUTTON_CCK_COLOR);
}
