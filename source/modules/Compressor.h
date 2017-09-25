#ifndef DEF_COMPRESSOR_H
#define DEF_COMPRESSOR_H

#include "../core/Buffer.h"
#include "../core/Utility.h"
#include "../core/Modules.h"

#define COMP_RATIO  MOD_COUNT + 0
#define COMP_ATT    MOD_COUNT + 1
#define COMP_REL    MOD_COUNT + 2
#define COMP_THR    MOD_COUNT + 3
#define COMP_GATE  MOD_COUNT + 4
#define COMP_CUT    MOD_COUNT + 5
#define COMP_DRY    MOD_COUNT + 6

#define COMP_WINDOW 30

static const int    COMP_PARAMS_COUNT = 7;
static const string COMP_PARAM_NAMES[MOD_COUNT+COMP_PARAMS_COUNT] = {"Volume", "Ratio", "Attack", "Release", "Thrsld",
                                                                    "Gate", "GateCut", "DryWet"};
static const float  COMP_DEFAULT_PARAM[MOD_COUNT+COMP_PARAMS_COUNT] = {0, 1, 2.25f, 200, 400, 0.1,
                                                                    1, 0.0031f, 1};

class Compressor : public Module{
    
    public :
    
        Compressor( const char *server );
        
    protected :
		
        virtual inline int do_process(jack_nframes_t nframes);
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);  /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
        
        Buffer_R buffer_;   /**< Running summ buffer */
        
        float ramp_;    /**< Store duration since last threshold reach */
        float ik_;      /**< Inverse of compression ration, used for accellerate calculations */
        float isr_;     /**< Inverse of samplerate, used for accellerate calculations */
        float thr2_;    /**< Threshold squared */
        
        bool last_;     /**< Used for check if threshold has been reached */
    
};

#endif