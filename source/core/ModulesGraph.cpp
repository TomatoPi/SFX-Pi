#include "ModulesGraph.h"

using namespace std;
using namespace PROG_CONST;

namespace {
    int MIDX = 0;
}

/*
*   ---------------------------------------------------------------------------
*   Module Node Stuff
*   ---------------------------------------------------------------------------
*/
Module_Node_List::Module_Node_List():
    count_(0),
    mute_(false),
    outvl_(0),
    outvr_(0),
    bank_(0)
{

    cout << "Create Module Graph Structure --- ";
    begin_ = new EndModule( MN_BEGIN, TC_BEGIN );
    end_ = new EndModule( MN_LAST, TC_LAST );

    const char **port;

    /*
    *   Connect begin module to capture ports
    */
    //cout << "Connect begin module to capture ports -- ";
    port = jack_get_ports (begin_->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    if (port == NULL) {

        cout << "Error : No Physical Capture Ports" << endl;
        exit (1);
    }

    if (jack_connect (begin_->get_client(), port[0], jack_port_name( begin_->get_port(AUDIO_I, 0))) ) {

        cout << "cannot connect input port Left" << endl;
        exit(1);
    }
    //cout << port[0] << " -> " << jack_port_name( begin_->get_port(AUDIO_I, 0)) << " ok -- ";
    if (jack_connect (begin_->get_client(), port[1], jack_port_name( begin_->get_port(AUDIO_I, 1))) ) {

        cout << "cannot connect input port Right" << endl;
        exit(1);
    }
    //cout << port[1] << " -> " << jack_port_name( begin_->get_port(AUDIO_I, 1)) << " ok -- ";
    free(port);
    //cout << "Connection made" << endl;

    /*
    *   Connect end module to playback ports
    */
    //cout << "Connect End module to Playback ports -- ";
    port = jack_get_ports (end_->get_client(), NULL, NULL, JackPortIsPhysical|JackPortIsInput);
    if (port == NULL) {

        cout << "no physical Playback ports" << endl;
        exit (1);
    }

    if (jack_connect (end_->get_client(), jack_port_name( end_->get_port(AUDIO_O, 0)), port[0]) ) {

        cout << "cannot connect output port Left" << endl;
        exit(1);
    }
    //cout << port[0] << " -> " << jack_port_name( end_->get_port(AUDIO_O, 0)) << " ok -- ";
    if (jack_connect (end_->get_client(), jack_port_name( end_->get_port(AUDIO_O, 1)), port[1]) ) {

        cout << "cannot connect output port Right" << endl;
        exit(1);
    }
    //cout << port[1] << " -> " << jack_port_name( end_->get_port(AUDIO_O, 1)) << " ok -- ";
    free(port);
    cout << "Graph OK" << endl;
}

Module_Node_List::~Module_Node_List(){

    this->clear_graph();

    delete begin_;
    delete end_;

}

int Module_Node_List::add_module( int type, int id){

    while(count_ < (int)list_.size()) count_++;
    cout << "Add module --- ";
    string txt = "";

    Module *newmod;
    try{

        newmod = ModuleFactory::create_module( type, id );

        if ( newmod != NULL ){

            txt = ModuleFactory::get_name( type );
        }
        else{

            throw ("Invalid Module Type" );
        }

    } catch(string const& e) {

        cout << e << endl;
        return 1;
    }

    txt.resize( 15, ' ' );
    string i = to_string( id );
    i.resize( 3, ' ' );
    cout << "Type : " << txt << "  ID : " << i << " -- OK" << endl;

    list_.push_back( newmod );

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

    Module *source = (source_id == BEGIN_NODE)? begin_:this->get(source_id);
    Module *target = (target_id == END_NODE  )? end_  :this->get(target_id);

    const char *source_port;
    const char *target_port;

    if ( source->get_port( AUDIO_O, is ) == NULL ){

        cout << "Error : Source port is invalid" << endl;
        return 1;
    }
    source_port = jack_port_name( source->get_port( AUDIO_O, is ) );

    if ( target->get_port( AUDIO_I, id ) == NULL ){

        cout << "Error : Target port is invalid" << endl;
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

        cout << "Error : cannot connect input ports" << endl;
        return 1;
    }
    Connection c = { source_id, is, target_id, id};
    this->connection_add( c );

    cout << "New connection made" << endl;

    return 0;
}

int Module_Node_List::add_connection( Connection c ){

    return this->add_connection( c.s, c.sp, c.t, c.tp );
}

int Module_Node_List::del_connection(short source_id, short is, short target_id, short id){

    if(source_id >= (int)list_.size() && source_id != BEGIN_NODE)return 1;
    if(target_id >= (int)list_.size() && target_id != END_NODE)return 1;

    if ( this->get(source_id) == NULL || this->get(target_id) == NULL ) return 1;

    Module *source = (source_id == BEGIN_NODE)? begin_:this->get(source_id);
    Module *target = (target_id == END_NODE  )? end_  :this->get(target_id);

    const char *source_port;
    const char *target_port;

    if ( source->get_port( AUDIO_I, is ) == NULL ){

        cout << "Error : Source port is invalid" << endl;
        return 1;
    }
    source_port = jack_port_name( source->get_port( AUDIO_O, is ) );

    if ( target->get_port( AUDIO_O, id ) == NULL ){

        cout << "Error : Target port is invalid" << endl;
        return 1;
    }
    target_port = jack_port_name( target->get_port( AUDIO_I, id ) );

    if(source_port == NULL && target_port == NULL)
        return 1;

    if(source == NULL){
        if (jack_disconnect (target->get_client(), source_port, target_port)) {
            cout << "Error : cannot disconnect input ports" << endl;
            return 1;
        }
    }else{
        if (jack_disconnect (source->get_client(), source_port, target_port)) {
            cout << "Error : cannot disconnect input ports" << endl;
            return 1;
        }
    }
    cout << "Connection removed" << endl;

    return 0;
}

void Module_Node_List::mute( bool m ){

    mute_ = m;
    if ( mute_ ){

        outvl_ = end_->get_param( END_LEFT );
        outvr_ = end_->get_param( END_RIGHT );

        end_->set_param( END_LEFT , 0.0f );
        end_->set_param( END_RIGHT, 0.0f );
    }
    else{

        end_->set_param( END_LEFT , outvl_ );
        end_->set_param( END_RIGHT, outvr_ );
    }
}

bool Module_Node_List::mute(){

    return mute_;
}

void Module_Node_List::next_bank(){

    bool once = true;

    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){

        (*itr)->next_bank();
        if( once ){
            bank_ = (*itr)->get_bank();
            once = false;
        }
    }
}

