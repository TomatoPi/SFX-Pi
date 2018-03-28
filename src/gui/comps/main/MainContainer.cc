/**********************************************************************
 * @file MainContainer.cc
 * @author TomatoPi
 * @version 1.0
 *
 * MainContainer of the Gui
 **********************************************************************/
#include "MainContainer.h"
#include "gui/utils/utils.h"
#include "gui/colors.h"

#include "log.h"

#include "effect/EffectParamSet.h"

#define NAME "MainContainer"

namespace{

    const int leftBoxMargin = 5;
}

/**
 * @brief MainContainer Constructor
 **/
MainContainer::MainContainer(GraphiContext* ctx):
    Container(gutils::getWindowRectangle(ctx->_window), true, false, ctx)
    ,_editableSelectBox(nullptr)
{
    this->setBackgroundColor(guicst::MAIN_FRAME_COLOR);

    // MAKE THE TOOLBAR //
    MainMenuBar* toolBar = new MainMenuBar(
            gutils::alignRectangle(
                gutils::getRelativeRectangle(_box, 0, 0, 1.0f, 0.06f)
                ,_box, gutils::VA_TOP)
            ,ctx);

    // MAKE THE CENTRAL LAYOUT //
    FlowLayout* centralLayout = new FlowLayout(
            gutils::alignRectangle(
                gutils::getRelativeRectangle(_box, 0, 0, 1.0f, 0.94f)
                ,_box, gutils::VA_BOTTOM)
            , true, false
            ,FlowLayout::H_LAYOUT, 3, FlowLayout::ALIGN_C
            ,ctx);
    centralLayout->setBackgroundColor(guicst::MAIN_FRAME_COLOR);
    SDL_Rect centralBox = centralLayout->box();

    // MAKE THE LEFT BOX //
    FlowLayout* leftBoxLayout = new FlowLayout(
            gutils::alignRectangle(
                gutils::getRelativeRectangle(centralBox, 0, 0, 0.25f, 1.0f)
                ,centralBox, gutils::HA_LEFT | gutils::VA_CENTER)
            ,true, false
            ,FlowLayout::V_LAYOUT, leftBoxMargin, FlowLayout::ALIGN_C
            ,ctx);
    leftBoxLayout->setBackgroundColor(guicst::TOOLBAR_COLOR);

    // POPULATE THE CONTAINER //
    populateLeftBox(leftBoxLayout);
    centralLayout->addChild(leftBoxLayout);

    this->addChild(toolBar);
    this->addChild(centralLayout);
}
MainContainer::~MainContainer()
{
}

void MainContainer::clearEditableList() {

    _editableSelectBox->clear(true);
}
void MainContainer::addEditable(GuiEvent event) {

    SDL_Rect box = _editableSelectBox->box();

    if (event._editableType == GuiEvent::PARAMSET) {

        ParamSet* edit = (ParamSet*)event._editable;

        IntButton* b = new IntButton(edit->name(),
            gutils::getRelativeRectangle(box, 0, 0, box.w - 6, 0.1f)
            ,true, false, edit->id(), _context
            ,callbackChgEditable);
        _editableSelectBox->addChild(b);
    }
    else if (event._editableType == GuiEvent::CONNECTION) {

        SFXPlog::todo(NAME) << "Add Connection Editable" << '\n';
    }
    else if (event._editableType == GuiEvent::SEQUENCER) {

        SFXPlog::todo(NAME) << "Add Sequencer Editable" << '\n';
    }
    else if (event._editableType == GuiEvent::ALINK) {

        SFXPlog::todo(NAME) << "Add AnalogLink Editable" << '\n';
    }
}

/**
 * @brief populateLeftBox, function to populate the leftBox of the
 *  MainContainer
 **/
void MainContainer::populateLeftBox(Container* leftBoxLayout) {

    SDL_Rect leftBoxBox = leftBoxLayout->box();
    GraphiContext* ctx = leftBoxLayout->context();

    // CREATE THE TYPE SELECTION CONTAINER //
    FlowLayout* editTypeLayout = new FlowLayout(
            gutils::getRelativeRectangle(leftBoxBox, 0, 0
                            , leftBoxBox.w - 2*leftBoxMargin
                            , 0.25f)
            ,true, false
            ,FlowLayout::V_LAYOUT, 3, FlowLayout::ALIGN_C
            ,ctx);
    SDL_Rect editTypeBox = editTypeLayout->box();

    // ADD TYPE SELECTION BUTTONS //
    IntButton* selectParamSet = new IntButton("Effect Unit"
            ,gutils::getRelativeRectangle(editTypeBox, 0, 0
                                        , editTypeBox.w - 6, 0.18f)
            ,true, false ,GuiEvent::PARAMSET, ctx
            , callbackChgEditType);
            
    IntButton* selectConnection = new IntButton("Connection"
            ,gutils::getRelativeRectangle(editTypeBox, 0, 0
                                        , editTypeBox.w - 6, 0.18f)
            ,true, false ,GuiEvent::CONNECTION, ctx
            , callbackChgEditType);
            
    IntButton* selectSequencer = new IntButton("Event Sequencer"
            ,gutils::getRelativeRectangle(editTypeBox, 0, 0
                                        , editTypeBox.w - 6, 0.18f)
            ,true, false ,GuiEvent::SEQUENCER, ctx
            , callbackChgEditType);
            
    IntButton* selectALink = new IntButton("Analog Link"
            ,gutils::getRelativeRectangle(editTypeBox, 0, 0
                                        , editTypeBox.w - 6, 0.18f)
            ,true, false ,GuiEvent::ALINK, ctx
            , callbackChgEditType);

    // POPULATE EDITLAYOUT //
    editTypeLayout->addChild(selectParamSet, true);
    editTypeLayout->addChild(selectConnection, true);
    editTypeLayout->addChild(selectSequencer, true);
    editTypeLayout->addChild(selectALink, true);

    _editableSelectBox = new FlowLayout(
            gutils::getRelativeRectangle(leftBoxBox, 0, 0
                            , leftBoxBox.w - 2*leftBoxMargin
                            , leftBoxBox.h - (editTypeBox.h + 3*leftBoxMargin))
            ,true, false
            ,FlowLayout::V_LAYOUT, 3, FlowLayout::ALIGN_TL
            ,ctx);

    leftBoxLayout->addChild(editTypeLayout);
    leftBoxLayout->addChild(_editableSelectBox);
}

/**
 * @brief callbackChgEditParamSet, callback for the button
 *  to edit paramsets of the preset
 **/
SFXPEvent* MainContainer::callbackChgEditType(Clickable* obj)
{
    SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_GuiEditTypeChanged);
    event->_gui._editableType = static_cast<IntButton*>(obj)->value();
    return event;
}
/**
 * @brief callbackChgEditable, callback called to change the
 *  currently edited object
 **/
SFXPEvent* MainContainer::callbackChgEditable(Clickable* obj) {

    return nullptr;
}
