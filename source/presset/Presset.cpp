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

Module_Node_List::Module_Node_List():count_(0){
}

int Module_Node_List::add_module(MODULE_TYPE mod){
	
	while(count_ < list_.size()) count_++;
	
	cout << "Add module --- ";
	
	Module *newmod;
    try{
        
        switch(mod){
            case MOD_DRIVE:
            
                newmod = new Drive(SERVER_NAME);
                cout << "new Drive" << endl;
                break;
                
            case MOD_DELAY:
            
                newmod = new Delay(SERVER_NAME);
                cout << "new Delay" << endl;
                break;
                
            case MOD_LFO:
            
                newmod = new LFO(SERVER_NAME);
                cout << "new LFO" << endl;
                break;
                
            case MOD_RINGM:
            
                newmod = new Ringmod(SERVER_NAME);
                cout << "new Ringmod" << endl;
                break;
                
            case MOD_TONE:
            
                newmod = new Tonestack(SERVER_NAME);
                cout << "new Tonestack" << endl;
                break;
                
            case MOD_REV:
            
                newmod = new Reverb(SERVER_NAME);
                cout << "new Reverb" << endl;
                break;
                
            case MOD_CHORUS:
            
                newmod = new Chorus(SERVER_NAME);
                cout << "new Chorus" << endl;
                break;
                
            default:
            
                throw string ("Invalid module type");
                break;
        }
        
    } catch(string const& e) {
        
        cout << e << endl;
        return 1;
    }
	
	list_.push_back(new Module_Node(newmod));
	cout << "Added new module : "<< count_ << endl;
	
	return 0;
}

int Module_Node_List::del_module(int idx){
	
	if(count_ >= list_.size())
		return 1;
	
	list_.erase(list_.begin() + idx);
	return 0;
}

int Module_Node_List::add_connection(short source_idx, short is, short target_idx, short id){
	
	if(source_idx >= list_.size() && source_idx != -1)return 1;
	if(target_idx >= list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:list_[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:list_[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, is, target);
	const char *target_port = get_target_name(target, id, source);
    
    cout << "Connection from : " << source_port << " to : " << target_port << endl;
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_connect (target->get_client(), source_port, target_port)) {
			cout << "cannot connect input ports" << endl;
			return 1;
		}
		Connection c = { source_idx, is, target_idx, id};
		list_[target_idx]->connection_add(c);
	}else{
		if (jack_connect (source->get_client(), source_port, target_port)) {
			cout << "cannot connect input ports" << endl;
			return 1;
		}	
		Connection c = { source_idx, is, target_idx, id};
		list_[source_idx]->connection_add(c);
	}
	
	cout << "New connection made" << endl;

	return 0;
}

int Module_Node_List::del_connection(short source_idx, short is, short target_idx, short id){

	if(source_idx >= list_.size() && source_idx != -1)return 1;
	if(target_idx >= list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)?NULL:list_[source_idx]->get_module();
	Module *target = (target_idx == -1)?NULL:list_[target_idx]->get_module();
	
	const char *source_port = get_source_name(source, is, target);
	const char *target_port = get_target_name(target, id, source);
	
	if(source_port == NULL && target_port == NULL)
		return 1;
	
	if(source == NULL){
		if (jack_disconnect (target->get_client(), source_port, target_port)) {
			cout << "cannot disconnect input ports" << endl;
			return 1;
		}
	}else{
		if (jack_disconnect (source->get_client(), source_port, target_port)) {
			cout << "cannot disconnect input ports" << endl;
			return 1;
		}
	}
	cout << "Connection removed" << endl;
	
	return 0;
}

int Module_Node_List::add_accessor(int target, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv){
	
	cout << "Add new accessor --- ";
	
	if(target >= list_.size()){
		 cout << "Target not found" << endl;
	   	return 1;
	}
	
	if(list_[target]->get_module()->get_param_count() <= param_idx){
		cout << "Param target not found" << endl;
		return 1;
	}
	
	cout << "Target : " << target << "." << param_idx << " -- Pot : " << potentiometer;
    cout << " -- Min : " << min << " -- Max : " << max << endl;
	
	list_[target]->accessor_add( new IO_Accessor(list_[target]->get_module(), target, param_idx, potentiometer, min, max, curve, is_db, is_inv));
	
	return 0;
}

