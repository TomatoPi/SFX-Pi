/**********************************************************************
 * @file EffectHandler.h
 * @author TomatoPi
 * @version 1.1
 *
 * File providing main program's handler that will store all effect units
 **********************************************************************/
#ifndef DEF_EFFECT_HANDLER_H
#define DEF_EFFECT_HANDLER_H

#include <map>
#include <jack/jack.h>

#include "log.h"

#include "effect/AbstractEffectUnit.h"
#include "effect/EffectFactory.h"

#include "preset/Preset.h"

#include "core/AbstractHandler.h"
#include "core/Event.h"

class EffectHandler : public AbstractHandler {

    public :

        typedef std::map<SFXP::id1_t, AbstractEffectUnit*> EffectGraph;

        EffectHandler();
        virtual ~EffectHandler();

        /**
         * Function used to push an event to an handler
         * The event is imediatly processed
         **/
        virtual void pushEvent(SFXPEvent& event);

        /**
         * Function called at each main loop frame
         * Makes the Handler doing his job
         **/
        virtual void run();

        void clearGraph();

        void printGraph();

        void connect(Connection c);
        void disconnect(Connection c);

    private :

        void eventPresetChanged(SFXPEvent& event);
        
        void eventAddBank(SFXPEvent& event);
        void eventRemoveBank(SFXPEvent& event);
        void eventChangeBank(SFXPEvent& event);
        void eventEditParam(SFXPEvent& event);

        void eventConnect(SFXPEvent& event);
        void eventDisconnect(SFXPEvent& event);

        void eventAddEffect(SFXPEvent& event);
        void eventRemoveEffect(SFXPEvent& event);

        void eventListAvailable();
        void eventShowEffect(SFXPEvent& event);
        void eventShowEffectPool(SFXPEvent& event);
        
        /**
         * Function to get name of a given port
         *  return empty string if port not found
         **/
        std::string buildPortName(SFXP::PortType pt, SFXP::id1_t id, SFXP::usize_t idx);
        

        /** List of all effects instances **/
        EffectGraph _graph;

        /** Client representing All programm for JACK **/
        jack_client_t* _client;

        /** Current Preset **/
        Preset* _preset;
};

#endif
