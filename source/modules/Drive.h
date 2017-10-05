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
#define DRIVE_ABS       MOD_COUNT + 0
#define DRIVE_ASM       MOD_COUNT + 1
#define DRIVE_OFFSET    MOD_COUNT + 2

#define DRIVE_GAIN_P  MOD_COUNT + 3
#define DRIVE_TYPE_P  MOD_COUNT + 4
#define DRIVE_SOFT_P  MOD_COUNT + 5
#define DRIVE_SHAPE_P MOD_COUNT + 6

#define DRIVE_GAIN_N  MOD_COUNT + 7
#define DRIVE_TYPE_N  MOD_COUNT + 8
#define DRIVE_SOFT_N  MOD_COUNT + 9
#define DRIVE_SHAPE_N MOD_COUNT + 10

#define DRIVE_F_BASS  MOD_COUNT + 11
#define DRIVE_F_HIGH  MOD_COUNT + 12
#define DRIVE_F_GBASS MOD_COUNT + 13
#define DRIVE_F_GMID  MOD_COUNT + 14
#define DRIVE_F_GHIGH MOD_COUNT + 15

/*
*	Default values for drive module
*/
static const int    DRIVE_PARAM_COUNT = 16;
static const float  DRIVE_DEFAULT_PARAMS[MOD_COUNT+DRIVE_PARAM_COUNT] = {0, 1, 
                                            0, 0, 0,
                                            198, 1, 22, 0.29f, 
                                            63, 1, 15, 0.1f, 
                                            200, 1000, 0.5f, 2.0f, 3.0f};
static const string DRIVE_PARAM_NAMES[MOD_COUNT+DRIVE_PARAM_COUNT] = {"Bypass", "Volume", 
                                            "Fullwave", "Asymetric", "Offset",
											"Gain-p", "Type-p", "Soft-p", "Shape-p",
											"Gain-n", "Type-n", "Soft-n", "Shape-n",
											"Lowcut", "Highcut", "Lowgain", "Midgain", "Hghgain"};
/**
 * List of Avaiable distortions
 */
typedef enum{
    
    SOFT_CLIP   =0,
    HARD_CLIP   =1,
    POW2        =2
}DISTORTION_FORM;

/**
* 	Drive, Soft & Hard Clipping effects
*	EQ pre-drive
*	Possibility of asymmetrical clipping
*/
class Drive : public Module{
	
	public:	
		
		Drive();
		//virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
		
		virtual int do_process(jack_nframes_t nframes);
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */

        virtual void new_bank();    /**< @see add_bank() */
    
        void update();  /**< Convert boolean params to valid value */
        
        Filter_3EQ filter_;     /**< Drive's filter @see Filter_3EQ */
        
        void set_clipping( DISTORTION_FORM );
        
        sample_t (*clip_p)( sample_t, float, float);
        sample_t (*clip_n)( sample_t, float, float);
};

#endif
