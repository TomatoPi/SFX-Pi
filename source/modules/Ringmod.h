#ifndef DEF_RINGMOD_H
#define DEF_RINGMOD_H

#include <jack/jack.h>
#include "../core/Modules.h"
#include "../core/Utility.h"

#define RINGMOD_PARAMS 1

#define RING_DEPTH 0

static const int    RINGMOD_PARAMS_COUNT = 1;
static const string RINGMOD_PARAM_NAMES[RINGMOD_PARAMS_COUNT] = {"Depth"};
static const float  RINGMOD_DEFAULT_PARAMS[RINGMOD_PARAMS_COUNT] = {1.0f};


class Ringmod : public Module{

	public:
	
		Ringmod(const char *server);
		
        virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
};

#endif
