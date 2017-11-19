/**********************************************************************
 * @file FootSwitch.h
 * @author TomatoPi
 * @version 1.0
 *
 * FootSwitch file
 * A footswitch can have three modes : push, pull or push-pull
 * defining when button's callback is activated
 **********************************************************************/
#ifndef DEF_FOOT_SWITCH_H
#define DEF_FOOT_SWITCH_H

#include <vector>
#include <string>

#include "../../core/Typedefs.h"
#include "../../Const.h"

class FootSwitch{

    public :

        enum Mode{
            Push    =0,
            Pull    =1,
            PushPull=2
        };

    public :

        /**
         * Create footswitch at given adress
         * Created footswitch with this function will not have visual
         * indicator
         **/
        FootSwitch(id2_t id, std::string name, Mode mode=Push);

        /**
         * Create a footswitch with a visual indicator
         **/
        FootSwitch(id2_t id, std::string name, id2_t lid, Mode mode=Push);

        /**
         * Add or Remove a target to this footswitch
         **/
        int addTarget(id2_t id);
        int removeTarget(id2_t id);
        std::vector<id2_t> getTargets() const;

        /**
         * Get visual indicator ID associated with this footswitch
         **/
        bool hasVisual() const;
        id2_t getVisualID() const;

        /**
         * Change and get current footswitch operating mode
         **/
        void setMode(Mode mode);
        Mode getMode() const;

        std::string getName() const;
        id2_t getID() const;

        void print() const;

    protected :

        id2_t       m_id;
        std::string m_name;
        Mode        m_mode;

        id2_t   m_iid;
        bool    m_hasi;

        std::vector<id2_t> m_targets;
};

#endif
