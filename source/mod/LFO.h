#ifndef DEF_OSC
#define DEF_OSC

#include <jack/jack.h>

#include "../utility/utility.h"

typedef enum{
	
	WAVE_SIN, WAVE_SQR, WAVE_TRI, WAVE_SAW, WAVE_VAR, WAVE_NPH
}LFO_Wave;

class LFO : public Fx_Client{
	
	public:
		
		LFO(const char *server, const char *name);
		
		int process(jack_nframes_t nframes, void *arg);
		
		sample_t (*waveform)(float, float, float, float);
		LFO_Wave type;
		
	protected:
		
		jack_port_t *out;
		
		float f;
		int sr;
		
		float ramp;
		float phase;
		float sign;
		
		float p1;
		float p2;
};

void lfo_set_type(LFO *lfo, LFO_Wave type);

sample_t w_sin(float in, float sign, float p1, float p2);
sample_t w_sqr(float in, float sign, float p1, float p2);
sample_t w_tri(float in, float sign, float p1, float p2);
sample_t w_saw(float in, float sign, float p1, float p2);
sample_t w_var(float in, float sign, float p1, float p2);
sample_t w_nph(float in, float sign, float p1, float p2);

#endif