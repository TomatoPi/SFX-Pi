/**********************************************************************
 * @file colors.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing constants for GUI design
 **********************************************************************/
#ifndef DEF_GUI_COLORS_H
#define DEF_GUI_COLORS_H

#include "gui/utils/utils.h"

namespace guicst {

    static const SDL_Color DEFAULT_BORDER   = gutils::buildColor(0x04, 0x00, 0x5E);
    static const SDL_Color DEFAULT_LIGHT    = gutils::buildColor(0x44, 0x40, 0x9E);

    static const SDL_Color MAIN_FRAME_COLOR = gutils::buildColor(0x31, 0x31, 0x31);
    
    static const SDL_Color TOOLBAR_COLOR    = gutils::buildColor(0x45, 0x45, 0x45);
    static const SDL_Color MENU_ITEM_COLOR  = gutils::buildColor(0x34, 0x34, 0x34);
    static const SDL_Color MENU_ITEM_BORDER = gutils::buildColor(0x24, 0x24, 0x24);

    static const SDL_Color MENU_TEXT_COLOR  = gutils::buildColor(0x83, 0xA4, 0xBE);

    static const SDL_Color DIALOG_COLOR     = gutils::buildColor(0x38, 0x38, 0x38);
    static const SDL_Color DIALOG_TOP_COLOR = gutils::buildColor(0x25, 0x25, 0x25);
    static const SDL_Color DIALOG_TEXT      = gutils::buildColor(0x83, 0xA4, 0xBE);
    static const SDL_Color DIALOG_BG_COLOR  = gutils::buildColor(0x92, 0xD7, 0xFF);

    static const SDL_Color BUTTON_COLOR     = gutils::buildColor(0x34, 0x34, 0x34);
    static const SDL_Color BUTTON_CCK_COLOR = gutils::buildColor(0x6F, 0x6F, 0x6F);
    static const SDL_Color BUTTON_TXT_COLOR = gutils::buildColor(0x83, 0xA4, 0xBE);

    static const SDL_Color DEBUG_COLOR      = gutils::buildColor(0xFF, 0x67, 0x00);

    static const SDL_Color INPUT_ACTIVE_BG  = gutils::buildColor(0xC5, 0xC9, 0xF5);
    static const SDL_Color INPUT_ACTIVE_TXT = gutils::buildColor(0x24, 0x2F, 0xB5);
    static const SDL_Color INPUT_INACTIVE_BG= gutils::buildColor(0x94, 0x9C, 0xFF);
    static const SDL_Color INPUT_INACTIVE_TXT=gutils::buildColor(0x15, 0x1C, 0x70);
};

#endif
