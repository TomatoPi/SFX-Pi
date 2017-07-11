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
static const int    DRIVE_PARAM_COUNT = 15;
static const float  DRIVE_DEFAULT_PARAMS[DRIVE_PARAM_COUNT] = {0, 0, 20, 1, 5, 0.5f, 20, 1, 5, 0.5f, 200, 1000, 0.75f, 3.0f, 7.0f};
static const string DRIVE_PARAM_NAMES[DRIVE_PARAM_COUNT] = {"Fullwave", "Asymetrical", 
											"Gain-p", "Soft-Clipping-p", "Softness-p", "Shape-p",
											"Gain-n", "Soft-Clipping-n", "Softness-n", "Shape-n",
											"Lowcut", "Highcut", "Lowgain", "Midgain", "Highgain"};

/**
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:	
		
		Drive(const char *server);
		
		virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */

        Filter_3EQ filter_;     /**< Drive's filter @see Filter_3EQ */
};

#endif
