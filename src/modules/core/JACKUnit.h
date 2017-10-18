/**********************************************************************
 * FIle containing Stuff about JACK Units
 * 
 * JACKUnits is the processing side of an effect unit
 * Containing all JACK related stuff
 * 
 **********************************************************************/
#ifndef DEF_JACK_UNIT_H
#define DEF_JACK_UNIT_H

#include <jack/jack.h>

#include "../../../Const.h"

/**
 * JACKUnit class.
 * Used For separattion between the processing side and the param side
 * of an Effect Unit
 **/
class JACKUnit{
        
    private :
    
        static const PCST::hex_t STATUS_NO_CLIENT   = 0x01;
        static const PCST::hex_t STATUS_NO_PORT     = 0x02;
        
        static const PCST::hex_t STATUS_OK          = 0x04;
    
    public :
    
        JACKUnit():portC_(0),status_(STATUS_NO_CLIENT|STATUS_NO_PORT){};
        ~JACKUnit();
        
        /**
         * Function Used at Unit Construction for register it Client
         * This Function Must Be called before port Registration
         * @return 1 if registration has failed (Failed to connect JACK server)
         */
        bool registerClient(std::string p_name);
        
        bool activateClient();
        bool killClient();
        /**
         * Function Used at Unit Construction for register it Ports
         * It take 4 numbers corresponding to number of ports of givens types
         * And a string array containing port's names in order
         * @return 1 if port registration has failed
         */
        bool registerPort(const std::string* names, size_t ai=1, size_t ao=1, size_t mi=0, size_t mo=0);
        
        /**
         * Process Callback used by JACK
         * Function Specific for each effect, basically the effect's core
         */
        virtual int process(jack_nframes_t nframes, void* arg){ return 0; };
        
    protected :
        
        jack_client_t   *client_;
        jack_port_t     **port_;
        size_t          portC_;
        
        std::string     *name_;
        
        PCST::hex_t     status_;
}

#endif
