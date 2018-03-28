/**********************************************************************
 * @file render.h
 * @author TomatoPi
 * @version 1.0
 *
 * Rendering functions for Gui
 **********************************************************************/
#ifndef DEF_GUI_RENDER_H
#define DEF_GUI_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace gutils {

    /**
     * @brief drawRectangle, function to draw a rectangle on given surface
     * @param r, the rectangle to draw
     * @param c, the color for the rectangle
     * @param screen, the surface where the rectangle will be printed
     **/
    static void drawRectangle(SDL_Rect r, SDL_Color c, SDL_Surface* screen)
    {
        SDL_Surface* rect = SDL_CreateRGBSurface(0, r.w, r.h, 32, 0, 0, 0, 0);
        SDL_FillRect(rect, nullptr, SDL_MapRGB(screen->format, c.r, c.g, c.b));
        SDL_BlitSurface(rect, nullptr, screen, &r);
        SDL_FreeSurface(rect);
    }
    
    /**
     * @brief drawBorderedRectangle, function to draw a rectangle on
     *  given surface, with a border
     * @param r, the rectangle to draw
     * @param c, the color for the rectangle
     * @param bs, border size in pixels
     * @param bc, border color
     * @param screen, the surface where the rectangle will be printed
     **/
    static void drawBorderedRectangle(SDL_Rect r, SDL_Color c
            , int bs, SDL_Color bc
            , SDL_Surface* screen)
    {
        
        // Draw the border
        drawRectangle(r, bc, screen);

        r.w -= 2*bs;
        r.h -= 2*bs;
        r.x += bs;
        r.y += bs;
        // Draw the Rectangle
        drawRectangle(r, c, screen);
    }
    
    /**
     * @brief drawShadowedRectangle, function to draw a rectangle on
     *  given surface, with a shadow
     * @param r, the rectangle to draw
     * @param c, the color for the rectangle
     * @param ss, the shadow size in pixels
     * @param bc, bright color of the shadow
     * @param dc, dark color of the shadow
     * @param screen, the surface where the rectangle will be printed
     **/
    static void drawShadowedRectangle(SDL_Rect r, SDL_Color c
            , int ss, SDL_Color bc, SDL_Color dc
            , SDL_Surface* screen)
    {
        
        // Draw the bright
        drawRectangle(r, bc, screen);

        r.w -= ss;
        r.h -= ss;
        r.x += ss;
        r.y += ss;
        // Draw the dark
        drawRectangle(r, dc, screen);

        r.w -= ss;
        r.h -= ss;
        // Draw the Rectangle
        drawRectangle(r, c, screen);
    }

    /**
     * @brief drawPartialRectangle, function to draw a portion of a
     *  rectangle on given surface.
     * @param src, the size to draw
     * @param dest, the coordinates were to draw
     * @param c, the color for the rectangle
     * @param screen, the surface where the rectangle will be printed
     **/
    static void drawPartialRectangle(SDL_Rect src, SDL_Rect dest
                                    , SDL_Color c, SDL_Surface* screen)
    {
        SDL_Surface* rect = SDL_CreateRGBSurface(0, src.w, src.h, 32, 0, 0, 0, 0);
        SDL_FillRect(rect, nullptr, SDL_MapRGB(screen->format, c.r, c.g, c.b));
        SDL_BlitSurface(rect, nullptr, screen, &dest);
        SDL_FreeSurface(rect);
    }

    /**
     * @brief drawText, function to draw a text on given surface
     * @param font, the font to use for the text
     * @param text, the text to print
     * @param r, the rectangle that contains the text
     * @param color, the color for the rectangle
     * @param screen, the surface where the rectangle will be printed
     **/
    static void drawText(TTF_Font* font, const char* text
                        ,SDL_Rect r, SDL_Color color
                        ,SDL_Surface* screen)
    {
        SDL_Surface* txt = TTF_RenderText_Blended(font, text, color);
        SDL_BlitSurface(txt , nullptr, screen, &r);
        SDL_FreeSurface(txt);
    }
};

#endif
