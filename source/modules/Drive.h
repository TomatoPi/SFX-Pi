#ifndef DEF_DRIVE_H
#define DEF_DRIVE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

#include "../core/Filter.h"
#include "../core/Modules.h"



/*
*	Drive's params count and param's index
*/
#define DRIVE_PARAMS 15

#define DRIVE_ABS 0
#define DRIVE_ASM 1

#define DRIVE_GAIN_P 2
#define DRIVE_TYPE_P 3
#define DRIVE_SOFT_P 4
#define DRIVE_SHAPE_P 5

#define DRIVE_GAIN_N 6
#define DRIVE_TYPE_N 7
#define DRIVE_SOFT_N 8
#define DRIVE_SHAPE_N 9

#define DRIVE_F_BASS 10
#define DRIVE_F_HIGH 11
#define DRIVE_F_GBASS 12
#define DRIVE_F_GMID 13
#define DRIVE_F_GHIGH 14

/*
*	Default values for drive module
*/
static const float default_drive_values[DRIVE_PARAMS] = {0, 0, 20, 1, 5, 0.5f, 20, 1, 5, 0.5f, 200, 1000, 0.75f, 3.0f, 7.0f};
static const char* drive_param_names[] = {"Fullwave", "Asymetrical", 
											"Gain-p", "Soft-Clipping-p", "Softness-p", "Shape-p",
											"Gain-n", "Soft-Clipping-n", "Softness-n", "Shape-n",
											"Lowcut", "Highcut", "Lowgain", "Midgain", "Highgain"};

class Drive_voice : public Module_voice{
	
	public :
	
		Drive_voice(jack_client_t *client, int idx);
		~Drive_voice();
		
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
		
		Filter_3EQ* get_filter() const;
		
	protected :
	
		Filter_3EQ *filter;
};
/*
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:	
		
		Drive(const char *server, int vc);
		~Drive();
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
	
	protected :
	
		/*
		* Specific parameters for drive module are, in order :	unique parameters -> positives parameters -> negatives parameters
		*
		*	int is_abs; //for full-wave rectification
		*	int is_asm; //for asymmetrical clipping : _p parameter for positives half-cycles and _n for negatives half-cycles
		*
		*	float gp, gn; //Gain applied to the signal
		*	int is_soft_clip_p, is_soft_clip_n; //for Soft or Hard-clipping
		*	float sp, sn; //Soft-Clipping Softness
		*	float shp, shn; //Soft-Clipping Shape
		*
		*	float freq_low
		* 	float freq_high
		*	float gain_low
		*	float gain_mid
		*	float gain_high
		*/
};

#endif
