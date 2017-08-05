#include "Modules.h"
/*
*	---------------------------------------------------------------------------
*	Misc stuff
*	---------------------------------------------------------------------------
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->process(nframes, u);
}

int mod_Bypass_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Module*>(u)->bypass(nframes, u);
}

string modtype_to_string(MODULE_TYPE type){

	switch(type){
		
		case MOD_DRIVE:
			return string("Drive");
		case MOD_DELAY:
			return string("Delay");
		case MOD_LFO:
			return string("LFO");
		case MOD_RINGM:
			return string("RingMod");
		case MOD_TONE:
			return string("Tonestack");
		case MOD_REV:
			return string("Reverb");
		case MOD_CHORUS:
			return string("Chorus");
		default:
			return string("");
	}
}

Module::Module(const char *server, MODULE_TYPE type, int pc, int ai, int ao, int mi, int mo, ...):
    type_(type),
    is_bypassed_(false),
    ai_(ai),
    ao_(ao),
    mi_(mi),
    mo_(mo),
    param_c_(pc),
    bank_idx_(0)
{
    
    /*
    * Setup JACK client -------------------------------------------------------
    */
    //cout << "Create a new module -- setup jack client" << endl;
    jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	char* name = (char*)modtype_to_string(type).c_str();
	
	// Creating jack client with name "name", in server "server"
	client = jack_client_open (name, options, &status, server);
	if (client == NULL) {
        
		throw string("Unable to connect JACK server");
	}
	
	// Upgrade name if given name is not unique
	if (status & JackNameNotUnique) {
        
		name = jack_get_client_name(client);
		cout << "Unique name " << name << " assigned" << endl;
	}
	
    //cout << "Client is ok! -- ";
	this->client_ = client;
	this->name_ = jack_get_client_name(client);
	
	// Register callback function
    //cout << "Register callback -- ";
	jack_set_process_callback(client, mod_Process_Callback, this);
    
    /*
    *   Register ports  -------------------------------------------------------
    */
    // Alloc arrays
    //cout << "Alloc ports : ";
    audio_in_  = new jack_port_t*[ai];
    audio_out_ = new jack_port_t*[ao];
    midi_in_   = new jack_port_t*[mi];
    midi_out_  = new jack_port_t*[mo];
    //cout << ai << " " << ao << " " << mi << " " << mo << " -- " << endl;
    
    // Register ports
    int count = ai+ao+mi+mo;
    va_list arg;
    
    if (count > 0) {
        
        va_start(arg, mo);
        for (int i = 0; i < count ; i++) {
            
            MODULE_PORT_TYPE t = MIDI_O;
            if (i < ai + ao + mi) {
                
                t = MIDI_I;
            }
            if (i < ai + ao) {
                
                t = AUDIO_O;
            }
            if (i < ai) {
                
                t = AUDIO_I;
            }
            
            const char* n = va_arg(arg, char*);
            
            switch (t) {
                
				case AUDIO_I :
                    //cout << "register audio in : " << n << " idx " << i << endl;
					audio_in_[i]        = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
					break;
				case AUDIO_O :
                    //cout << "register audio out : " << n << " idx " << i-ai << endl;
					audio_out_[i-ai]    = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
					break;
				case MIDI_I :
                    //cout << "register midi in : " << n << " idx " << i-ai-ao << endl;
					midi_in_[i-ai-ao]   = jack_port_register( client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
					break;
				case MIDI_O :
                    //cout << "register midi out : " << n << " idx " << i-ai-ao-mi << endl;
					midi_out_[i-ai-ao-mi] = jack_port_register( client, n, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
					break;
			}
        }
    }
    
    //cout << "Port registration ended" << endl;
    va_end(arg);
    
    /*
    *   Alloc params array ----------------------------------------------------
    */
    //cout << "Alloc param array" << endl;
    param_ = new float[pc];
}

Module::~Module(){
    
    delete param_;
    
    delete audio_in_;
    delete audio_out_;
    delete midi_in_;
    delete midi_out_;
    
    for(ModBank::iterator itr = bank_.begin(); itr != bank_.end(); itr++){
    
        delete *(itr);
    }
    bank_.clear();
    
    jack_client_close(client_);
}

void Module::set_bypass(bool state){
    
    is_bypassed_ = state;
    if (is_bypassed_) {
        
        jack_set_process_callback(client_, mod_Bypass_Callback, this);
    }else{
        jack_set_process_callback(client_, mod_Process_Callback, this);
    }
}

bool Module::get_bypass() const{
    
    return is_bypassed_;
}

jack_client_t* 	Module::get_client() const{
    
    return client_;
}

MODULE_TYPE Module::get_type(){
    
    return type_;
}

char* Module::get_name(){
    
    return name_;
}

void Module::set_param(int idx, float value){
    
    if (idx < param_c_){
        
        param_[idx] = value;
        
        if(bank_.size() != 0){
            bank_.at(bank_idx_)[idx] = value;
        }
        
        this->change_param(idx, value);
    }
}

void Module::set_param(int count, const float *values){
    
    if (count == param_c_){
        
        memcpy(param_, values, sizeof(float) * param_c_);
        
        if(bank_.size() != 0){
            memcpy(bank_.at(bank_idx_), values, sizeof(float) * param_c_);
        }
        
        this->change_param(values);
    }
}

int Module::get_param_count() const{
    
    return param_c_;
}

float Module::get_param(int idx) const{
    
    if (idx < param_c_){
        return param_[idx];
    }
    return 0;
}

void Module::add_bank(int size, const float *bank){
    
    if(size == param_c_){
        
        float *newbank = new float[param_c_];
        memcpy(newbank, bank, sizeof(float) * param_c_);
        
        if(bank_.size() == 0){
            
            bank_.push_back(newbank);
            this->set_bank(0);
        }else{
            
            bank_.push_back(newbank);
        }
    }
}

void Module::add_bank(){
    
    this->new_bank();
}

void Module::remove_bank(){
    
    this->remove_bank(bank_idx_);
}

void Module::remove_bank(int idx){
    
    if(idx > 0 && idx < bank_.size()){
        
        delete bank_.at(idx);
        bank_.erase(bank_.begin() + idx);
        
        if(bank_idx_ >= bank_.size()) bank_idx_ = 0;
        if(bank_.size() == 0) bank_idx_ = -1;
    }
}

bool Module::next_bank(){
    
    bool cycle = false;
    
    if(bank_.size() != 0){
        
        this->set_bank(++bank_idx_%bank_.size());
        cycle = bank_idx_ == 0;
    }
    
    return cycle;
}

bool Module::prev_bank(){
    
    bool cycle = false;
    
    if(bank_.size() > 0){
        
        this->set_bank((--bank_idx_ < 0)? bank_idx_ = bank_.size() -1 : bank_idx_);
        cycle = bank_idx_ == bank_.size() - 1;
    }
    
    return cycle;
}

bool Module::set_bank(int idx){
    
    if(idx < bank_.size()){
        
        bank_idx_ = idx;        
        this->set_param(param_c_, bank_.at(idx));
        
        return true;
    }
    return false;
}

int Module::get_bank(){
    
    return bank_idx_;
}

string Module::get_param_name(int idx){
    
    if (idx < param_c_){
        
        return this->return_param_name(idx);
    }else{
        return string("NONE");
    }
}

string Module::get_formated_param(int idx){
    
    if (idx < param_c_){
        
        return this->return_formated_param(idx);
    }else{
        return string("NONE");
    }
}

jack_port_t* Module::get_port(MODULE_PORT_TYPE type, int idx){
    
    switch (type) {
        
        case AUDIO_I :
            if (idx < ai_) return audio_in_[idx];
            break;
        case AUDIO_O :
            if (idx < ao_) return audio_out_[idx];
            break;
        case MIDI_I :
            if (idx < mi_) return midi_in_[idx];
            break;
        case MIDI_O :
            if (idx < mo_) return midi_out_[idx];
            break;
    }
    return NULL;
}

int Module::get_port_count(MODULE_PORT_TYPE type){
    
    switch (type) {
        
        case AUDIO_I :
        
            return ai_;
            
        case AUDIO_O :
        
            return ao_;
            
        case MIDI_I :
            
            return mi_;
            
        case MIDI_O :
            
            return mo_;
    }
    return 0;
}