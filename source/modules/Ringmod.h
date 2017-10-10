#ifndef DEF_RINGMOD_H
#define DEF_RINGMOD_H

#include <jack/jack.h>
#include "../core/Modules.h"
#include "../core/Utility.h"

#include "../core/ModuleFactory.h"
#include "../consts.h"

#define RING_DEPTH MOD_COUNT + 0

static const int    RINGMOD_PARAMS_COUNT = 1;
static const std::string RINGMOD_PARAM_NAMES[MOD_COUNT+RINGMOD_PARAMS_COUNT] = {"Volume", "Depth"};
static const float  RINGMOD_DEFAULT_PARAMS[MOD_COUNT+RINGMOD_PARAMS_COUNT] = {0, 1, 1.0f};

class Ringmod : public Module{

    public:

        Ringmod( int id );
        //virtual int bypass(jack_nframes_t nframes, void *arg);

    protected :

        virtual int do_process(jack_nframes_t nframes);

        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */

        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
};

Module* build_ring( int id );

#endif
