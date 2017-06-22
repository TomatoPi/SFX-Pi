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

#define REVERB_PARAMS 5

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

static const char* reverb_param_names[] = {"Roomsize", "Damp", "Wet", "Dry", "Allpf"};
static const float reverb_default_params[] = {0.5f, 0.5f, 1.0f, 0.0f, 0.5f};

class Reverb_voice : public Module_voice{
	
	public :
	
		Reverb_voice(jack_client_t *client, int idx);
		~Reverb_voice();
	
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
	
		Filter_comb* 	get_comb(int idx) const;
		Filter_allpass* get_allp(int idx) const;
		
		void update();
		
	protected :
		
		Filter_comb *comb_[COMBCOUNT];
		Filter_allpass *allp_[ALLPCOUNT];
		
		float room_;
		float damp_;
		float wet_;
		float dry_;
		float allpf_;
};

class Reverb : public Module{
	
	public:
		
		Reverb(const char *server, int vc);
	
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
	
	protected:
	
		float gain_ = 0.015f;
	
		/*
		*	gain
		*
		*	dry
		*	wet1
		*	wet2
		*/
};

#endif
