#ifndef DEF_DELAY_H
#define DEF_DELAY_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Buffer.h"

/*
*	Default values for delay
*/
static const float default_delay_values[] = {200, 0.5f, 0.5f};

/*
*	Delay's params count and param's index
*/
#define DELAY_PARAMS 3

#define DELAY_DELAY 0
#define DELAY_FEEDB 1
#define DELAY_DRYWET 2

static const char* delay_param_names[] = {"Delay", "Feedback", "Dry-Wet"};

/*
*	Functuion for register write callback
*/
int delay_Process_Callback(jack_nframes_t nframes, void *u);

/*
*
*/
class Delay_voice : public Module_voice{
	
	public :
		
		Delay_voice(jack_client_t *client, jack_client_t *client_2, int idx);
		~Delay_voice();
		
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
		
		jack_port_t* get_p_send() const;
		jack_port_t* get_p_return() const;
		
		Buffer_S* get_buffer() const;
		
		void connect(jack_client_t *client);
		
	protected :
	
		int samplerate;
	
		jack_port_t *p_send_, *p_return_;	//Private port connected between read client and write client
		
		Buffer_S *buffer_;
};

/*
*	Singletap delay module
*/
class Delay : public Module{

	public:
	
		Delay(const char *server, int vc);
		~Delay();
		
		int process(jack_nframes_t nframes, void *arg);
		int process_2(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
	
	protected:
	
		jack_client_t *client_2_;
		char* name_2_;
	
		/*
		* Params are :
		*
		* delay
		* feedback
		*
		* dry _ wet
		*/
};

#endif
