#include "main.h"

const char *SERVER_NAME = "Space_Fx";

std::unordered_map<int, Module*> MAIN_LIST_MODULE;
int MAIN_COUNT_MODULE = 0;

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	io_init_spi();
	
	int i = main_add_module(MDRIVE);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[i], 0);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[i], 1);
	
	main_add_connection(MAIN_LIST_MODULE[i], 2, NULL, 0);
	main_add_connection(MAIN_LIST_MODULE[i], 3, NULL, 1);
	
	io_param_accessor test* = new io_param_accessor(0, 10, 40, &(MAIN_LIST_MODULE[i]->get_param(2)), 1);
	io_param_accessor test1* = new io_param_accessor(1, 0, 20, &(MAIN_LIST_MODULE[i]->get_param(6)), 1);
	
	while(1){
	
		test->io_update_param();
		test1->io_update_param();
		usleep(10000);
	}
	
	sleep(-1);
}

int main_add_module(MODULE_TYPE mod){
	
	while(MAIN_LIST_MODULE.find(MAIN_COUNT_MODULE) != MAIN_LIST_MODULE.end()){
		MAIN_COUNT_MODULE++;
	}
	fprintf (stderr, "Add module --- ");
	
	Module *newmod;
	switch(mod){
		case MDRIVE:
			newmod = new Drive(SERVER_NAME);
			fprintf (stderr, "new Drive\n");
			break;
		case MDELAY:
			newmod = new Delay(SERVER_NAME);
			fprintf (stderr, "new Delay\n");
			break;
		case MLFO:
			newmod = new LFO(SERVER_NAME);
			fprintf (stderr, "new LFO\n");
			break;
		case MRINGM:
			newmod = new Ringmod(SERVER_NAME);
			fprintf (stderr, "new Ringmod\n");
			break;
		default:
			newmod = NULL;
			break;
	}
	
	if(newmod==NULL) return 1;
	
	MAIN_LIST_MODULE.insert(make_pair(MAIN_COUNT_MODULE, newmod));
	fprintf (stderr, "Added new module !\n");
	
	return 0;
}

int main_del_module(int idx){
	
	if(MAIN_LIST_MODULE.find(idx) == MAIN_LIST_MODULE.end())
		return 1;
	
	MAIN_LIST_MODULE.erase(idx);
	return 0;
}

char** get_ports_names(Module *source, int is, Module *destination, int id){
	
	const char *from, *to;
	int is_source_null = 0;
	
	if(source == NULL && destination == NULL)
		return NULL;
	
	if(source == NULL){
		fprintf (stderr, "Capture source --- ");
		
		const char **port;
		port = jack_get_ports (destination->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
		if (port == NULL) {
			fprintf(stderr, "no physical capture ports\n");
			exit (1);
		}
		from = port[(is==0)?0:1];
		is_source_null = 1;
		free(port);
	}else{
		fprintf (stderr, "Module source --- ");
		
		if(source->get_port(is) != NULL){
			from = jack_port_name(source->get_port(is));
		}else{
			fprintf (stderr, "\n***Failed get source port***\n");
			return NULL;
		}
	}
	if(destination == NULL){
		fprintf (stderr, "Playback destination\n");
		
		const char **port;
		port = jack_get_ports (source->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsInput);
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
			fprintf (stderr, "***Failed get destination port***\n");
			return NULL;
		}
	}
	
	char *names[] = {from, to};
	return names;
}

int main_add_connection(Module *source, int is, Module *target, int id){
	
	char *names[] = get_ports_names(source, is, target, id);
	if(names == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_connect (destination->get_client(), names[0], names[1])) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}else{
		if (jack_connect (source->get_client(), names[0], names[1])) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}
	fprintf (stderr, "New connection made\n");
	
	return 0;
}

int main_del_connection(Module *source, int is, Module *target, int id){
	
	char *names[] = get_ports_names(source, is, target, id);
	if(names == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_disconnect (destination->get_client(), names[0], names[1])) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}else{
		if (jack_disconnect (source->get_client(), names[0], names[1])) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}
	fprintf (stderr, "Connection removed\n");
	
	return 0;
}
