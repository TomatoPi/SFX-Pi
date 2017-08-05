#ifndef DEF_TONESTACK_H
#define DEF_TONESTACK_H

#include "../core/Filter.h"
#include "../core/Modules.h"

#define TONE_FLOW 	0
#define TONE_FHIGH 	1
#define TONE_GL		2
#define TONE_GM		3
#define TONE_GH		4

static const int    TONE_PARAMS_COUNT    = 5;
static const string TONE_PARAM_NAMES[TONE_PARAMS_COUNT]   = {"Lowcut", "Highcut", "Lowgain", "Midgain", "Hghgain"};
static const float  TONE_DEFAULT_PARAM[TONE_PARAMS_COUNT] = {200, 1200, 0.75f, 3.0f, 7.0f};

class Tonestack : public Module{

	public:
	
		Tonestack(const char *server);
		
        virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);  /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
    
        Filter_3EQ filter_;
};

#endif
