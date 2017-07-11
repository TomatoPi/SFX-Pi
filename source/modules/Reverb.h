#ifndef DEF_REVERB_H
#define DEF_REVERB_H

#include <jack/jack.h>

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Filter.h"

#define R_DAMP 0.4f
#define R_ROOM 0.5f
#define R_ALLF 0.5f
#define R_GAIN 0.015f
#define R_DRY 0.5f
#define R_WIDTH 1.0f

#define REV_ROOM 0
#define REV_DAMP 1
#define REV_WET  2
#define REV_DRY  3
#define REV_ALL 4

#define COMBCOUNT 8
#define ALLPCOUNT 4
#define STEREOSPREAD 23

#define scalewet 3.0f
#define scaledry 2.0f
#define scaleroom 0.28f
#define offsetroom 0.7f
#define scaledamp 0.5f

static const int combsize[COMBCOUNT] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
static const int allpsize[ALLPCOUNT] = {556, 441, 341, 225};

static const int    REVERB_PARAMS_COUNT = 5;
static const string REVERB_PARAM_NAMES[REVERB_PARAMS_COUNT] = {"Roomsize", "Damp", "Wet", "Dry", "Allpf"};
static const float  REVERB_DEFAULT_PARAMS[REVERB_PARAMS_COUNT] = {0.5f, 0.5f, 1.0f, 0.0f, 0.5f};


class Reverb : public Module{
	
	public:
		
		Reverb(const char *server);

        virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
    
		void update();          /**< Update and scale reverb parameters */
        
        int samplerate_;
	
		float const gain_;
	
		Filter_comb     comb_[COMBCOUNT];
		Filter_allpass  allp_[ALLPCOUNT];
		
		float room_;
		float damp_;
		float wet_;
		float dry_;
		float allpf_;
};

#endif
