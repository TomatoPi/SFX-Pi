#ifndef DEF_CHORUS_H
#define DEF_CHORUS_H

#include "../core/Modules.h"
#include "../core/Buffer.h"
#include "../core/Utility.h"

/*
*	Default values for chorus
*/
static const float default_chorus_values[] = {0.1, 1.0};
static const char* chorus_params_name[] = {"Depth", "Drywet"};

#define CHORUS_SIZE 3

/*
*	Chorus param's count and adresses
*/
#define CHORUS_PARAMS 2

#define CHORUS_DEPTH  0
#define CHORUS_DRYWET 1


class Chorus_voice : public Module_voice{
	
	public :
	
		Chorus_voice(jack_client_t *client, int idx);
		~Chorus_voice();
		
		virtual void set_param(int param, float var);
		virtual void set_param_list(int size, float *pl);
		
		Buffer_M* get_buffer() const;
	
	private :
	
		int samplerate_;
		Buffer_M *buffer_;
};

class Chorus : public Module{
	
	public :
	
		Chorus(const char* server, int vc);
		
		int process(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
		
		void add_voice();
		
		const char* get_param_name(int p) const;
		
	private :
	
		
};

#endif