void Module_Node_List::prev_bank(){

    bool once = true;

    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){

        (*itr)->prev_bank();
        if( once ){
            bank_ = (*itr)->get_bank();
            once = false;
        }
    }
}

int Module_Node_List::get_bank(){

    return bank_;
}

Module* Module_Node_List::get( int id ){

    if ( id == END_NODE ) return end_;

    if ( id == BEGIN_NODE ) return begin_;

    for ( Module_iterator itr = list_.begin() ; itr != list_.end(); itr++ ){

        if ( (*itr)->get_id() == id ) return *itr;
    }
    cout << "Error : Module Not Founded : (" << id << ")" << endl;
    return NULL;
}

void Module_Node_List::set_out_volume( float vol ){

    outv_ = vol;

    end_->set_param( END_LEFT , vol );
    end_->set_param( END_RIGHT, vol );
}
void Module_Node_List::connection_add(Connection c){

    Connection a = Connection(c);
    this->connection_list_.push_back(a);
}

void Module_Node_List::connection_remove(int i){

    this->connection_list_.erase(this->connection_list_.begin() +i);
}

Connection Module_Node_List::connection_get(int i) const{


    if(i >= (int)this->connection_list_.size())return Connection();
    return this->connection_list_.at(i);
}

Connection_List Module_Node_List::connection_get_list(){

    return this->connection_list_;
}

void Module_Node_List::clear_graph(){

    for ( Module_iterator itr = list_.begin(); itr != list_.end(); itr++ ){

        delete *itr;
    }
    list_.clear();
    connection_list_.clear();
}
