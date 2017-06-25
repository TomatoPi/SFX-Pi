#include "Presset.h"

const char *SERVER_NAME = "Space_Fx";
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

IO_Accessor_List* Module_Node::accessor_get_list(){
	
	return &this->_io_accessor_list;
}

void Module_Node::connection_add(Connection c){
	
	Connection a = Connection(c);
	this->_connection_list.push_back(a);
}

void Module_Node::connection_remove(int i){
	
	this->_connection_list.erase(this->_connection_list.begin() +i);
}

Connection Module_Node::connection_get(int i) const{
	
	
	if(i >= this->_connection_list.size())return Connection();
	return this->_connection_list.at(i);
}

Connection_List* Module_Node::connection_get_list(){
	
	return &this->_connection_list;
}


int Module_Node_List::add_module(MODULE_TYPE mod, int v){
	
	while(count_ < list_.size()) count_++;
	
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
		case MCHORUS:
			newmod = new Chorus(SERVER_NAME, v);
			fprintf (stderr, "new Chorus\n");
			break;
		default:
			newmod = NULL;
			break;
	}
	
	if(newmod==NULL) return 1;
	
	list_.push_back(new Module_Node(newmod));
	fprintf (stderr, "Added new module : %d\n", count_);
	
	return 0;
}

int Module_Node_List::del_module(int idx){
	
	if(count_ >= list_.size())
		return 1;
	
	list_.erase(list_.begin() + idx);
	return 0;
}