const char* get_source_name(Module *source, int is, Module *target){
	
	const char *from;
	if(source == NULL){
		//fprintf (stderr, "Capture source --- ");
		
		const char **port;
		port = jack_get_ports (target->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
		if (port == NULL) {
			fprintf(stderr, "no physical capture ports\n");
			exit (1);
		}
		from = port[(is==0)?0:1];
		free(port);
	}else{
		//fprintf (stderr, "Module source --- ");
		
		if(source->get_port(AUDIO_O, is) != NULL){
			from = jack_port_name(source->get_port(AUDIO_O, is));
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
		//fprintf (stderr, "Playback destination\n");
		
		const char **port;
		port = jack_get_ports (source->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsInput);
		if (port == NULL) {
			fprintf(stderr, "no physical playback ports\n");
			exit (1);
		}
		to = port[(id==0)?0:1];
		free(port);
	}else{
		//fprintf (stderr, "Module destination\n");
		
		if(target->get_port(AUDIO_I, id) != NULL){
			to = jack_port_name(target->get_port(AUDIO_I, id));
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
		
		cout << "Save presset file : " << filename;
		int i = 0;
		flux << NODE_ROOT << " " << version << " " << name << " " << list.list_.size() << endl;
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			int param_count = node->get_module()->get_param_count();
			
			flux << NODE_MODULE << " " << node->get_module()->get_type() << " " << param_count << endl;
				
            flux << "    " << NODE_PARAM << " " << param_count ;
            for(int n = 0; n < param_count; n++) flux << " " << node->get_module()->get_param(n);
            flux << endl;
			
			for(IO_Accessor_List::iterator atr = node->accessor_get_list()->begin(); atr != node->accessor_get_list()->end(); atr++){
				
				IO_Accessor *a = *atr;
				flux << "    " << NODE_ACCESSOR << " " << i << " " << a->get_target_param() << " " << a->get_potar();
				flux << " " << a->get_min() << " " << a->get_max() << " " << a->get_curve() << " " << a->get_db() << " " << a->get_inv() << endl;
			}
			
			i++;
		}
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			for(Connection_List::iterator ctr = node->connection_get_list()->begin(); ctr != node->connection_get_list()->end(); ctr++){
				
				Connection c = (*ctr);
				
				flux << NODE_CONNECTION << " " << c.s << " " << c.sp << " " << c.t << " " << c.tp << endl;
			}
		}
	}else{
		
		flux.close();
		cout << "Error can't open file " << filename << endl;
		return 1;
	}
	flux.close();
	cout << " -- Presset successfully saved" << endl;
	return 0;
}

int load_presset(string name, string version, Module_Node_List *list){
	
	string filename = "./Files/" + name + ".txt";
	ifstream flux(filename.c_str());
    
    cout << "Load presset : " << filename << endl;
	
	if(flux){
		
		Module_Node_List newlist;
		
		string current;
		flux >> current;
		//cout << current << " ";
		
		if(!!current.compare(NODE_ROOT)){
			cout << "File format not mach, First ndoe must be a NODE_ROOT -- " << current << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		//cout << current << " ";
		if(!!current.compare(version)){
			cout << "Bad version -- File : " << current << " -- Current : " << version << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		//cout << current;
		current.clear();
		
		int module_count = 0;
		int current_module = -1;
		flux >> module_count;
		//cout << " " << module_count << endl;
		
		//cout << "Document start" << endl;
		current.clear();
		while(flux >> current){
			
			if(!current.compare(NODE_MODULE)){
				
				
				int type, param_count;
				flux >> type;
				flux >> param_count;
				
				//cout << current << " " << type << " " << param_count << endl;
				
				if(newlist.add_module(static_cast<MODULE_TYPE>(type))){
					cout << "Failed load Module -- Type : " << type << endl;
					flux.close();
					return 1;
				}
				++current_module;
				
			}else if(!current.compare(NODE_PARAM) && current_module != -1){
				
				int param_count;
				flux >> param_count;
				
				//cout << current << " " << param_count ;
				
				float *params = new float[param_count];
				for(int i = 0; i < param_count; i++){
					flux >> *(params+i);
					//cout << " " << params[i];
				}
				//cout << endl;
				
				newlist.list_[current_module]->get_module()->set_param(param_count, params);
				
			}else if(!current.compare(NODE_ACCESSOR) && current_module != -1){
				
				int ti, pi, p, c, id, iv;
				float min, max;
				flux >> ti;
				flux >> pi;
				flux >> p;
				flux >> min;
				flux >> max;
				flux >> c;
				flux >> id;
				flux >> iv;
				
				//cout << current << " " << ti << " " << pi << " " << p << " " << c << " " << id << " " << iv << endl;
				
				newlist.add_accessor(ti, pi, p, min, max, static_cast<IO_CURVE>(c), id, iv);
				
			}else if(!current.compare(NODE_CONNECTION) && current_module != -1){
				
				short si, sp, ti, tp;
				flux >> si;
				flux >> sp;
				flux >> ti;
				flux >> tp;
				
				//cout << current << " " << si << " " << sp << " " << ti << " " << tp <<endl;
				
				newlist.add_connection(si, sp, ti, tp);
			}
			current.clear();
		}
		
		//cout << "document end" << endl;
		
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