#ifndef DEF_MODULES_H
#define DEF_MODULES_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <vector>

#include <jack/jack.h>

#define PORT_TYPE_COUNT 4
#define PORT_AI 0
#define PORT_AO 1
#define PORT_MI 2
#define PORT_MO 3

static const float PARAM_NOT_FOUND = 859457.0f;

/*
*   Function for register class member callback functions
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u);
int mod_Bypass_Callback(jack_nframes_t nframes, void *u);

/*
*	Function for register Modules ports array
*	Only use it in constructor
*	client	 : client port's owner
*	port_tab : array of ports, [audio_in][], [audio_out][], [midi_in][], [midi_out][]
*	type	 : Type of the ports
*	count	 : Number of ports
*	...		 : Names of ports
*/
void register_port(jack_client_t *client, jack_port_t **port_tab, int type, int count, ...);

/*
*	Basic callback that copy input to output
*	Assuming ports are the first of port[PORT_AI] and port[PORT_AO]
*/
int mod_generic_bypass_callback(jack_nframes_t nframes, jack_port_t *port_in, jack_port_t *port_out);

/*
*	Enum constainig list of all avaiable modules
*/
typedef enum{
	
	MDRIVE	=0, 
	MDELAY	=1, 
	MLFO	=2, 
	MRINGM	=3, 
	MTONE	=4, 
	MREV	=5,
	MCHORUS	=6
}MODULE_TYPE;

/*
*	Cast enum to char*
*/
const char* mod_tton(MODULE_TYPE type);

class Module_voice{
	
	public :
	
		/*
		*	Only calloc arrays, module constructor MUST register io and set default params
		*/
		Module_voice(jack_client_t *client, int pc, int ai, int ao, int mi, int mo);
		~Module_voice();
		
		virtual void set_param(int param, float var);		// Set param_[] at param value to var
		virtual void set_param_list(int size, float *pl);	// Change all param's in a single row
		float 	get_param(int param) const;					// Return param_[]'s value at param
		int 	get_param_count() const;					// Return total number of params
	
		
		jack_port_t* 	get_port(int type, int port) const;	// Return port at [type][idx]
		jack_port_t** 	get_port_array(int type) const;		// Return array of ports
		int				get_port_count(int type) const;		// Return number of port for given type
	
		int is_ready() const;	// Return true if voice fully constructed
		
	protected :
	
		int is_ready_;
		
		jack_port_t **port_audio_in_, **port_audio_out_,  **port_midi_in_,  **port_midi_out_;
		int			port_count_[PORT_TYPE_COUNT];	// Number of ports, same order as port_
		
		float		*param_;		// Params array
		int			param_count_;	// Number of params
};

/*
*	Alias for module's voices array
*/
typedef std::vector<Module_voice*> Voice_array;

/*
*	Basic class for all modules
*/
class Module{
	
	public:
		/*
		*	Constructor for all modules, setup jack client and register the callback function
		* 	server 	: server name
		*	type 	: type of the module
		*	pc 		: number of module's parameter
		*	vc		: unmber of module voices
		*/
		Module(const char *server, MODULE_TYPE type, int vc);
		~Module();
		
		virtual int process(jack_nframes_t nframes, void *arg){}; 	// Client's callback function
		virtual int bypass(jack_nframes_t nframes, void *arg){}; 	// Client's callback function when client is bypassed
	
		void 	set_bypass(int state);
		int 	get_bypass() const;
		
		Module_voice* 	get_voice(int idx) const;	// Return pointer to the Module_Voice at idx or at 0 if idx too big
		virtual void	add_voice() {};				// Add a voice to the module
		void 			del_voice(int idx);			// Remove voice at idx
		
		int 			get_voice_count() const;	// Return number of voices
		
		virtual const char* get_param_name(int p) const{};	//	Used for user-interface, return name for param at i ex : "Gain"
		
		jack_client_t* 	get_client() const;
		
		int 	get_type();		//	Return code type
	
	protected:
	
		jack_client_t 	*client_;	// JACK Client
		char* 			name_;		// Client unique name
		MODULE_TYPE 	type_;		// Module's type
		
		Voice_array 	voice_;		// Array of voice
		
		int 			is_bypassed_;	// True if Module is bypassed
};

#endif