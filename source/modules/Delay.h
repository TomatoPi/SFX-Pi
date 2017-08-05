#ifndef DEF_DELAY_H
#define DEF_DELAY_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Buffer.h"

/*
*	Delay's params count and param's index
*/
#define DELAY_DELAY 0
#define DELAY_FEEDB 1
#define DELAY_DRYWET 2

/*
*	Default values for delay
*/
static const int    DELAY_PARAMS_COUNT = 3;
static const float  DELAY_DEFAULT_PARAMS[DELAY_PARAMS_COUNT] = {200, 0.5f, 0.5f};
static const string DELAY_PARAM_NAMES[DELAY_PARAMS_COUNT] = {"Delay", "Feedb", "DryWet"};

/*
*	Functuion for register write callback
*/
int delay_Process_Callback(jack_nframes_t nframes, void *u);

/**
*	Single tape delay module.
*   A delay module is formed by two submodules
*   The first one is the reading head, delay normal input is here
*   It read data comming from delay's buffer and send normal output
*   First module also the feedback loop's send port
*
*   The second part is the writing head, it take normal input from read head
*   and summ it to feedback loop's return for write datas in buffer
*
*   When delay is bypassed the read head continue it normal job but stop send
*   normal input to buffer ( only the feddback loop is maintained )
*   And the write head 
*/
class Delay : public Module{

	public:
	
		Delay(const char *server);
        ~Delay();
        
        /**
        *   Delay's second process callback.
        *   Process called by delay's second part for write data in buffer
        *   When delay is bypassed it stop read input and only write
        *   feedback loop for a tail effect
        *
        *   @see process(jack_nframes_t nframes, void *arg)
        */
        int process_2(jack_nframes_t nframes, void *arg);
        
		virtual int process(jack_nframes_t nframes, void *arg);
		virtual int bypass(jack_nframes_t nframes, void *arg);
	
	protected :
    
        virtual void change_param(int idx, float value); /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values);        /**< @see set_param(float *values) */
    
        virtual string return_param_name(int idx);       /**< @see get_param_name(int idx) */
        virtual string return_formated_param(int idx);   /**< @see get_formated_param(int idx) */
  
        virtual void new_bank();    /**< @see add_bank() */
    
		jack_client_t *client_2_;
		char* name_2_;
	
        int samplerate_;
	
		jack_port_t *p_send_, *p_return_;	/**< Private port connected between read client and write client*/
		
		Buffer_S buffer_;
};

#endif
