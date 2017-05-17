#ifndef DEF_MODULES_H
#define DEF_MODULES_H

/*
*   	Function for register class member callback function
*/
private int mod_Process_Callback(jack_nframes_t nframes, void *u);

/*
*	Basic class for all modules
*/
class Module{
	
	public:
		//see mod_register_ports for additional params
		Module(const char *server, const char *name, int port_count, const char **port_names, const char **port_types, unsigned long **port_flags);
		
		virtual int process(jack_nframes_t nframes, void *arg){}; // Client's callback functio
	
	protected:
	
		jack_client_t *client;	//JACK Client
		char* name;		//Client unique name
	
	private:
		/*
		*	Create and register ports for this Module
		*	Takes number of ports and arrays of port names, port types, port flags
		*/
		void mod_Register_Port(int port_count, const char **port_names, const char **port_types, unsigned long **port_flags);
};

#endif