int Module_Node_List::add_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id){
	
	if(source_idx >= list_.size() && source_idx != -1)return 1;
	if(target_idx >= list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:list_[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:list_[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, source_v, is, target);
	const char *target_port = get_target_name(target, target_v, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_connect (target->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}
		Connection c = { source_idx, source_v, is, target_idx, target_v, id};
		list_[target_idx]->connection_add(c);
	}else{
		if (jack_connect (source->get_client(), source_port, target_port)) {
			fprintf (stderr, "cannot connect input ports\n");
			return 1;
		}	
		Connection c = { source_idx, source_v, is, target_idx, target_v, id};
		list_[source_idx]->connection_add(c);
	}
	
	fprintf (stderr, "New connection made\n");

	return 0;
}

int Module_Node_List::del_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id){

	if(source_idx >= list_.size() && source_idx != -1)return 1;
	if(target_idx >= list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:list_[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:list_[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, source_v, is, target);
	const char *target_port = get_target_name(target, target_v, id, source);
	
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

int Module_Node_List::add_accessor(int target, int target_voice, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv){
	
	fprintf (stderr, "Add new accessor --- ");
	
	if(target >= list_.size()){
		fprintf (stderr, "\nTarget not found\n");
	   	return 1;
	}
	
	if(list_[target]->get_module()->get_voice(target_voice)->get_param_count() <= param_idx){
		fprintf (stderr, "\nParam target not found\n");
		return 1;
	}
	
	fprintf (stderr, "Idx : %d -- Target : %d.%d -- Pot : %d -- Min : %f -- Max : %f\n", 0, target, param_idx, potentiometer, min, max);
	
	list_[target]->accessor_add( new IO_Accessor(list_[target]->get_module(), list_[target]->get_module()->get_voice(target_voice), target, target_voice, param_idx, potentiometer, min, max, curve, is_db, is_inv));
	
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
/*
*	---------------------------------------------------------------------------
*	Presset stuff
*	---------------------------------------------------------------------------
*/
int save_presset(string name, string version, Module_Node_List list){
	
	string filename = "./Files/" + name + ".txt";
	ofstream flux(filename.c_str());
	
	if(flux){
		
		cout << "Save file -- " << filename << endl;
		int i = 0;
		flux << node_root << " " << version << " " << name << " " << list.list_.size() << endl;
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			int voice_count = node->get_module()->get_voice_count();
			int param_count = node->get_module()->get_voice(0)->get_param_count();
			
			flux << node_module << " " << node->get_module()->get_type() << " " << voice_count << " " << param_count << endl;
			for(int k = 0; k < voice_count; k++){
				
				flux << "    " << node_param << " " << k << " " << param_count ;
				for(int n = 0; n < param_count; n++) flux << " " << node->get_module()->get_voice(k)->get_param(n);
				flux << endl;
			}
			
			for(IO_Accessor_List::iterator atr = node->accessor_get_list()->begin(); atr != node->accessor_get_list()->end(); atr++){
				
				IO_Accessor *a = *atr;
				flux << "    " << node_accessor << " " << i << " " << a->get_target_voice() << " " << a->get_target_param() << " " << a->get_potar();
				flux << " " << a->get_min() << " " << a->get_max() << " " << a->get_curve() << " " << a->get_db() << " " << a->get_inv() << endl;
			}
			
			i++;
		}
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			for(Connection_List::iterator ctr = node->connection_get_list()->begin(); ctr != node->connection_get_list()->end(); ctr++){
				
				Connection c = (*ctr);
				
				flux << node_connection << " " << c.s << " " << c.sv << " " << c.sp << " " << c.t << " " << c.tv << " " << c.tp << endl;
			}
		}
	}else{
		
		flux.close();
		cout << "Error can't open file " << filename << endl;
		return 1;
	}
	flux.close();
	cout << "Save ok !" << endl;
	return 0;
}

int load_presset(string name, string version, Module_Node_List *list){
	
	string filename = "./Files/" + name + ".txt";
	ifstream flux(filename.c_str());
	
	if(flux){
		
		Module_Node_List newlist;
		
		string current;
		flux >> current;
		cout << current << " ";
		
		if(!!current.compare(node_root)){
			cout << "File format not mach, First ndoe must be a node_root -- " << current << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		cout << current << " ";
		if(!!current.compare(version)){
			cout << "Bad version -- File : " << current << " -- Current : " << version << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		cout << current;
		current.clear();
		
		int module_count = 0;
		int current_module = -1;
		flux >> module_count;
		cout << module_count << endl;
		
		cout << "Document start" << endl;
		current.clear();
		while(flux >> current){
			
			cout << "New word " << current << endl;
			if(!current.compare(node_module)){
				
				
				int type, voice_count, param_count;
				flux >> type;
				flux >> voice_count;
				flux >> param_count;
				
				cout << current << "-" << type << "-" << voice_count << "-" << param_count << endl;
				
				if(newlist.add_module(static_cast<MODULE_TYPE>(type), voice_count)){
					cout << "Failed load Module -- Type : " << type << " -- Voice count : " << voice_count << endl;
					flux.close();
					return 1;
				}
				++current_module;
				
			}else if(!current.compare(node_param) && current_module != -1){
				
				int voice_idx, param_count;
				flux >> voice_idx;
				flux >> param_count;
				
				cout << current << "-" << voice_idx << "-" << param_count ;
				
				float *params = new float[param_count];
				for(int i = 0; i < param_count; i++){
					flux >> *(params+i);
					cout << "-" << params[i];
				}
				cout << endl;
				
				newlist.list_[current_module]->get_module()->get_voice(voice_idx)->set_param_list(param_count, params);
				
			}else if(!current.compare(node_accessor) && current_module != -1){
				
				int ti, tv, pi, p, c, id, iv;
				float min, max;
				flux >> ti;
				flux >> tv;
				flux >> pi;
				flux >> p;
				flux >> min;
				flux >> max;
				flux >> c;
				flux >> id;
				flux >> iv;
				
				cout << current << "-" << ti << "-" << tv << "-" << pi << "-" << p << "-" << c << "-" << id << "-" << iv << endl;
				
				newlist.add_accessor(ti, tv, pi, p, min, max, static_cast<IO_CURVE>(c), id, iv);
				
			}else if(!current.compare(node_connection) && current_module != -1){
				
				short si, sv, sp, ti, tv, tp;
				flux >> si;
				flux >> sv;
				flux >> sp;
				flux >> ti;
				flux >> tv;
				flux >> tp;
				
				cout << current << "-" << si << "-" << sv << "-" << sp << "-" << ti << "-" << tv << "-" << tp <<endl;
				
				newlist.add_connection(si, sv, sp, ti, tv, tp);
			}
			current.clear();
		}
		
		cout << "document end" << endl;
		
		for(Module_List::iterator itr = list->list_.begin(); itr != list->list_.end(); itr++) delete *itr;
		list->list_.clear();
		
		*list = newlist;
		
	}else{
		
		cout << "Error can't open file " << filename << endl;
		flux.close();
		return 1;
	}
	flux.close();
	cout << "Presset loaded successfull" << endl;
	return 0;
}