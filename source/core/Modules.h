#ifndef DEF_MODULES_H
#define DEF_MODULES_H

/*
*   Function for register class member callback function
*/
private int mod_Process_Callback(jack_nframes_t nframes, void *u);

/*
*	  Basic class for all modules
*/
class Mod_Module{
	
	public:
	
		Mod_Module(const char *server, const char *name);
		
		virtual int process(jack_nframes_t nframes, void *arg){}; // Client's callback function
		
	protected:
	
		jack_client_t *client;	//JACK Client
		char* name;		//Client unique name
};

#endif
