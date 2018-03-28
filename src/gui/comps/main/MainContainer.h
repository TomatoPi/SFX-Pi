/**********************************************************************
 * @file MainContainer.h
 * @author TomatoPi
 * @version 1.0
 *
 * MainContainer of the Gui
 **********************************************************************/
#ifndef DEF_MAIN_CONTAINER_H
#define DEF_MAIN_CONTAINER_H

#include "gui/base/Base.h"
#include "gui/comps/menu/Menu.h"

class MainContainer : public Container {

    public :

        /**
         * @brief MainContainer Constructor
         **/
        MainContainer(GraphiContext* ctx);
        virtual ~MainContainer();

        void clearEditableList();
        void addEditable(GuiEvent event);

    protected :

        /**
         * @brief populateLeftBox, function to populate the leftBox of the
         *  MainContainer
         **/
        void populateLeftBox(Container* leftbox);

        Container* _editableSelectBox;

    public :
        /**
         * @brief callbackChgEditType, callback for the button
         *  to edit paramsets of the preset
         **/
        static SFXPEvent* callbackChgEditType(Clickable* obj);
        /**
         * @brief callbackChgEditable, callback called to change the
         *  currently edited object
         **/
        static SFXPEvent* callbackChgEditable(Clickable* obj);
};

#endif
