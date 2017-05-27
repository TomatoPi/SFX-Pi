#include "main.h"

const char *SERVER_NAME = "Space_Fx";

std::unordered_map<int, Module*> MAIN_LIST_MODULE;
int MAIN_COUNT_MODULE = 0;

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	
	int i = main_add_module(DRIVE);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[i], 0);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[i], 1);
	
	main_add_connection(MAIN_LIST_MODULE[i], 2, NULL, 0);
	main_add_connection(MAIN_LIST_MODULE[i], 3, NULL, 1);
	
	sleep(-1);
}

int main_add_module(MODULE_TYPE mod){
	
	while(MAIN_LIST_MODULE.find(MAIN_COUNT_MODULE) == MAIN_LIST_MODULE.end())
		MAIN_COUNT_MODULE++;
	
	Module *newmod;
	switch(mod){
		case DRIVE:
			newmod = new Drive(SERVER_NAME);
			fprintf (stderr, "new Drive \n");
			break;
		case DELAY:
			newmod = new Delay(SERVER_NAME);
			fprintf (stderr, "new Delay \n");
			break;
		case LFO:
			newmod = new LFO(SERVER_NAME);
			fprintf (stderr, "new LFO \n");
			break;
		case RINGM:
			newmod = new Ringmod(SERVER_NAME);
			fprintf (stderr, "new Ringmod \n");
			break;
		default:
			newmod = NULL;
			break;
	}
	
	if(newmod==NULL) return 1;
	
	MAIN_LIST_MODULE.insert(make_pair(MAIN_COUNT_MODULE, newmod));
	fprintf (stderr, "Added new module ! \n");
	
	return 0;
}

int main_add_connection(Module *source, int is, Module *destination, int id){
	
	const char *from, *to;
	
	if(source == NULL && destination == NULL)
		return 1;
	
	if(source == NULL){
		fprintf (stderr, "Capture source --- ");
		
		const char **port;
		port = jack_get_ports (destination->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
		if (port == NULL) {
			fprintf(stderr, "no physical capture ports\n");
			exit (1);
		}
		from = port[(is==0)?0:1];
		free(port);
	}else{
		fprintf (stderr, "Module source --- ");
		
		if(source->get_port(is) != NULL){
			from = jack_port_name(source->get_port(is));
		}else{
			from = NULL;
		}
	}
	if(destination == NULL){
		fprintf (stderr, "Playback destination\n");
		
		const char port**;
		port = jack_get_ports (client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
		if (port == NULL) {
			fprintf(stderr, "no physical playback ports\n");
			exit (1);
		}
		to = port[(id==0)?0:1];
		free(port);
	}else{
		fprintf (stderr, "Module destination\n");
		
		if(destination->get_port(id) != NULL){
			to = jack_port_name(destination->get_port(id));
		}else{
			to = NULL;
		}
	}
	
	if(from == NULL || to == NULL)
		return 1;
	
	if (jack_connect (source->get_client(), from, to)) {
		fprintf (stderr, "cannot connect input ports\n");
		return 1;
	}
	fprintf (stderr, "New connection made\n");
	
	return 0;
}
