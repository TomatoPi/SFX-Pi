#ifndef DEF_LFO_H
#define DEF_LFO_H

#include <jack/jack.h>
#include <iostream>

using namespace std;

#include "../core/Utility.h"
#include "../core/Modules.h"

/*
*	LFO's params count and param's index
*/
#define LFO_TYPE 	0
#define LFO_FREQ 	1

#define LFO_PHASE 	2
#define LFO_SIGN 	3

#define LFO_VAR1 	4
#define LFO_VAR2 	5

static const int    LFO_PARAMS_COUNT = 6;
static const string LFO_PARAM_NAMES[LFO_PARAMS_COUNT] 	 = {"Waveform", "Frequency", "Phase", "Sign", "Param-1", "Param-2"};
static const float  LFO_DEFAULT_PARAMS[LFO_PARAMS_COUNT] = {0, 1.0f, 0.0f, 1, 0.0f, 0.0f};

static int RandSeed = 48172;

/**
*	List of possible LFO waveshape.
*	Sinus
*	Square
*	Triangular
*	Sawtooth
*	Varislope
*	N-Phase
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

/*
*	LFO Module
*/
class LFO : public Module{
	
	public:
		
		LFO(const char *server);

        virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
        
        void sync();
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        
        void update_type(LFO_wave type);
        
        sample_t (*waveform_)(float, float, float, float);  /**< waveform generator function */
        
        int samplerate_;
		float ramp_;
};

/*
*	Waveform functions, see wiki
*/
sample_t w_sin(float in, float sign, float p1, float p2);	//Sinusoid : p1, p2 unused
sample_t w_sqr(float in, float sign, float p1, float p2);	//Square : p1, p2 unused
sample_t w_tri(float in, float sign, float p1, float p2);	//Triangular : p1, p2 unused
sample_t w_saw(float in, float sign, float p1, float p2);	//Sawtooth : sign > 0, ascendent ; sign < 0 descendent
sample_t w_var(float in, float sign, float p1, float p2);	//Varislope : p1 first extremum position ( 0 - 100% ) ; p2 transition form ( 0 : square, 1 : triangle )
sample_t w_nph(float in, float sign, float p1, float p2);	//N-Phase : p1 phase quantity (int) ; p2 cutting quantity (int)
sample_t w_whi(float in, float sign, float p1, float p2);	//White noise

#endif
