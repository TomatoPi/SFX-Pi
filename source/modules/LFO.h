#ifndef DEF_LFO_H
#define DEF_LFO_H

#include <jack/jack.h>

#include "../core/Utility.h"
#include "../core/Modules.h"

#define L_TYPE WAVE_SIN
#define L_FREQ 1.0
#define L_SPLR 48000

#define L_RAMP 0.0
#define L_PHASE 0.0
#define L_SIGN 1.0

#define L_PAR1 1.0
#define L_PAR2 1.0

/*
*	List of possible LFO waveshape
*	Sinus
*	Square
*	Triangular
*	Sawtooth
*	Varislope
*	N-Phase
*/
class LFO_Wave{
	
	public:
		static const LFO_Wave WAVE_SIN = LFO_Wave(0);
		static const LFO_Wave WAVE_SQR = LFO_Wave(1);
		static const LFO_Wave WAVE_TRI = LFO_Wave(2);
		static const LFO_Wave WAVE_SAW = LFO_Wave(3);
		static const LFO_Wave WAVE_VAR = LFO_Wave(4);
		static const LFO_Wave WAVE_NPH = LFO_Wave(5);
		
		operator float() const {return (float)this->val;}
		operator int() const {return (int)this->val;}
	private:
		LFO_Wave(int v){this->val = v;}
		int val;
};

/*
*	LFO Module
*/
class LFO : public Module{
	
	public:
		
		LFO(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
    
    		friend void lfo_set_type(LFO *lfo, LFO_Wave type);
		
	protected:
	
	    	sample_t (*waveform)(float, float, float, float); // waveform generator function		
	/*
		LFO_Wave type;
		float f; 		//LFO frequency
		int sr;			//Client Samplerate
		
		float ramp;		//Current LFO value
		float phase;		//LFO Phase
		float sign;		//LFO sign
		
		float p1;		//waveshape param 1
		float p2;		//waveshape param 2
	*/
};

/*
*	Setup waveform function
*/
void lfo_set_type(LFO *lfo, LFO_Wave type);

/*
*	Waveform functions, see wiki
*/
sample_t w_sin(float in, float sign, float p1, float p2);	//Sinusoid : p1, p2 unused
sample_t w_sqr(float in, float sign, float p1, float p2);	//Square : p1, p2 unused
sample_t w_tri(float in, float sign, float p1, float p2);	//Triangular : p1, p2 unused
sample_t w_saw(float in, float sign, float p1, float p2);	//Sawtooth : sign > 0, ascendent ; sign < 0 descendent
sample_t w_var(float in, float sign, float p1, float p2);	//Varislope : p1 first extremum position ( 0 - 100% ) ; p2 transition form ( 0 : square, 1 : triangle )
sample_t w_nph(float in, float sign, float p1, float p2);	//N-Phase : p1 phase quantity (int) ; p2 cutting quantity (int)

#endif
