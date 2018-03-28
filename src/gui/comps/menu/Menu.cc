/**********************************************************************
 * @file Menu.h
 * @author TomatoPi
 * @version 1.0
 *
 * Main MenuBar Object
 **********************************************************************/
#include "Menu.h"
#include "gui/utils/utils.h"
#include "gui/colors.h"

#include "effect/EffectFactory.h"

namespace {

    const float menuItemH = 0.9f;
    const float menuItemW = 0.1f;

    const float subMenuItemH = 0.75f;
    const float subMenuItemW = 0.15f;
}

/**
 * @brief MainMenuBar Constructor, same constructor as a FlowLayout
 **/
MainMenuBar::MainMenuBar(SDL_Rect r, GraphiContext* ctx):
    FlowLayout(r, true, false, H_LAYOUT, 5, ALIGN_C, ctx)
{

    this->setBackgroundColor(guicst::TOOLBAR_COLOR);

    // FICHIER MENU //
    
    MenuItem* fichierItem = new MenuItem("Fichier"
        ,gutils::getRelativeRectangle(r, 0, 0, menuItemW, menuItemH)
        ,true, ctx, nullptr, nullptr);

    FlowLayout* fichierMenu = new FlowLayout(
        gutils::getRelativeRectangle(r, 0, r.h, 0.1f, 0)
        ,false, true, V_LAYOUT, 5, ALIGN_C, ctx);
    fichierMenu->setBackgroundColor(guicst::TOOLBAR_COLOR);
    fichierMenu->setBorderColor(guicst::TOOLBAR_COLOR);

    // POPULATE FICHIER MENU //
    this->populateFichierMenu(fichierMenu);

    fichierItem->setSubMenu(fichierMenu);
    this->addChild(fichierItem);

    // EFFETS MENU //
    
    MenuItem*  effectsItem = new MenuItem("Effets"
        ,gutils::getRelativeRectangle(r, 0, 0, menuItemW, menuItemH)
        ,true, ctx, nullptr, nullptr);

    FlowLayout* effectsMenu = new FlowLayout(
        gutils::getRelativeRectangle(r, 0, r.h, 0.1f, 0)
        ,false, true, V_LAYOUT, 5, ALIGN_C, ctx);
    effectsMenu->setBackgroundColor(guicst::TOOLBAR_COLOR);
    effectsMenu->setBorderColor(guicst::TOOLBAR_COLOR);

    // POPULATE EFFET MENU //
    this->populateEffetsMenu(effectsMenu);

    effectsItem->setSubMenu(effectsMenu);
    this->addChild(effectsItem);
}

/**
 * @brief populateFichierMenu, function called to add items in
 *  the fichier menu
 **/
void MainMenuBar::populateFichierMenu(FlowLayout* menu) {

    MenuItem* nouveau = new MenuItem("Nouveau"
                ,gutils::getRelativeRectangle(_box, 0, 0, subMenuItemW, subMenuItemH)
                ,true, _context, nullptr, callbackNouveauButton);
                
    MenuItem* charger = new MenuItem("Charger"
                ,gutils::getRelativeRectangle(_box, 0, 0, subMenuItemW, subMenuItemH)
                ,true, _context, nullptr, callbackChargerBouton);
                
    MenuItem* enregistrer = new MenuItem("Enregistrer"
                ,gutils::getRelativeRectangle(_box, 0, 0, subMenuItemW, subMenuItemH)
                ,true, _context, nullptr, callbackEnregistrerBouton);
                
    MenuItem* quitter = new MenuItem("Quitter"
                ,gutils::getRelativeRectangle(_box, 0, 0, subMenuItemW, subMenuItemH)
                ,true, _context, nullptr, callbackQuitButton);

    menu->addChild(nouveau, true);
    menu->addChild(charger, true);
    menu->addChild(enregistrer, true);
    menu->addChild(quitter, true);
}
/**
 * @brief populateEffetsMenu, function called to add items in
 *  the effet menu
 **/
void MainMenuBar::populateEffetsMenu(FlowLayout* menu) {

    // ADD AVAILABLE EFFECTS TO THE MENU //
    for (auto& tc : TCTLoader::typeCodeTable()) {

        Plugin* p = EffectFactory::getPlugin(tc.first);
        if (p) {

            IntButton* effect = new IntButton(p->name()
                ,gutils::getRelativeRectangle(_box, 0, 0, subMenuItemW, subMenuItemH)
                ,true, false, tc.first
                ,_context, callbackNewEffectBouton);
                
            menu->addChild(effect, true);
        }
    }
}

/* ****************************************************************** */
/* **************************** Callbacks *************************** */
/* ****************************************************************** */

    /* ********************************************************** */
    /* ************************ Fichiers  *********************** */
    /* ********************************************************** */
/**
 * @brief callbackQuitButton, Callback called when the quit
 *  button is clicked
 * @return an Event_GuiQuit
 **/
SFXPEvent* MainMenuBar::callbackQuitButton(Clickable* b) {

    if (Dialog::BoolDialog("Quitter ?"
            ,"Sauvergarder avant de Quitter ?"
            ,"Sauvegarder", "Quitter", b->context()))
    {

        SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_SavePreset);
        event->_preset._file = Dialog::FileDialog("Sauvergarder"
                                ,"Sauvegarder", "Annuler", b->context());
                                
        return event;
    }
    else
    {
        return new SFXPEvent(SFXPEvent::Type::Event_GuiQuit);
    }
}

/**
 * @brief callbackNouveauButton, Callback called when the Nouveau
 *  button is clicked
 * @return an Event_NewPreset
 **/
SFXPEvent* MainMenuBar::callbackNouveauButton(Clickable* b) {

    return new SFXPEvent(SFXPEvent::Type::Event_NewPreset);
}

/**
 * @brief callbackChargerBouton, Callback called when the Charger
 *  button is clicked
 * @return an Event_LoadPreset
 **/
SFXPEvent* MainMenuBar::callbackChargerBouton(Clickable* b) {

    SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_LoadPreset);
    event->_preset._file = Dialog::FileDialog("Charger"
                            ,"Charger", "Annuler", b->context());
                            
    return event;
}

/**
 * @brief callbackEnregistrerBouton, Callback called when the
 *  Enregistrer button is clicked
 * @return an Event_SavePreset
 **/
SFXPEvent* MainMenuBar::callbackEnregistrerBouton(Clickable* b) {

    SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_SavePreset);
    event->_preset._file = Dialog::FileDialog("Sauvergarder"
                            ,"Sauvegarder", "Annuler", b->context());
                            
    return event;
}

    /* ********************************************************** */
    /* ************************** Effets ************************ */
    /* ********************************************************** */

/**
 * @brief callbackNewEffectBouton, Callback called when an
 *  AddEffect is clicked
 * @return an Event_AddEffect
 **/
SFXPEvent* MainMenuBar::callbackNewEffectBouton(Clickable* b) {

    int id = Dialog::IntDialog("Choose Effect Id", "Valider", "Annuler"
        , 0, 1024, -1, b->context());
    
    // If user canceled
    if (id == -1) {

        return nullptr;
    }
    // Else send the AddEffect Event
    SFXPEvent* event = new SFXPEvent(SFXPEvent::Type::Event_AddEffect);
    event->_edit._id = (SFXP::id1_t)id;
    event->_edit._type = (SFXP::tc_t)(dynamic_cast<IntButton*>(b)->value());

    // TODO chainer un event identique pour le GuiHandler

    return event;
}
