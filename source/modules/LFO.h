#ifndef DEF_LFO_H
#define DEF_LFO_H

#include <jack/jack.h>
#include <iostream>

using namespace std;

#include "../core/Utility.h"
#include "../core/Modules.h"

/*
*   LFO's params count and param's index
*/
#define LFO_TYPE    MOD_COUNT + 0
#define LFO_FREQ    MOD_COUNT + 1

#define LFO_PHASE   MOD_COUNT + 2
#define LFO_SIGN    MOD_COUNT + 3

#define LFO_VAR1    MOD_COUNT + 4
#define LFO_VAR2    MOD_COUNT + 5

#define LFO_PHD     MOD_COUNT + 6
#define LFO_PHO     MOD_COUNT + 7

static const int    LFO_PARAMS_COUNT = 8;
static const string LFO_PARAM_NAMES[MOD_COUNT+LFO_PARAMS_COUNT] = {"Volume", "Waveform", "Freq", "Phase", "Sign", "Param1", "Param2", "Ph-Dist", "Ph-Fix"};
static const float  LFO_DEFAULT_PARAMS[MOD_COUNT+LFO_PARAMS_COUNT] = {0, 1, 2, 2.0f, 0.0f, 1, 0.0f, 0.0f, 0.5f, 0.0f};

/**
*   List of possible LFO waveshape.
*   Sinus
*   Square
*   Triangular
*   Sawtooth
*   Varislope
*   N-Phase
*/
typedef enum {

    WAVE_SIN=0,
    WAVE_SQR=1,
    WAVE_TRI=2,
    WAVE_SAW=3,
    WAVE_VAR=4,
    WAVE_NPH=5,
    WAVE_WHI=6
}LFO_wave;
static const int  WAVE_COUNT = 7;

/*
*   LFO Module
*/
class LFO : public Module{

    public:

        LFO(const char *server);
        //virtual int bypass(jack_nframes_t nframes, void *arg);

        void sync();

    protected :

        virtual int do_process(jack_nframes_t nframes);

        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */

        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */

        void update_type(LFO_wave type);

        sample_t (*waveform_)(float, float, float, float);  /**< waveform generator function */

        int samplerate_;
        float ramp_;
};

/*
*   Waveform functions, see wiki
*/
sample_t w_sin(float in, float sign, float p1, float p2);   //Sinusoid : p1, p2 unused
sample_t w_sqr(float in, float sign, float p1, float p2);   //Square : p1, p2 unused
sample_t w_tri(float in, float sign, float p1, float p2);   //Triangular : p1, p2 unused
sample_t w_saw(float in, float sign, float p1, float p2);   //Sawtooth : sign > 0, ascendent ; sign < 0 descendent
sample_t w_var(float in, float sign, float p1, float p2);   //Varislope : p1 first extremum position ( 0 - 100% ) ; p2 transition form ( 0 : square, 1 : triangle )
sample_t w_nph(float in, float sign, float p1, float p2);   //N-Phase : p1 phase quantity (int) ; p2 cutting quantity (int)
sample_t w_whi(float in, float sign, float p1, float p2);   //White noise

#endif
