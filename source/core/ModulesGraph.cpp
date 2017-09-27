#include "Modules.h"
#include "../modules/ModuleList.h"

namespace {
    const char *SERVER_NAME = "Space_Fx";
    int MIDX = 0;
}
/*
*	---------------------------------------------------------------------------
*	Module Node Stuff
*	---------------------------------------------------------------------------
*/
Module_Node::Module_Node(Module *mod):_mod(mod), id_( ++MIDX ){
	
}

Module_Node::Module_Node(Module* mod, int id): _mod(mod), id_(id){
    
}

Module_Node::~Module_Node(){
	
	delete this->_mod;
    
	this->_connection_list.clear();
}

Module* Module_Node::get_module() const{
	
	return this->_mod;
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

int Module_Node::get_id() const{
    
    return id_;
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
    //cout << "Connect begin module to capture ports -- ";
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
    //cout << "Connection made" << endl;
    
    /*
    *   Connect end module to playback ports
    */
    //cout << "Connect End module to Playback ports -- ";
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
    //cout << "Connection made" << endl;
}

Module_Node_List::~Module_Node_List(){
    
    for ( Module_iterator itr = list_.begin(); itr != list_.end(); itr++ ){
        
        delete *itr;
    }
}

int Module_Node_List::add_module(MODULE_TYPE mod){
    
    while(count_ < (int)list_.size()) count_++;
	cout << "Add module --- ";
	
	string txt = "";
	
	Module *newmod;
    try{
        
        switch(mod){
            case MOD_DRIVE:
            
                txt = "new Drive";
                newmod = new Drive(SERVER_NAME);
                break;
                
            case MOD_DELAY:
            
                txt = "new Delay";
                newmod = new Delay(SERVER_NAME);
                break;
                
            case MOD_LFO:
            
                txt = "new LFO";
                newmod = new LFO(SERVER_NAME);
                break;
                
            case MOD_RINGM:
            
                txt = "new Ringmod";
                newmod = new Ringmod(SERVER_NAME);
                break;
                
            case MOD_TONE:
            
                txt = "new Tonestack";
                newmod = new Tonestack(SERVER_NAME);
                break;
                
            case MOD_REV:
            
                txt = "new Reverb";
                newmod = new Reverb(SERVER_NAME);
                break;
                
            case MOD_CHORUS:
            
                txt = "new Chorus";
                newmod = new Chorus(SERVER_NAME);
                break;
                
            case MOD_COMP:
                
                txt = "new Compressor";
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
    
    txt.resize( 10, ' ' );
	
	list_.push_back(new Module_Node(newmod));
    
    cout << "Type : \"" << txt << "\" ID : \"" << list_[list_.size()-1]->get_id() << "\"     --OK" << endl;
	//cout << "Added new module : "<< count_ << endl;
	
	return 0;
}
int Module_Node_List::add_module(MODULE_TYPE mod, int id){
	
	while(count_ < (int)list_.size()) count_++;
	cout << "Add module --- ";
    string txt = "";
	
	Module *newmod;
    try{
        
        switch(mod){
            case MOD_DRIVE:
            
                txt = "Drive";
                newmod = new Drive(SERVER_NAME);
                break;
                
            case MOD_DELAY:
            
                txt = "Delay";
                newmod = new Delay(SERVER_NAME);
                break;
                
            case MOD_LFO:
            
                txt = "LFO";
                newmod = new LFO(SERVER_NAME);
                break;
                
            case MOD_RINGM:
            
                txt = "Ringmod";
                newmod = new Ringmod(SERVER_NAME);
                break;
                
            case MOD_TONE:
            
                txt = "Tonestack";
                newmod = new Tonestack(SERVER_NAME);
                break;
                
            case MOD_REV:
            
                txt = "Reverb";
                newmod = new Reverb(SERVER_NAME);
                break;
                
            case MOD_CHORUS:
            
                txt = "Chorus";
                newmod = new Chorus(SERVER_NAME);
                break;
                
            case MOD_COMP:
                
                txt = "Compressor";
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
    
    txt.resize( 15, ' ' );
    string i = to_string( id );
    i.resize( 3, ' ' );
    cout << "Type : " << txt << "  ID : " << i << " -- OK" << endl;
	
	list_.push_back(new Module_Node(newmod, id) );
	//cout << "Added new module : "<< count_ << endl;
	
	return 0;
}

int Module_Node_List::del_module(int idx){
	
	if(count_ >= (int)list_.size())
		return 1;
	
	list_.erase(list_.begin() + idx);
	return 0;
}

int Module_Node_List::add_connection(short source_id, short is, short target_id, short id){

	if(source_id < 0 && source_id != BEGIN_NODE)return 1;
	if(target_id < 0 && target_id != END_NODE  )return 1;
    
    if ( this->get(source_id) == NULL || this->get(target_id) == NULL ) return 1;
	
	Module *source = (source_id == BEGIN_NODE)? begin_.get_module():this->get(source_id)->get_module();
	Module *target = (target_id == END_NODE  )? end_.get_module()  :this->get(target_id)->get_module();
	
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
    
    string s = string(source_port);
    s.resize( 25, ' ' );
    string sd = to_string( source_id );
    sd.resize( 3, ' ' );
    
    string t = string(target_port);
    t.resize( 25, ' ' );
    string td = to_string( target_id );
    td.resize( 3, ' ' );
    
    cout << "Connection from : " << s << "( " << sd << " )   to : " << t << "( " << td << " )  -- ";
	
    if (jack_connect (source->get_client(), source_port, target_port)) {
        
        cout << "cannot connect input ports" << endl;
        return 1;
    }
    Connection c = { source_id, is, target_id, id};
    if( source_id == BEGIN_NODE ){
        
        begin_.connection_add(c);
        
    }else if ( target_id == END_NODE ){
        
        end_.connection_add(c);
        
    }else{
        
        this->get(source_id)->connection_add(c);
    }
	
	cout << "New connection made" << endl;

	return 0;
}

int Module_Node_List::del_connection(short source_id, short is, short target_id, short id){

	if(source_id >= (int)list_.size() && source_id != BEGIN_NODE)return 1;
	if(target_id >= (int)list_.size() && target_id != END_NODE)return 1;
    
    if ( this->get(source_id) == NULL || this->get(target_id) == NULL ) return 1;
	
	Module *source = (source_id == BEGIN_NODE)? begin_.get_module():this->get(source_id)->get_module();
	Module *target = (target_id == END_NODE  )? end_.get_module()  :this->get(target_id)->get_module();
	
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

Module_Node* Module_Node_List::get( int id ){
    
    if ( id == END_NODE ) return &end_;
    
    if ( id == BEGIN_NODE ) return &begin_;
    
    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){
        
        if ( (*itr)->get_id() == id ) return *itr;
    }
    
    return NULL;
}

void Module_Node_List::set_out_volume( float vol ){
    
    outv_ = vol;
    
    end_.get_module()->set_param( END_LEFT , vol );
    end_.get_module()->set_param( END_RIGHT, vol );
}