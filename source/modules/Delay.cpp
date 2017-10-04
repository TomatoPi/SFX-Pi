#include "Delay.h"

int delay_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Delay*>(u)->process_2(nframes, u);
}

Delay::Delay(const char *server):Module(server, MOD_DELAY, DELAY_PARAMS_COUNT,
    2, 2, 0, 0, "In", "Return", "Out", "Send"),
    samplerate_(jack_get_sample_rate(client_))
{
	
	jack_options_t options_2 = JackNullOption;
	jack_status_t status_2;
	
	jack_client_t *client_2;
	
	const char* name_2 = "Delay_Part_2";
	
	/*
	*	Creating jack client_2 with name "name_2", in server "server"
	*/
	client_2 = jack_client_open (name_2, options_2, &status_2, server);
	if (client_2 == NULL) {
		
        throw string("Unable to connect JACK server");
	}
	
	/*
	*	Upgrade name_2 if given name is not unique
	*/
	if (status_2 & JackNameNotUnique) {
		name_2 = jack_get_client_name(client_2);
		//cout << "Unique name " << name_2 << " assigned" << endl;
	}
	
	client_2_ = client_2;
	name_2_ = (char*)jack_get_client_name(client_2);
	
	jack_set_process_callback(client_2, delay_Process_Callback, this);
    
    buffer_ = new Buffer_S(100, samplerate_);
    
    this->set_param(MOD_COUNT + DELAY_PARAMS_COUNT, DELAY_DEFAULT_PARAMS);
    
    jack_port_unregister(client_, audio_in_[1]);
    audio_in_[1] = jack_port_register( client_2, "Return", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if ( audio_in_[1] == NULL ){
        
        throw string("Echec de réassignation du port return");
    }
	p_send_ = jack_port_register(client_, "p_send", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	p_return_ = jack_port_register(client_2, "p_return", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	if ( p_send_ == NULL || p_return_ == NULL ){
        
        throw string("Echec de creation des ports privés");
    }
    
	/*
	*	Activation of jack clients
	*/
	if ( jack_activate (client_) ){
        
		throw string("Echec de l'activation du client");
	}

	if ( jack_activate (client_2_) ){
        
		throw string("Echec de l'activation du client 2");
	}
    
    if ( jack_connect (client_, jack_port_name(p_send_), jack_port_name(p_return_)) ){
        
		throw string("Failed make private Connection");
	}
}

Delay::~Delay(){
	
	jack_client_close(this->client_2_);
    jack_deactivate( client_ );
    
    delete buffer_;
}
		
inline int Delay::do_process(jack_nframes_t nframes){

    sample_t *in = 	  (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    
    sample_t *out =   (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
        
    sample_t *send =  (sample_t*)jack_port_get_buffer(audio_out_[1], nframes);
    
    sample_t *p_out = (sample_t*)jack_port_get_buffer(p_send_, nframes);
        
    float vol = param_[MOD_VOLUME];
    
    float dw = param_[DELAY_DRYWET];

    for(jack_nframes_t i = 0; i < nframes; i++){
            
        //read buffer
        sample_t w = buffer_->read();
        
        //send buffer output to send port
        send[i] = w;
        
        //send input to write head, or 0 if module is bypassed
        p_out[i] = (is_bypassed_)?0:in[i];
        
        //mix buffer and input to regular output
        out[i] = vol * spi_dry_wet(in[i], w, dw);
    }

	return 0;
}

int Delay::process_2(jack_nframes_t nframes, void *arg){
	
    sample_t *rtrn;
    rtrn = (sample_t*)jack_port_get_buffer(audio_in_[1], nframes);

    sample_t *p_in;
    p_in = (sample_t*)jack_port_get_buffer(p_return_, nframes);
    
    float f = param_[DELAY_FEEDB];

    for(jack_nframes_t i = 0; i < nframes; i++){
        
        if(!this->is_bypassed_){
            
            // write clear input + feedbackloop to buffer
            buffer_->write(p_in[i] + f*rtrn[i]);
        }else{
            
            // If delay is bypassed stop write data in buffer
            buffer_->write(f*rtrn[i]);
        }
    }
        
	return 0;
}

/*
int Delay::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in;	
    in = 	(sample_t*)jack_port_get_buffer(audio_in_[0], nframes);
    
    sample_t *out;
    out = 	(sample_t*)jack_port_get_buffer(audio_out_[0], nframes);
        
    sample_t *send;
    send = 	(sample_t*)jack_port_get_buffer(audio_out_[1], nframes);
    
    sample_t *p_out;
    p_out = (sample_t*)jack_port_get_buffer(p_send_, nframes);
    
    float dw = param_[DELAY_DRYWET];
    
    for(jack_nframes_t i = 0; i < nframes; i++){
        
        sample_t w = buffer_->read();
        
        send[i] = w;
        
        p_out[i] = 0;
        
        out[i] = spi_dry_wet(in[i], w, dw);
    }

	return 0;
}
*/

void Delay::change_param(int idx, float value){
    
    //param_[idx] = value;
    
    if (idx == DELAY_DELAY) {
        
        buffer_->set_length((int)value, samplerate_);
    }
}

void Delay::change_param(const float *values){
    
    //memcpy(param_, values, sizeof(float) * param_c_);
    
    buffer_->set_length((int)param_[DELAY_DELAY], samplerate_);
}

void Delay::new_bank(){
    
    this->add_bank( MOD_COUNT + DELAY_PARAMS_COUNT, DELAY_DEFAULT_PARAMS);
}

string Delay::return_param_name(int idx){
    
    return DELAY_PARAM_NAMES[idx];
}

string Delay::return_formated_param(int idx){
    
    string n = DELAY_PARAM_NAMES[idx];
    
    switch (idx){
        
        case MOD_VOLUME :
        
            n += " ";
            break;
        
        case DELAY_DELAY :
        
            n += "  ";
            break;
        
        case DELAY_FEEDB :
            
            n += "  ";
            break;
            
        case DELAY_DRYWET :
            
            n += " ";
            break;
           
        default :
            break;
    }
    
    n += f_ftos( param_[idx] );
    
    return n;
} 
