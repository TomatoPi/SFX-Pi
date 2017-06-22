#include "main.h"

const char *SERVER_NAME = "Space_Fx";

static Module_Node_List MAIN_LIST_MODULE;
static int MAIN_COUNT_MODULE = 0;

static int MAIN_POTAR_VALUES[SPI_POTAR_COUNT] = {};

/*
*	---------------------------------------------------------------------------
*	Main programm function
*	---------------------------------------------------------------------------
*/
int main(int argc, char *argv[]){
	
	fprintf (stderr, "\nBienvenu dans le fantastique et magnifique software Space-FX\n\n-----------------v0.2\n");
	MAIN_SCREEN = new IO_screen();
	IO_Potentiometer* MAIN_POTAR_TAB[SPI_POTAR_COUNT];
	io_init_spi();
	io_init_potar_tab(MAIN_POTAR_TAB);
	
	main_add_module(MDRIVE, 2);
	int drive = MAIN_COUNT_MODULE;
	float drivep[] = {0, 1, 20.0, 1, 8.0, 0.26, 10.0, 1, 10.0, 0.6, 440, 1200, 3.0, 0.75, 5.0};
	float hardp[] = {0, 1, 20.0, 0, 0.0, 0.0, 10.0, 1, 5.0, 0.4, 200, 1200, 8.0, 1.5, 1.0};
	MAIN_LIST_MODULE[drive]->get_module()->get_voice(0)->set_param_list(15, drivep);
	MAIN_LIST_MODULE[drive]->get_module()->get_voice(1)->set_param_list(15, hardp);
	main_add_connection(-1, 0, 0, drive, 0, 0);
	main_add_connection(-1, 0, 0, drive, 1, 0);
	main_add_accessor(drive, 0, 2, 0, 10.0, 60.0, CURVE_LIN, 1, 0);
	main_add_accessor(drive, 0, 6, 0, 10.0, 40.0, CURVE_LIN, 1, 1);
	main_add_accessor(drive, 1, 2, 1, 10.0, 60.0, CURVE_LIN, 1, 0);
	main_add_accessor(drive, 1, 6, 1, 10.0, 40.0, CURVE_LIN, 1, 0);
	char n[20];
	strcpy(n, "Disto-1");
	MAIN_POTAR_TAB[0]->set_name(n);
	MAIN_POTAR_TAB[0]->set_plage(10, 60);
	strcpy(n, "Disto-2");
	MAIN_POTAR_TAB[1]->set_name(n);
	MAIN_POTAR_TAB[1]->set_plage(10, 60);
	
	main_add_module(MDELAY, 2);
	int delay = MAIN_COUNT_MODULE;
	float delayr[] = {166, 0.6f, 0.4f};
	float delayl[] = {250, 0.6f, 0.4f};
	((Delay_voice*)MAIN_LIST_MODULE[delay]->get_module()->get_voice(0))->set_param_list(3, delayr);
	((Delay_voice*)MAIN_LIST_MODULE[delay]->get_module()->get_voice(1))->set_param_list(3, delayl);
	main_add_connection(drive, 0, 0, delay, 0, 0);
	main_add_connection(drive, 1, 0, delay, 1, 0);
	main_add_accessor(delay, 0, 0, 2, 100, 500, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 1, 0, 2, 150, 750, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 0, 1, 3, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(delay, 1, 1, 3, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	strcpy(n, "Delay-T");
	MAIN_POTAR_TAB[2]->set_name(n);
	MAIN_POTAR_TAB[2]->set_plage(100, 500);
	strcpy(n, "Delay-F");
	MAIN_POTAR_TAB[3]->set_name(n);
	MAIN_POTAR_TAB[3]->set_plage(0, 90);
	
	main_add_module(MREV, 2);
	int rev = MAIN_COUNT_MODULE;
	float reverb[] = {0.7f, 0.4f, 1.0f, 0.0f, 0.6f};
	MAIN_LIST_MODULE[rev]->get_module()->get_voice(0)->set_param_list(5, reverb);
	MAIN_LIST_MODULE[rev]->get_module()->get_voice(1)->set_param_list(5, reverb);
	main_add_connection(delay, 0, 1, rev, 0, 0);
	main_add_connection(delay, 1, 1, rev, 1, 0);
	main_add_connection(rev, 0, 0, delay, 0, 1);
	main_add_connection(rev, 1, 0, delay, 1, 1);
	main_add_accessor(rev, 0, 0, 4, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 1, 0, 4, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 0, 4, 5, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	main_add_accessor(rev, 1, 4, 5, 0.0f, 0.9f, CURVE_LIN, 0, 0);
	strcpy(n, "R-Room");
	MAIN_POTAR_TAB[4]->set_name(n);
	MAIN_POTAR_TAB[4]->set_plage(0, 90);
	strcpy(n, "R-Feed");
	MAIN_POTAR_TAB[5]->set_name(n);
	MAIN_POTAR_TAB[5]->set_plage(0, 90);

	main_add_connection(delay, 0, 0, -1, 0, 0);
	main_add_connection(delay, 1, 0, -1, 0, 1);

	/*
	*	---------------------Main Loop-------------------------------
	*/
	while(1){
	
		/*
		*	Read potar values and store values in MAIN_POTAR_VALUES
		*/
		io_update_potar_tab(MAIN_POTAR_TAB, MAIN_POTAR_VALUES);
		
		/*
		*	Update each accesor for each module
		*/
		for(Module_Node_List::iterator itr = MAIN_LIST_MODULE.begin(); itr != MAIN_LIST_MODULE.end(); itr++){
			
			itr->second->accessor_update(MAIN_POTAR_VALUES);
		}
		
		/*
		*	Render screen
		*/
		MAIN_SCREEN->update();
		
		/*
		*	Pause main loop for 50ms ( 20Hz )
		*/
		usleep(50000);
	}
}

/*
*	---------------------------------------------------------------------------
*	Module Node Stuff
*	---------------------------------------------------------------------------
*/

Module_Node::Module_Node(Module *mod):_mod(mod){
	
}

Module_Node::~Module_Node(){
	
	delete this->_mod;
	
	for (IO_Accessor_List::iterator it = this->_io_accessor_list.begin() ; it != this->_io_accessor_list.end(); it++){
		delete (*it);
	} 
	this->_io_accessor_list.clear();
	this->_connection_list.clear();
}

Module* Module_Node::get_module() const{
	
	return this->_mod;
}

void Module_Node::accessor_add(IO_Accessor *accessor){
	
	this->_io_accessor_list.push_back(accessor);
}

void Module_Node::accessor_remove(int i){
	
	this->_io_accessor_list.erase(this->_io_accessor_list.begin() +i);
}

IO_Accessor* Module_Node::accessor_get(int i) const{
	
	if(i >= this->_io_accessor_list.size())return NULL;
	return this->_io_accessor_list.at(i);
}

void Module_Node::accessor_update(int *potar_tab){
	
	for (IO_Accessor_List::iterator it = this->_io_accessor_list.begin() ; it != this->_io_accessor_list.end(); it++){
		
		if( !!(*it)->update(potar_tab) && (*it)->is_dead() ) this->_io_accessor_list.erase(it);
	}
}

void Module_Node::connection_add(int i){
	
	this->_connection_list.push_back(i);
}

void Module_Node::connection_remove(int i){
	
	this->_connection_list.erase(this->_connection_list.begin() +i);
}

int Module_Node::connection_get(int i) const{
	
	if(i >= this->_connection_list.size())return 0;
	return this->_connection_list.at(i);
}

/*
*	---------------------------------------------------------------------------
*	Main Fuctions
*	---------------------------------------------------------------------------
*/
int main_add_module(MODULE_TYPE mod, int v){
	
	while(MAIN_LIST_MODULE.find(MAIN_COUNT_MODULE) != MAIN_LIST_MODULE.end()) MAIN_COUNT_MODULE++;
	
	fprintf (stderr, "Add module --- ");
	
	Module *newmod;
	switch(mod){
		case MDRIVE:
			newmod = new Drive(SERVER_NAME, v);
			fprintf (stderr, "new Drive\n");
			break;
		case MDELAY:
			newmod = new Delay(SERVER_NAME, v);
			fprintf (stderr, "new Delay\n");
			break;
		case MLFO:
			newmod = new LFO(SERVER_NAME, v);
			fprintf (stderr, "new LFO\n");
			break;
		case MRINGM:
			newmod = new Ringmod(SERVER_NAME, v);
			fprintf (stderr, "new Ringmod\n");
			break;
		case MTONE:
			newmod = new Tonestack(SERVER_NAME, v);
			fprintf (stderr, "new Tonestack\n");
			break;
		case MREV:
			newmod = new Reverb(SERVER_NAME, v);
			fprintf (stderr, "new Reverb\n");
			break;
		default:
			newmod = NULL;
			break;
	}
	
	if(newmod==NULL) return 1;
	
	MAIN_LIST_MODULE.insert(make_pair(MAIN_COUNT_MODULE, new Module_Node(newmod)));
	fprintf (stderr, "Added new module : %d\n", MAIN_COUNT_MODULE);
	
	return 0;
}

int main_del_module(int idx){
	
	if(MAIN_LIST_MODULE.find(idx) == MAIN_LIST_MODULE.end())
		return 1;
	
	MAIN_LIST_MODULE.erase(MAIN_LIST_MODULE.begin());
	return 0;
}

const char* get_source_name(Module *source, int v, int is, Module *target){
	
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
		
		if(source->get_voice(v)->get_port(PORT_AO, is) != NULL){
			from = jack_port_name(source->get_voice(v)->get_port(PORT_AO, is));
		}else{
			fprintf (stderr, "\n***Failed get source port***\n");
			return NULL;
		}
	}
	return from;
}
	
const char* get_target_name(Module *target, int v, int id, Module *source){
	
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
		
		if(target->get_voice(v)->get_port(PORT_AI, id) != NULL){
			to = jack_port_name(target->get_voice(v)->get_port(PORT_AI, id));
		}else{
			fprintf (stderr, "***Failed get destination port***\n");
			return NULL;
		}
	}
	return to;
}

int main_add_connection(int source_idx, int source_v, int is, int target_idx, int target_v, int id){
	
	if(MAIN_LIST_MODULE.find(source_idx) == MAIN_LIST_MODULE.end() && source_idx != -1)return 1;
	if(MAIN_LIST_MODULE.find(target_idx) == MAIN_LIST_MODULE.end() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:MAIN_LIST_MODULE[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:MAIN_LIST_MODULE[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, source_v, is, target);
	const char *target_port = get_target_name(target, target_v, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_connect (target->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
		MAIN_LIST_MODULE[target_idx]->connection_add(-1*(id));
	}else{
		if (jack_connect (source->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
		if(target_idx == -1)MAIN_LIST_MODULE[source_idx]->connection_add( -1 * (is*1000000 + id) );
		else				MAIN_LIST_MODULE[source_idx]->connection_add( is*1000000 + target_idx*1000 + id);
	}
	
	fprintf (stderr, "New connection made\n");
	
	return 0;
}

int main_del_connection(int source_idx, int source_v, int is, int target_idx, int target_v, int id){

	if(MAIN_LIST_MODULE.find(source_idx) == MAIN_LIST_MODULE.end() && source_idx != -1)return 1;
	if(MAIN_LIST_MODULE.find(target_idx) == MAIN_LIST_MODULE.end() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:MAIN_LIST_MODULE[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:MAIN_LIST_MODULE[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, source_v, is, target);
	const char *target_port = get_target_name(target, target_v, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_disconnect (target->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
		MAIN_LIST_MODULE[target_idx]->connection_remove(-1*(id));
	}else{
		if (jack_disconnect (source->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
		if(target_idx == -1)MAIN_LIST_MODULE[source_idx]->connection_remove( -1 * (is*1000000 + id) );
		else				MAIN_LIST_MODULE[source_idx]->connection_remove( is*1000000 + target_idx*1000 + id);
	}
	fprintf (stderr, "Connection removed\n");
	
	return 0;
}

int main_add_accessor(int target, int target_voice, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv){
	
	fprintf (stderr, "Add new accessor --- ");
	
	if(MAIN_LIST_MODULE.find(target) == MAIN_LIST_MODULE.end()){
		fprintf (stderr, "\nTarget not found\n");
	   	return 1;
	}
	
	if(MAIN_LIST_MODULE[target]->get_module()->get_voice(target_voice)->get_param_count() <= param_idx){
		fprintf (stderr, "\nParam target not found\n");
		return 1;
	}
	
	fprintf (stderr, "Idx : %d -- Target : %d.%d -- Pot : %d -- Min : %f -- Max : %f\n", 0, target, param_idx, potentiometer, min, max);
	
	MAIN_LIST_MODULE[target]->accessor_add( new IO_Accessor(MAIN_LIST_MODULE[target]->get_module(), MAIN_LIST_MODULE[target]->get_module()->get_voice(target_voice), param_idx, potentiometer, min, max, curve, is_db, is_inv));
	
	return 0;
}