/**********************************************************************
 * @file InputBox.h
 * @author TomatoPi
 * @version 1.0
 *
 * Object to get user text inputs from keyboard
 **********************************************************************/
#ifndef DEF_GUI_INPUT_BOX_H
#define DEF_GUI_INPUT_BOX_H

#include "Button.h"

class GIntIBox : public IntButton {

    public :

        /**
         * @brief Clickable Constructor, initialise attributes
         * @param r, rectangle that represent boundaries of the object
         * @param visible, true if the object has to be drawn
         * @param isv, true if the object is volatile
         * @param ctx, Pointer to the graphic Context
         **/
        GIntIBox(SDL_Rect r, bool visible , bool isv
                ,int min, int max
                , GraphiContext* ctx);

        /**
         * @brief setMin, change minimal accepted value for this box
         **/
        void setMin(int m);
        /**
         * @brief min, get current minimal accepted value
         **/
        int min() const;
        
        /**
         * @brief setMax, change maximal accepted value for this box
         **/
        void setMax(int m);
        /**
         * @brief max, get current maximal accepted value
         **/
        int max() const;
        
        /**
         * @brief setValue, method to change the stored value of the button
         **/
        virtual void setValue(int v);

    protected :

        int _min, _max;
        
    protected :

        /**
         * @brief intInputCallback, callback called when an int input is
         *  clicked
         * @warning this callback will stop the main thread until the
         *  input is finished
         **/
        static SFXPEvent* intInputCallback(Clickable* obj);
};

#endif
