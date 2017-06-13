#ifndef DEF_LFO_H
#define DEF_LFO_H

#include <jack/jack.h>

#include "../core/Utility.h"
#include "../core/Modules.h"

/*
*	LFO's params count and param's index
*/
#define LFO_PARAMS 7

#define LFO_TYPE 	0
#define LFO_FREQ 	1

#define LFO_RAMP 	2
#define LFO_PHASE 	3
#define LFO_SIGN 	4

#define LFO_VAR1 	5
#define LFO_VAR2 	6

static const char* lfo_param_names[] 	= {"Waveform", "Frequency", "Ramp", "Phase", "Sign", "Param-1", "Param-2"};
static const float lfo_default_params[] = {0, 1.0f, 0.0f, 0.0f, 1, 0.0f, 0.0f};

static int RandSeed = 48172;

/*
*	List of possible LFO waveshape
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


class LFO_voice : public Module_voice{
	
	public :
	
		LFO_voice(jack_client_t *client, int idx);
		
		void set_param(int param, float var);
		void set_param_list(int size, float *pl);
		
	    sample_t (*waveform_)(float, float, float, float); // waveform generator function
		int get_sr() const;
		
		void update_type(LFO_wave type);
		
	protected :
	
		int samplerate_;
};

/*
*	LFO Module
*/
class LFO : public Module{
	
	public:
		
		LFO(const char *server, int vc);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		void sync();
		
		const char* get_param_name(int p) const;
		
	protected:
			
	/*
		LFO_Wave type;
		float f; 		//LFO frequency
		int sr;			//Client Samplerate
		
		float ramp;		//Current LFO value
		float phase;	//LFO Phase
		float sign;		//LFO sign
		
		float p1;		//waveshape param 1
		float p2;		//waveshape param 2
	*/
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
