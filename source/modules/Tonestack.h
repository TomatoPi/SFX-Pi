#ifndef DEF_TONESTACK_H
#define DEF_TONESTACK_H

#include "../core/Filter.h"
#include "../core/Modules.h"

#define TONE_FLOW 	MOD_COUNT + 0
#define TONE_FHIGH 	MOD_COUNT + 1
#define TONE_GL		MOD_COUNT + 2
#define TONE_GM		MOD_COUNT + 3
#define TONE_GH		MOD_COUNT + 4

static const int    TONE_PARAMS_COUNT    = 5;
static const string TONE_PARAM_NAMES[MOD_COUNT+TONE_PARAMS_COUNT]   = {"Volume", "Lowcut", "Highcut", "Lowgain", "Midgain", "Hghgain"};
static const float  TONE_DEFAULT_PARAM[MOD_COUNT+TONE_PARAMS_COUNT] = {0, 1, 200, 1200, 0.75f, 3.0f, 7.0f};

class Tonestack : public Module{

	public:
	
		Tonestack(const char *server);
		//virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
		
        virtual int do_process(jack_nframes_t nframes);
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);  /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
    
        Filter_3EQ filter_;
};

#endif
