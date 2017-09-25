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

void Module_Node::accessor_add(Accessor *accessor){
	
	this->_io_accessor_list.push_back(accessor);
}

void Module_Node::accessor_remove(int i){
	
	this->_io_accessor_list.erase(this->_io_accessor_list.begin() +i);
}

Accessor* Module_Node::accessor_get(int i) const{
	
	if(i >= (int)this->_io_accessor_list.size())return NULL;
	return this->_io_accessor_list.at(i);
}

void Module_Node::accessor_update(int *potar_tab){
	
    /*
	for (IO_Accessor_List::iterator it = this->_io_accessor_list.begin() ; it != this->_io_accessor_list.end(); it++){
		
		if( !!(*it)->update(potar_tab) && (*it)->is_dead() ) this->_io_accessor_list.erase(it);
	}
    */
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
	
	
	if(i >= (int)this->_connection_list.size())return Connection();
	return this->_connection_list.at(i);
}

Connection_List Module_Node::connection_get_list(){
	
	return this->_connection_list;
}

Module_Node_List::Module_Node_List():
    count_(0), 
    begin_(new EndModule(SERVER_NAME, MOD_FIRST)), 
    end_(new EndModule(SERVER_NAME, MOD_LAST)),
    mute_(false),
    outvl_(0),
    outvr_(0)
{

    const char **port;
    
    /*
    *   Connect begin module to capture ports
    */
    cout << "Connect begin module to capture ports -- ";
    port = jack_get_ports (begin_.get_module()->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    if (port == NULL) {
        
        cout << "no physical capture ports" << endl;
        exit (1);
    }
        
    if (jack_connect (begin_.get_module()->get_client(), port[0], jack_port_name( begin_.get_module()->get_port(AUDIO_I, 0))) ) {
        
        cout << "cannot connect input port Left" << endl;
        exit(1);
    }
    //cout << port[0] << " -> " << jack_port_name( begin_.get_module()->get_port(AUDIO_I, 0)) << " ok -- ";
    if (jack_connect (begin_.get_module()->get_client(), port[1], jack_port_name( begin_.get_module()->get_port(AUDIO_I, 1))) ) {
        
        cout << "cannot connect input port Right" << endl;
        exit(1);
    }
    //cout << port[1] << " -> " << jack_port_name( begin_.get_module()->get_port(AUDIO_I, 1)) << " ok -- ";
    free(port);
    cout << "Connection made" << endl;
    
    /*
    *   Connect end module to playback ports
    */
    cout << "Connect End module to Playback ports -- ";
    port = jack_get_ports (end_.get_module()->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsInput);
    if (port == NULL) {
        
        cout << "no physical Playback ports" << endl;
        exit (1);
    }
        
    if (jack_connect (end_.get_module()->get_client(), jack_port_name( end_.get_module()->get_port(AUDIO_O, 0)), port[0]) ) {
        
        cout << "cannot connect input port Left" << endl;
        exit(1);
    }
    //cout << port[0] << " -> " << jack_port_name( end_.get_module()->get_port(AUDIO_O, 0)) << " ok -- ";
    if (jack_connect (end_.get_module()->get_client(), jack_port_name( end_.get_module()->get_port(AUDIO_O, 1)), port[1]) ) {
        
        cout << "cannot connect input port Right" << endl;
        exit(1);
    }
    //cout << port[1] << " -> " << jack_port_name( end_.get_module()->get_port(AUDIO_O, 1)) << " ok -- ";
    free(port);
    cout << "Connection made" << endl;
}

Module_Node_List::~Module_Node_List(){
    
    for ( Module_iterator itr = list_.begin(); itr != list_.end(); itr++ ){
        
        delete *itr;
    }
}

int Module_Node_List::add_module(MODULE_TYPE mod){
	
	while(count_ < (int)list_.size()) count_++;
	cout << "Add module --- ";
	
	Module *newmod;
    try{
        
        switch(mod){
            case MOD_DRIVE:
            
                cout << "new Drive" << endl;
                newmod = new Drive(SERVER_NAME);
                break;
                
            case MOD_DELAY:
            
                cout << "new Delay" << endl;
                newmod = new Delay(SERVER_NAME);
                break;
                
            case MOD_LFO:
            
                cout << "new LFO" << endl;
                newmod = new LFO(SERVER_NAME);
                break;
                
            case MOD_RINGM:
            
                cout << "new Ringmod" << endl;
                newmod = new Ringmod(SERVER_NAME);
                break;
                
            case MOD_TONE:
            
                cout << "new Tonestack" << endl;
                newmod = new Tonestack(SERVER_NAME);
                break;
                
            case MOD_REV:
            
                cout << "new Reverb" << endl;
                newmod = new Reverb(SERVER_NAME);
                break;
                
            case MOD_CHORUS:
            
                cout << "new Chorus" << endl;
                newmod = new Chorus(SERVER_NAME);
                break;
                
            case MOD_COMP:
                
                cout << "new Compressor" << endl;
                newmod = new Compressor(SERVER_NAME);
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
	//cout << "Added new module : "<< count_ << endl;
	
	return 0;
}

int Module_Node_List::del_module(int idx){
	
	if(count_ >= (int)list_.size())
		return 1;
	
	list_.erase(list_.begin() + idx);
	return 0;
}

int Module_Node_List::add_connection(short source_idx, short is, short target_idx, short id){
	
	if(source_idx >= (int)list_.size() && source_idx != -1)return 1;
	if(target_idx >= (int)list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)? begin_.get_module():list_[source_idx]->get_module();
	Module *target = (target_idx == -1)? end_.get_module()  :list_[target_idx]->get_module();
	
	const char *source_port;
	const char *target_port;
    
    if ( source->get_port( AUDIO_O, is ) == NULL ){
        
        cout << "Source port is invalid" << endl;
        return 1;
    }
    source_port = jack_port_name( source->get_port( AUDIO_O, is ) );
    
    if ( target->get_port( AUDIO_I, id ) == NULL ){
        
        cout << "Target port is invalid" << endl;
        return 1;
    }
    target_port = jack_port_name( target->get_port( AUDIO_I, id ) );
    
    cout << "Connection from : " << source_port << " to : " << target_port << " -- ";
	
    if (jack_connect (source->get_client(), source_port, target_port)) {
        
        cout << "cannot connect input ports" << endl;
        return 1;
    }
    Connection c = { source_idx, is, target_idx, id};
    if( source_idx == -1 ){
        
        begin_.connection_add(c);
        
    }else if ( target_idx == -1){
        
        end_.connection_add(c);
        
    }else{
        
        list_[source_idx]->connection_add(c);
    }
	
	cout << "New connection made" << endl;

	return 0;
}

int Module_Node_List::del_connection(short source_idx, short is, short target_idx, short id){

	if(source_idx >= (int)list_.size() && source_idx != -1)return 1;
	if(target_idx >= (int)list_.size() && target_idx != -1)return 1;
	
	Module *source = (source_idx == -1)? begin_.get_module():list_[source_idx]->get_module();
	Module *target = (target_idx == -1)? end_.get_module()  :list_[target_idx]->get_module();
	
	const char *source_port;
	const char *target_port;
    
    if ( source->get_port( AUDIO_I, is ) == NULL ){
        
        cout << "Source port is invalid" << endl;
        return 1;
    }
    source_port = jack_port_name( source->get_port( AUDIO_O, is ) );
    
    if ( target->get_port( AUDIO_O, id ) == NULL ){
        
        cout << "Target port is invalid" << endl;
        return 1;
    }
    target_port = jack_port_name( target->get_port( AUDIO_I, id ) );
	
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
	
	if(target >= (int)list_.size()){
		 cout << "Target not found" << endl;
	   	return 1;
	}
	
	if(list_[target]->get_module()->get_param_count() <= param_idx){
		cout << "Param target not found" << endl;
		return 1;
	}
	
	cout << "Target : " << target << "." << param_idx << " -- Pot : " << potentiometer;
    cout << " -- Min : " << min << " -- Max : " << max << endl;
	
	//list_[target]->accessor_add( new Accessor(list_[target]->get_module(), target, param_idx, potentiometer, min, max, curve, is_db, is_inv));
	
	return 0;
}

void Module_Node_List::mute( bool m ){
    
    mute_ = m;
    if ( mute_ ){
        
        outvl_ = end_.get_module()->get_param( END_LEFT );
        outvr_ = end_.get_module()->get_param( END_RIGHT );
        
        end_.get_module()->set_param( END_LEFT , 0.0f );
        end_.get_module()->set_param( END_RIGHT, 0.0f );
    }
    else{
        
        end_.get_module()->set_param( END_LEFT , outvl_ );
        end_.get_module()->set_param( END_RIGHT, outvr_ );
    }
}

bool Module_Node_List::mute(){
    
    return mute_;
}

void Module_Node_List::next_bank(){
    
    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){
        
        (*itr)->get_module()->next_bank();
    }
}

void Module_Node_List::prev_bank(){
    
    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){
        
        (*itr)->get_module()->prev_bank();
    }
}

void Module_Node_List::set_out_volume( float vol ){
    
    outv_ = vol;
    
    end_.get_module()->set_param( END_LEFT , vol );
    end_.get_module()->set_param( END_RIGHT, vol );
}