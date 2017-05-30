#include "main.h"

const char *SERVER_NAME = "Space_Fx";

std::unordered_map<int, Module*> MAIN_LIST_MODULE;
int MAIN_COUNT_MODULE = 0;

std::unordered_map<int, io_param_accessor*> MAIN_LIST_ACCESSOR;
int MAIN_COUNT_ACCESSOR = 0;

int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n------------------\n");
	io_init_spi();
	
	main_add_module(MDRIVE);
	int drive = MAIN_COUNT_MODULE;
	float drivep[] = {0, 1, 20.0, 1, 8.0, 0.26, 10.0, 1, 10.0, 0.6};
	MAIN_LIST_MODULE[drive]->set_param_list(10, drivep);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[drive], 0);
	main_add_connection(NULL, 0, MAIN_LIST_MODULE[drive], 1);
	main_add_accessor(drive, 2, 0, 10.0, 40.0 ,1, 0);
	main_add_accessor(drive, 6, 0, 10.0, 30.0, 1, 1);
	
	main_add_module(MDELAY);
	int delay = MAIN_COUNT_MODULE;
	float delayp[] = {5000.0, 250.0, 180.0, 0.75, 0.8, 0.5};
	MAIN_LIST_MODULE[delay]->set_param_list(2, delayp);
	main_add_accessor(delay, 1, 1, 50.0, 1000.0, 0, 0);
	main_add_accessor(delay, 2, 2, 50.0, 1000.0, 0, 0);
	main_add_accessor(delay, 3, 3, 0.0, 1.0, 0, 0);
	main_add_accessor(delay, 4, 4, 0.0, 1.0, 0, 0);
	main_add_accessor(delay, 5, 5, 0.0, 1.0, 0, 0);
	
	main_add_connection(MAIN_LIST_MODULE[drive], 2, MAIN_LIST_MODULE[delay], 0);
	main_add_connection(MAIN_LIST_MODULE[drive], 3, MAIN_LIST_MODULE[delay], 1);
	
	main_add_connection(MAIN_LIST_MODULE[delay], 2, NULL, 0);
	main_add_connection(MAIN_LIST_MODULE[delay], 3, NULL, 1);
	
	while(1){
	
		unordered_map<int, io_param_accessor*>:: iterator itr;
		for (itr = MAIN_LIST_ACCESSOR.begin(); itr != MAIN_LIST_ACCESSOR.end(); itr++)
		{
			// type itr->first stores the key part  and
			// itr->second stroes the value part
			itr->second->io_update_param();
		 }
		usleep(50000);
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
	fprintf (stderr, "Added new module : %d\n", MAIN_COUNT_MODULE);
	
	return 0;
}

int main_del_module(int idx){
	
	if(MAIN_LIST_MODULE.find(idx) == MAIN_LIST_MODULE.end())
		return 1;
	
	MAIN_LIST_MODULE.erase(idx);
	return 0;
}

const char* get_source_name(Module *source, int is, Module *target){
	
	const char *from;
	if(source == NULL){
		fprintf (stderr, "Capture source --- ");
		
		const char **port;
		port = jack_get_ports (target->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
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
			fprintf (stderr, "\n***Failed get source port***\n");
			return NULL;
		}
	}
	return from;
}
	
const char* get_target_name(Module *target, int id, Module *source){
	
	const char *to;
	if(target == NULL){
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
		
		if(target->get_port(id) != NULL){
			to = jack_port_name(target->get_port(id));
		}else{
			fprintf (stderr, "***Failed get destination port***\n");
			return NULL;
		}
	}
	return to;
}

int main_add_connection(Module *source, int is, Module *target, int id){
	
	const char *source_port = get_source_name(source, is, target);
	const char *target_port = get_target_name(target, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_connect (target->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}else{
		if (jack_connect (source->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}
	fprintf (stderr, "New connection made\n");
	
	return 0;
}

int main_del_connection(Module *source, int is, Module *target, int id){
	
	const char *source_port = get_source_name(source, is, target);
	const char *target_port = get_target_name(target, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_disconnect (target->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}else{
		if (jack_disconnect (source->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
	}
	fprintf (stderr, "Connection removed\n");
	
	return 0;
}

int main_add_accessor(int target, int param_idx, int potentiometer, float min, float max, int is_db, int is_inv){
	
	fprintf (stderr, "Add new accessor --- ");
	
	if(MAIN_LIST_MODULE.find(target) == MAIN_LIST_MODULE.end()){
		fprintf (stderr, "\nTarget not found\n");
	   	return 1;
	}
	
	if(MAIN_LIST_MODULE[target]->get_param_adress(param_idx) == NULL){
		fprintf (stderr, "\nParam target not found\n");
		return 1;
	}
	
	while(MAIN_LIST_ACCESSOR.find(MAIN_COUNT_ACCESSOR) != MAIN_LIST_ACCESSOR.end())
		MAIN_COUNT_ACCESSOR++;
	
	fprintf (stderr, "Idx : %d -- Target : %d.%d -- Pot : %d -- Min : %f -- Max : %f\n", MAIN_COUNT_ACCESSOR, target, param_idx, potentiometer, min, max);
	io_param_accessor *accessor = new io_param_accessor(potentiometer, min, max, MAIN_LIST_MODULE[target]->get_param_adress(param_idx), is_db, is_inv);
	MAIN_LIST_ACCESSOR.insert(make_pair(MAIN_COUNT_ACCESSOR, accessor));
	return 0;
}
	
int main_del_accessor(int idx){
	
	if(MAIN_LIST_ACCESSOR.find(idx) == MAIN_LIST_ACCESSOR.end())
		return 1;
	
	MAIN_LIST_ACCESSOR.erase(idx);
	return 0;
}
