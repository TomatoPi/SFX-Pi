#ifndef DEF_ENDMODULE_H
#define DEF_ENDMODULE_H

#include "../core/Modules.h"
#include "../core/Utility.h"

/*
*	Default values for end module
*/

#define END_RIGHT   MOD_VOLUME
#define END_LEFT    MOD_COUNT

static const int    END_PARAM_COUNT = 1;
static const float  END_DEFAULT_PARAMS[MOD_COUNT + END_PARAM_COUNT] = {0, 1, 1};
static const string END_PARAM_NAMES[MOD_COUNT + END_PARAM_COUNT] = {"VRight", "VLeft"};

/**
* EndModule class.
* End module is the last module of process graph
* I perform volume changement and check if output is saturated ( >1 or <-1 )
*/
class EndModule : public Module{
  
    public :
    
        /**
        *   End modules constructor
        *   @param server JACK server
        *   @param t module type, MOD_FIRST for capture module or MOD_LAST for playback module
        */
        EndModule( MODULE_TYPE t, int id );
        
        /**
        *   Function called by main loop for verify if output is staturated
        *   @return true if output has saturated since last reset
        *   @see reset_staturated()
        */
        bool is_saturated();
        /**
        *   Function used to reset staturation monitoring
        */
        void reset_saturated();
        
    protected :
        
        virtual int do_process( jack_nframes_t nframes );
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */

        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
        
        bool is_sat_;   /**< @see is_saturated() */
        float max;
    
};
#endif
