#include "Presset.h"
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

/*
*	---------------------------------------------------------------------------
*	Presset stuff
*	---------------------------------------------------------------------------
*/
int save_presset(string name, string version, Module_Node_List list){
	
	string filename = "./Files/" + name + ".txt";
	ofstream flux(filename.c_str());
	
	if(flux){
		
		int i = 0;
		flux << node_root << " " << version << " " << name << " " << list.size() << endl;
		
		for(Module_Node_List::iterator itr = list.begin(); itr != list.end(); itr++){
			
			Module_Node* node = *itr;
			int voice_count = node->get_module()->get_voice_count();
			int param_count = node->get_module()->get_voice(0)->get_param_count();
			
			flux << node_module << " " << node->get_module()->get_type() << " " << voice_count << " " << param_count << endl;
			for(int i = 0; i < voice_count; i++){
				
				flux << "    " << node_param << " " << param_count << " [";
				for(int k = 0; k < param_count; k++){
					flux << " " << node->get_module()->get_voice(i)->get_param(k);
				}
				
				flux << " ]" << endl;
			}
			
			for(IO_Accessor_List::iterator atr = node->accessor_get_list()->begin(); atr != node->accessor_get_list()->end(); atr++){
				
				IO_Accessor *a = *atr;
				flux << "    " << node_accessor << " " << i << " " << a->get_target_voice() << " " << a->get_target_param() << " " << a->get_potar();
				flux << " " << a->get_min() << " " << a->get_max() << " " << a->get_curve() << " " << a->get_db() << " " << a->get_inv() << endl;
			}
			
			i++;
		}
		
		for(Module_Node_List::iterator itr = list.begin(); itr != list.end(); itr++){
			
			Module_Node* node = *itr;
			for(Connection_List::iterator ctr = node->connection_get_list()->begin(); ctr != node->connection_get_list()->end(); ctr++){
				
				Connection c = (*ctr);
				
				flux << node_connection << " " << c.s << " " << c.sv << " " << c.sp << " " << c.t << " " << c.tv << " " << c.tp << endl;
			}
		}
	}else{
		
		cout << "Error can't open file " << filename << endl;
		return 1;
	}
	return 0;
}