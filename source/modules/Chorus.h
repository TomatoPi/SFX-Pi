#ifndef DEF_CHORUS_H
#define DEF_CHORUS_H

#include "../core/Modules.h"
#include "../core/Buffer.h"
#include "../core/Utility.h"

#include "../core/ModuleFactory.h"
#include "../consts.h"

/*
*   Chorus param's count and adresses
*/

#define CHORUS_DEPTH  MOD_COUNT + 0
#define CHORUS_DRYWET MOD_COUNT + 1
#define CHORUS_SIZE   MOD_COUNT + 2
#define CHORUS_WINDOW MOD_COUNT + 3
#define CHORUS_DELAY  MOD_COUNT + 4
#define CHORUS_WEIGTH CHORUS_DELAY + MAX_CHORUS_SIZE

#define MAX_CHORUS_SIZE 5
#define CHORUS_BUFFER   200

/*
*   Default values for chorus
*/
static const int    CHORUS_PARAMS_COUNT = 4 + 2*MAX_CHORUS_SIZE;

static const float  CHORUS_DEFAULT_PARAMS[MOD_COUNT+CHORUS_PARAMS_COUNT] = {0, 1, 1, 1, 1, 10,
                                                            10, 13, 18, 23, 28, 1, 1, 1, 1, 1};

static const std::string CHORUS_PARAM_NAMES[CHORUS_PARAMS_COUNT] = {"Depth", "Drywet", "Size",
                                                            "D1", "D2", "D3", "D4", "D5", "W1", "W2", "W3", "W4", "W5"};

static const int    CHORUS_DELAYS_LENGTH[2*MAX_CHORUS_SIZE] = {10, 10, 13, 13, 18, 18, 23, 23, 28, 28};

class Chorus : public Module{

    public :

        Chorus( int id );
        ~Chorus();

    protected :

        virtual inline int do_process(jack_nframes_t nframes);

        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);  /**< @see set_param(float *values) */

        //virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        //virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */

        int samplerate_;    /**< Current semplerate */
        Buffer_A *buffer_;  /**< Chorus buffer */

        float ramp1_;
        float ramp2_;
        float dr_;
};

Module* build_chorus( int id );

#endif
