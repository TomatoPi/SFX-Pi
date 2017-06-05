#ifndef DEF_DELAY_H
#define DEF_DELAY_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Buffer.h"
#include "../core/Filter.h"

#define D_DMAX 5000
#define D_DELAY 250
#define D_FEED 0.5

int delay_Process_Callback(jack_nframes_t nframes, void *u);

class Delay : public Module{

	public:
		Delay(const char *server);
		~Delay();
		
		int process(jack_nframes_t nframes, void *arg);
		int process_2(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		int set_param(int param, float var);
		int set_param_list(int size, float *params);
	
	protected:
	
		jack_client_t *client_2;
		char *name_2;
	
		jack_port_t **private_port;
		int private_port_count;
		
		Buffer_S *buffer_L;
		Buffer_S *buffer_R;
	
		/*
		* Params are :
		*
		* delay_L
		* delay_R
		* feedback_L
		* feedback_R
		*
		* dry _ wet
		*/
};

#endif
