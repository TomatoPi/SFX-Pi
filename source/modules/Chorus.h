#ifndef DEF_CHORUS_H
#define DEF_CHORUS_H

#include "../core/Modules.h"
#include "../core/Buffer.h"
#include "../core/Utility.h"

#define CHORUS_SIZE 3

/*
*	Chorus param's count and adresses
*/

#define CHORUS_DEPTH  0
#define CHORUS_DRYWET 1

/*
*	Default values for chorus
*/
static const int    CHORUS_PARAMS_COUNT = 2;
static const float  CHORUS_DEFAULT_PARAMS[CHORUS_PARAMS_COUNT] = {0.1, 1.0};
static const string CHORUS_PARAM_NAMES[CHORUS_PARAMS_COUNT] = {"Depth", "Drywet"};
static const int    CHORUS_DELAYS_LENGTH[CHORUS_SIZE] = {7, 15, 18};

class Chorus : public Module{
	
	public :
	
		Chorus(const char* server);
        ~Chorus();
		
		virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */
    
        virtual void new_bank();    /**< @see add_bank() */
    
		int samplerate_;    /**< Current semplerate */
		Buffer_M *buffer_;  /**< Choru's buffer */
};

#endif