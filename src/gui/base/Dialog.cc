/**********************************************************************
 * @file Dialog.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object for pop up and ask user inputs
 **********************************************************************/
#include "Dialog.h"
#include "gui/utils/utils.h"
#include "gui/colors.h"

#include "gui/base/InputBox.h"

#include "core/FileUtils.h"
#include "log.h"

#define NAME "DialogBox"
#define VERIFY_EVENT(event) if (!(event)) {SFXPlog::err(NAME) << "Null Event Received" << '\n';break;}

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
Dialog::Dialog(std::string title, std::string okName, std::string clName
            , Container* content
            ,SDL_Rect r, GraphiContext* ctx
            ,Clickable::onClicked_f okCallback
            ,Clickable::onClicked_f cancelCallback):
    Container(r, true, false, ctx)
    ,_content(content)
{
    this->setBackgroundColor(guicst::DIALOG_COLOR);
    
    // Create the main Vertical FlowLayout
    FlowLayout* ml = new FlowLayout(r, true, false
                        ,FlowLayout::V_LAYOUT, 5, FlowLayout::ALIGN_C
                        ,ctx);
    ml->setBackgroundColor(guicst::DIALOG_COLOR);
                        
    // Create the Label
    GLabel* label = new GLabel(title
                ,gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0.0f, 0.0f, 1.0f, 0.1f)
                    ,r ,gutils::VA_TOP)
                , true, false, 0, ctx);
    label->setBackgroundColor(guicst::DIALOG_TOP_COLOR);
    label->setTextColor(guicst::DIALOG_TEXT);
    label->reverseShadow();

    // Create the BottomLayout
    FlowLayout* bl = new FlowLayout(
                gutils::alignRectangle(
                    gutils::getRelativeRectangle(r, 0.0f, 0.0f, 1.0f, 0.1f)
                    ,r ,gutils::VA_BOTTOM)
                , true, false
                ,FlowLayout::H_LAYOUT, 5, FlowLayout::ALIGN_C
                ,ctx);
    bl->setBackgroundColor(guicst::DIALOG_TOP_COLOR);
    bl->reverseShadow();

    // Create the Ok Button
    GButton* ok = new GButton(okName
                ,gutils::getRelativeRectangle(r, 0.0f, 0.0f, 0.25f, 0.1f)
                ,true, false, ctx
                ,(okCallback)?okCallback:callbackDefaultOk);

    // If a CancelCallback has been given add a cancel button
    GButton* cancel(nullptr);
    if (cancelCallback) {
        cancel = new GButton(clName
                ,gutils::getRelativeRectangle(r, 0.0f, 0.0f, 0.25f, 0.1f)
                ,true, false, ctx
                ,cancelCallback);
    }

    // Populate the Dialog
    ml->addChild(label, true);
    ml->addChild(content, true);

    if (cancel) bl->addChild(cancel, true);
    bl->addChild(ok, true);
    
    ml->addChild(bl, true);
    this->addChild(ml, true);
}

/**
 * @brief render, function called when the window is beeing
 *  updated, draw the Object on given Surface
 * This default function draw a blank Rect on the screen
 * @param screen, Surface witchon the object will be drawn
 **/
void Dialog::render(SDL_Surface* screen) {

    this->Container::render(screen);
}
void Dialog::render(SDL_Surface* screen, SDL_Rect rect) {

    this->Container::render(screen, rect);
}

/**
 * @brief content, method to get the content of the DialogBox
 **/
Container* Dialog::content() const {

    return _content;
}

/**
 * @brief BoolDialog, function to pop a dialog that ask for
 *  a validate or cancel, return a bool dependent on user
 * @param title, title of the dialog
 * @param msg, message displayed
 * @param ok, validate button text
 * @param cancel, cancel button text
 * @param ctx, graphical context of the gui
 **/
bool Dialog::BoolDialog(std::string title, std::string msg
                ,std::string ok, std::string cancel
                ,GraphiContext* ctx)
{
    // Get Correct Rectangles
    SDL_Rect r = gutils::dialogRectangle(ctx->_window);

    // Create the message Container
    Container* m = new Container(
                    gutils::getRelativeRectangle(r, 0,0,1.0f, 0.8f)
                    ,true, false, ctx);
    m->setBackgroundColor(guicst::DIALOG_BG_COLOR);
    GLabel* txt = new GLabel(msg
                    ,gutils::getRelativeRectangle(r, 0,0,1.0f, 0.8f)
                    ,true, false, 0, ctx);
    txt->setBackgroundColor(guicst::DIALOG_BG_COLOR);
    txt->reverseShadow();

    // Fill the container and create the Dialog
    m->addChild(txt);

    Dialog* dialog = new Dialog(title, ok, cancel, m, r, ctx
                            ,callbackDefaultOk
                            ,callbackDefaultCancel);

    // The Wait for the User Input
    SFXPEvent* event(nullptr);
    do {

        if (event) delete event;
        event = waitForDialog(dialog);
        
        VERIFY_EVENT(event);
        
    } while (event->_type != SFXPEvent::Type::Event_GuiInputBool);
    
    // Free the Dialog
    delete dialog;

    // Get the user validation
    bool res = (event)?event->_gui._boolDialog:false;
    if (event) delete event;
    
    // Return User input ( or false if an error occured )
    return res;
}

/**
 * @brief FileDialog, function to pop a dialog that ask for
 *  choose a file, return a file name or en empty string
 * @param title, title of the dialog
 * @param ok, validate button text
 * @param cancel, cancel button text
 * @param ctx, graphical context of the gui
 **/
