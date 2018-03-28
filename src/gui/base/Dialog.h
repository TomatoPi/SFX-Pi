/**********************************************************************
 * @file Dialog.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for pop up and ask user inputs
 **********************************************************************/
#ifndef DEF_GUI_DIALOG_H
#define DEF_GUI_DIALOG_H

#include <vector>
#include <string>
#include "gui/base/Base.h"

class Dialog : public Container {

    public :

        /**
         * @brief Dialog constructor, create a dialog frame to inform
         *  or get an input from the user
         * @param title, name of the dialog frame
         * @param content, content of the Dialog
         * @param r, rectanle representing the Object colision box
         * @param ctx, Pointer to the graphic Context
         * @param okCallback, pointer to the onclick function to call
         *  when the ok button is pressed, if this callback is null
         *  the function is set to callbackDefaultOk
         * @param cancelCallback, pointer to the onclick function to call
         *  when the cancel button is pressed, if this callback is null
         *  no cancel button will be displayed
         * @warning passing null content to this constructor will cause
         *  segfault
         **/
        Dialog(std::string title, std::string okName, std::string clName
            , Container* content
            ,SDL_Rect r, GraphiContext* ctx
            ,Clickable::onClicked_f okCallback
            ,Clickable::onClicked_f cancelCallback);

        /**
         * @brief render, function called when the window is beeing
         *  updated, draw the Object on given Surface
         * This default function draw a blank Rect on the screen
         * @param screen, Surface witchon the object will be drawn
         **/
        virtual void render(SDL_Surface* screen);
        virtual void render(SDL_Surface* screen, SDL_Rect rect);

        /**
         * @brief content, method to get the content of the DialogBox
         **/
        Container* content() const;

    protected :

        Container* _content;

    public :

        /**
         * @brief BoolDialog, function to pop a dialog that ask for
         *  a validate or cancel, return a bool dependent on user
         * @param title, title of the dialog
         * @param msg, message displayed
         * @param ok, validate button text
         * @param cancel, cancel button text
         * @param ctx, graphical context of the gui
         **/
        static bool BoolDialog(std::string title, std::string msg
                        ,std::string ok, std::string cancel
                        ,GraphiContext* ctx);

        /**
         * @brief FileDialog, function to pop a dialog that ask for
         *  choose a file, return a file name or en empty string
         * @param title, title of the dialog
         * @param ok, validate button text
         * @param cancel, cancel button text
         * @param ctx, graphical context of the gui
         **/
        static std::string FileDialog(std::string title
                        ,std::string ok, std::string cancel
                        ,GraphiContext* ctx);

        /**
         * @brief IntDialog, function to pop a dialog that ask for
         *  choose a number in a given range
         * @param title, title of the dialog
         * @param ok, validate button text
         * @param cancel, cancel button text
         * @param min, minimal accepted value
         * @param max, maximal accepted value
         * @param err, value returned on cancel
         * @param ctx, graphical context of the gui
         **/
        static int IntDialog(std::string title
                        ,std::string ok, std::string cancel
                        ,int min, int max, int err
                        ,GraphiContext* ctx);

    protected :

        /**
         * @brief waitForDialog, function that upate the screen and wait
         *  for the user to click on a clickable on the dialog
         * @param dialog, the dialog that is waiting for user
         * @return the event returned by the dialog
         **/
        static SFXPEvent* waitForDialog(Dialog* dialog);

        /**
         * @brief callbackDefaultOk, default callback for the ok button
         *  of dialogs
         **/
        static SFXPEvent* callbackDefaultOk(Clickable* obj);
        /**
         * @brief callbackDefaultCancel, default callback for the cancel
         *  button of dialogs
         **/
        static SFXPEvent* callbackDefaultCancel(Clickable* obj);

        /**
         * @brief callbackSelectFile, default callback for select file
         *  buttons in dialogs
         **/
        static SFXPEvent* callbackSelectFile(Clickable* obj);
};

#endif
