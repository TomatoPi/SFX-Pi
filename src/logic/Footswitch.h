/**********************************************************************
 * @file FootSwitch.h
 * @author TomatoPi
 * @version 1.2
 *
 * FootSwitch file
 * A footswitch can have three modes : push, pull or push-pull
 * defining when button's callback is activated
 **********************************************************************/
#ifndef DEF_FOOT_SWITCH_H
#define DEF_FOOT_SWITCH_H

#include <vector>
#include <string>

#include "types.h"
#include "log.h"

class Footswitch{

    public :

        /**
         * Create footswitch at given adress
         * Created footswitch with this function will not have visual
         * indicator
         **/
        Footswitch(SFXP::id2_t id, std::string name);

        /**
         * Create a footswitch with a visual indicator
         **/
        Footswitch(SFXP::id2_t id, std::string name, SFXP::id2_t lid);

        /**
         * Get visual indicator ID associated with this footswitch
         **/
        bool hasVisual() const;
        SFXP::id2_t getVisualID() const;

        std::string getName() const;
        SFXP::id2_t getID() const;

        void print() const;

    protected :

        SFXP::id2_t _id;
        std::string _name;

        SFXP::id2_t _iid;
        bool        _hasi;
};

#endif