std::string Dialog::FileDialog(std::string title
                ,std::string ok, std::string cancel
                ,GraphiContext* ctx)
{
    // Get Correct Rectangles
    SDL_Rect r = gutils::dialogRectangle(ctx->_window);

    // Create File names Container
    ScrollLayout* sl = new ScrollLayout(
                    
                        gutils::getRelativeRectangle(r, 0,0,1.0f, 0.8f)
                    ,true, false
                    ,ScrollLayout::V_LAYOUT, 3, ScrollLayout::ALIGN_C
                    ,ctx);
    sl->setBackgroundColor(guicst::DIALOG_BG_COLOR);
    sl->reverseShadow();
    
    // Create the Dialog
    Dialog* dialog = new Dialog(title, ok, cancel, sl, r, ctx
                            ,callbackDefaultOk
                            ,callbackDefaultCancel);

    // Wait for the user input
    SFXPEvent* event(nullptr);
    std::string path("./");
    std::string current("");
    bool changed(true);
    do {

        // Populate the layout according to files
        if (changed) {

            // List files in the directory
            std::vector<std::string> files = utils::listFiles(path);
            
            changed = false;
            sl->clear(true);
            
            for (auto& f : files) {

                GButton* b = new GButton(f,
                    gutils::getRelativeRectangle(r, 0,0,0.98f, 0.075f)
                    ,true, false, ctx, callbackSelectFile);

                sl->addChild(b, false);
            }
        }

        // Wait for an event
        if (event) delete event;
        event = waitForDialog(dialog);
        VERIFY_EVENT(event);

        if (event->_type == SFXPEvent::Type::Event_GuiInputString) {

            current = event->_gui._strDialog;
            if (utils::isDirectory(path + current)) {

                path += current + "/";
                changed = true;
            }
        }
        
    } while (event->_type != SFXPEvent::Type::Event_GuiInputBool);
    
    // Free the Dialog
    delete dialog;
    
    // Get the user validation
    bool res = (event)?event->_gui._boolDialog:false;
    if (event) delete event;
    
    // Return selected file
    if (res) return current;
    return "";
}

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
int Dialog::IntDialog(std::string title
                ,std::string ok, std::string cancel
                ,int min, int max, int err
                ,GraphiContext* ctx)
{
    // Get Correct Rectangles
    SDL_Rect r = gutils::dialogRectangle(ctx->_window);

    // Create the content pane
    Container* content = new Container(r, true, false, ctx);
    GIntIBox* box = new GIntIBox(r, true, true, min, max, ctx);
    content->addChild(box);
    
    // Create the Dialog
    Dialog* dialog = new Dialog(title, ok, cancel, content, r, ctx
                            ,callbackDefaultOk
                            ,callbackDefaultCancel);

    // Wait for the user input
    SFXPEvent* event(nullptr);
    int current;
    do {

        // Wait for an event
        event = waitForDialog(dialog);
        VERIFY_EVENT(event);
        
        if (event->_type == SFXPEvent::Type::Event_GuiInputInt) {

            current = event->_gui._intDialog;
        }
        
    } while (event->_type != SFXPEvent::Type::Event_GuiInputBool);
    
    // Free the Dialog
    delete dialog;
    
    // Get the user validation
    bool res = (event)?event->_gui._boolDialog:false;
    if (event) delete event;
    
    // Return selected file
    if (res) return current;
    return err;
}

/**
 * @brief waitForDialog, function that upate the screen and wait
 *  for the user to click on a clickable on the dialog
 * @param dialog, the dialog that is waiting for user
 * @return the event returned by the dialog
 **/
SFXPEvent* Dialog::waitForDialog(Dialog* dialog) {

    while (1) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.state == SDL_PRESSED) {
                
                GraphicObj* clicked = dialog->childAt(event.button.x, event.button.y);
                if (clicked) {

                    //printf("Dialog Click Intercepeted\n");

                    Clickable* obj = dynamic_cast<Clickable*>(clicked);
                    return obj->clicked();
                }
            }
        }
        }

        dialog->render(SDL_GetWindowSurface(dialog->context()->_window));
        SDL_UpdateWindowSurface(dialog->context()->_window);
    }
}

/**
 * @brief callbackDefaultOk, default callback for the ok button
 *  of dialogs
 **/
SFXPEvent* Dialog::callbackDefaultOk(Clickable* obj) {

    SFXPEvent* res = new SFXPEvent(SFXPEvent::Type::Event_GuiInputBool);
    res->_gui._boolDialog = true;
    return res;
}
/**
 * @brief callbackDefaultCancel, default callback for the cancel
 *  button of dialogs
 **/
SFXPEvent* Dialog::callbackDefaultCancel(Clickable* obj) {

    SFXPEvent* res = new SFXPEvent(SFXPEvent::Type::Event_GuiInputBool);
    res->_gui._boolDialog = false;
    return res;
}

/**
 * @brief callbackSelectFile, default callback for select file
 *  buttons in dialogs
 **/
SFXPEvent* Dialog::callbackSelectFile(Clickable* obj) {

    for (auto& b : obj->parent()->childs()) {

        b->setBackgroundColor(guicst::BUTTON_COLOR);
    }

    SFXPEvent* res = new SFXPEvent(SFXPEvent::Type::Event_GuiInputString);
    res->_gui._strDialog = dynamic_cast<GButton*>(obj)->label()->text();
    obj->setBackgroundColor(guicst::BUTTON_CCK_COLOR);

    //printf("FileButton Pressed : %s\n", res._gui._strDialog.c_str());
    return res;
}
