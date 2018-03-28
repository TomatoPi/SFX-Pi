/**********************************************************************
 * @file Menu.h
 * @author TomatoPi
 * @version 1.0
 *
 * Main MenuBar Object
 **********************************************************************/
#ifndef DEF_GUI_MENU_H
#define DEF_GUI_MENU_H

#include "gui/base/Base.h"
#include "gui/comps/menu/MenuItem.h"
#include "gui/base/Context.h"

class MainMenuBar : public FlowLayout {

    public :

        /**
         * @brief MainMenuBar Constructor, same constructor as a FlowLayout
         **/
        MainMenuBar(SDL_Rect r, GraphiContext* ctx);

    private :

        /**
         * @brief populateFichierMenu, function called to add items in
         *  the fichier menu
         **/
        void populateFichierMenu(FlowLayout* menu);
        /**
         * @brief populateEffetsMenu, function called to add items in
         *  the effet menu
         **/
        void populateEffetsMenu(FlowLayout* menu);

    /* ************************************************************** */
    /* ************************** Callbacks ************************* */
    /* ************************************************************** */
    private :

        /* ********************************************************** */
        /* ************************ Fichiers  *********************** */
        /* ********************************************************** */

        /**
         * @brief callbackQuitButton, Callback called when the quit
         *  button is clicked
         * @return an Event_GuiQuit
         **/
        static SFXPEvent* callbackQuitButton(Clickable* b);

        /**
         * @brief callbackNouveauButton, Callback called when the Nouveau
         *  button is clicked
         * @return an Event_NewPreset
         **/
        static SFXPEvent* callbackNouveauButton(Clickable* b);

        /**
         * @brief callbackChargerBouton, Callback called when the Charger
         *  button is clicked
         * @return an Event_LoadPreset
         **/
        static SFXPEvent* callbackChargerBouton(Clickable* b);

        /**
         * @brief callbackEnregistrerBouton, Callback called when the
         *  Enregistrer button is clicked
         * @return an Event_SavePreset
         **/
        static SFXPEvent* callbackEnregistrerBouton(Clickable* b);

        /* ********************************************************** */
        /* ************************** Effets ************************ */
        /* ********************************************************** */

        /**
         * @brief callbackNewEffectBouton, Callback called when an
         *  AddEffect is clicked
         * @return an Event_AddEffect
         **/
        static SFXPEvent* callbackNewEffectBouton(Clickable* b);
};

#endif
