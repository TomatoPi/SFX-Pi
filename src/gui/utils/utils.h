/**********************************************************************
 * @file utils.h
 * @author TomatoPi
 * @version 1.0
 *
 * Utility functions for Gui
 **********************************************************************/
#ifndef DEF_UTILS_GUI_H
#define DEF_UTILS_GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace gutils {

    static const int VA_CENTER  = 0x01;
    static const int VA_TOP     = 0x02;
    static const int VA_BOTTOM  = 0x04;

    static const int HA_CENTER  = 0x08;
    static const int HA_LEFT    = 0x10;
    static const int HA_RIGHT   = 0x20;

    /**
     * @brief buildRectangle, function to create an SDL_Rect with given
     *  size
     **/
    static SDL_Rect buildRectangle(int x, int y, int w, int h)
    {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;

        return r;
    }

    /**
     * @brief alignRectangle, function to crate an SDL_Rect with given
     *  alignement relative to an other one
     * @param r, the rectangle to align
     * @param rel, the relative rectangle for alignement
     * @param align, a list of flags concatenated with | operator
     * @note the default alignement is HA_CENTER | VA_CENTER, centered
     *  verticaly and horizontaly
     **/
    static SDL_Rect alignRectangle(SDL_Rect r, SDL_Rect rel, int align)
    {
        if (align & VA_TOP) {
            
            r.y = rel.y;
        }
        else if (align & VA_BOTTOM) {

            r.y = rel.y + rel.h - r.h;
        }
        else { // (align & VA_CENTER)

            r.y = rel.y + (rel.h - r.h)/2;
        }
        
        if (align & HA_LEFT) {

            r.x = rel.x;
        }
        else if (align & HA_RIGHT) {

            r.x = rel.x + rel.w - r.w;
        }
        else { // (align & HA_CENTER)

            r.x = rel.x + (rel.w - r.w)/2;
        }

        return r;
    }

    /**
     * @brief getWindowRectangle, function to get an SDL_rect with
     *  the size of the window
     **/
    static SDL_Rect getWindowRectangle(SDL_Window* window)
    {
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        SDL_GetWindowSize(window, &(r.w), &(r.h));
        
        return r;
    }

    /**
     * @brief getRelativeRectangle, function to get a rectangle
     *  inside given one, with relative size and coordinates
     * If param givens are inferior to 1.0f, value is assumed to be
     *  relative
     * If param is supperior to 1.0f, value is assumed to be absolute
     * @param x, abscissa
     * @param y, ordinate
     * @param w, rect's width
     * @param h, rect's height
     **/
    static SDL_Rect getRelativeRectangle(SDL_Rect rect
                , float x, float y, float w, float h)
    {
        SDL_Rect res;
        res.x = (x>1.0f)? x : x*rect.x;
        res.y = (y>1.0f)? y : y*rect.y;
        res.w = (w>1.0f)? w : w*rect.w;
        res.h = (h>1.0f)? h : h*rect.h;

        return res;
    }

    /**
     * @brief clip, function used to contrain a rectangle inside an other
     * @param rect the reclangle to clip
     * @param the rectangle that represent boundaries
     **/
    static SDL_Rect clipRectangle(SDL_Rect rect, SDL_Rect clip) {

        SDL_Rect res;
        res.x = (rect.x<clip.x)?clip.x:rect.x;
        res.y = (rect.y<clip.y)?clip.y:rect.y;
        res.w = (rect.w+rect.x>clip.w+clip.x)?clip.x+clip.w-rect.x:rect.w;
        res.h = (rect.h+rect.y>clip.h+clip.y)?clip.y+clip.h-rect.y:rect.h;

        return res;
    }

    /**
     * @brief centeredRectangle, function to get a rectangle at the
     *  center of given one.
     * @param rect, relative rectangle
     * @param w, width of the wanted rectangle
     * @param h, height of the wanted rectangle
     **//*
    static SDL_Rect centeredRectangle(SDL_Rect rect, int w, int h)
    {
        SDL_Rect res;
        res.x = rect.x + (rect.w - w)/2;
        res.y = rect.y + (rect.h - h)/2;
        res.w = w;
        res.h = h;

        return res;
    }*/

    /**
     * @brief textRectangle, function to get the rectangle that bounds
     *  given text
     **/
    static SDL_Rect textRectangle(TTF_Font* font, const char* text)
    {
        SDL_Rect res;
        TTF_SizeText(font, text, &res.w, &res.h);
        return res;
    }

    static SDL_Rect dialogRectangle(SDL_Window* window) {

        SDL_Rect wdw = gutils::getWindowRectangle(window);
        return  gutils::alignRectangle(
                    gutils::getRelativeRectangle(wdw ,0,0, 0.5f, 0.5f)
                    ,wdw, 0);
    }

    /**
     * @brief centeredTextRectangle, function to get the correct rectangle
     *  to center given text inside given Rectangle
     * @param font, the font used to calculate text size
     * @param text, the texte to center
     * @param rect, the relative rectangle
     **//*
    static SDL_Rect centeredTextRectangle(TTF_Font* font
                , const char* text, SDL_Rect rect)
    {
        return alignRectangle(
                    textRectangle(font, text)
                    ,rect
                    ,HA_CENTER|VA_CENTER);
    }*/

    /**
     * @brief offsetRectangle, function to move a rectangle with given
     *  offsets in pixel
     * @param rect, rectangle to move
     * @param dx, abscissa offset
     * @param dy, ordinate offset
     * @return moved Rectangle
     **/
    static SDL_Rect offsetRectangle(SDL_Rect rect, int dx, int dy)
    {
        rect.x += dx;
        rect.y += dy;
        return rect;
    }

    /**
     * @brief moveRectangle, function to move a rectangle to given
     *  position in pixels
     * @param rect, rectangle to move
     * @param dx, new abscissa
     * @param dy, new ordinate
     * @return moved Rectangle
     **/
    static SDL_Rect moveRectangle(SDL_Rect rect, int dx, int dy)
    {
        rect.x = dx;
        rect.y = dy;
        return rect;
    }

    /**
     * @brief buildColor, functino to build an SDL_Color from the canals
     *  values
     **/
    static SDL_Color buildColor(unsigned char r, unsigned char g, unsigned char b)
    {
        SDL_Color c = {r, g, b};
        return c;
    }
};

#endif
