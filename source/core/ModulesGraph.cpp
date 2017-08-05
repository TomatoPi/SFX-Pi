#include "Modules.h"
#include "../modules/ModuleList.h"

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