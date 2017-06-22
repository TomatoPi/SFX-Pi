#include "Delay.h"

int delay_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Delay*>(u)->process_2(nframes, u);
}

/*
*	---------------------------------------------------------------------------
*	Delay voice stuff
*	---------------------------------------------------------------------------
*/
Delay_voice::Delay_voice(jack_client_t *client, jack_client_t *client_2, int idx): Module_voice(client, DELAY_PARAMS, 2, 2, 0, 0){
	
	this->samplerate = jack_get_sample_rate(client);
	
	this->buffer_ = new Buffer_S(100, this->samplerate);
	this->set_param_list(DELAY_PARAMS, (float*)default_delay_values);

	char n[20];
	sprintf(n, "In_%d", idx);
	this->port_audio_in_[0] = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	sprintf(n, "Return_%d", idx);
	this->port_audio_in_[1] = jack_port_register( client_2, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	
	sprintf(n, "Out_%d", idx);
	this->port_audio_out_[0] = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	sprintf(n, "Send_%d", idx);
	this->port_audio_out_[1] = jack_port_register( client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	sprintf(n, "p_send_%d", idx);
	this->p_send_ = jack_port_register(client, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	sprintf(n, "p_return_%d", idx);
	this->p_return_ = jack_port_register(client_2, n, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
}

Delay_voice::~Delay_voice(){
	
	delete this->buffer_;
}

void Delay_voice::set_param(int param, float var){
	
	if(param == DELAY_DELAY){
		
		this->buffer_->set_length((int)var, this->samplerate);
		this->param_[DELAY_DELAY] = var;
	}else{
		
		this->Module_voice::set_param(param, var);
	}
}

void Delay_voice::set_param_list(int size, float *pl){
	
	if(size == this->param_count_){
		this->Module_voice::set_param_list(size, pl);
		this->buffer_->set_length((int)this->param_[DELAY_DELAY], this->samplerate);
	}
}

jack_port_t* Delay_voice::get_p_send() const{
	
	return this->p_send_;
}

jack_port_t* Delay_voice::get_p_return() const{
	
	return this->p_return_;
}

Buffer_S* Delay_voice::get_buffer() const{
	
	return this->buffer_;
}

void Delay_voice::connect(jack_client_t *client){
	
	/*
	*	Connect direct link
	*/
	if (jack_connect (client, jack_port_name(this->p_send_), jack_port_name(this->p_return_))) {
		fprintf (stderr, "cannot make private connection \n");
		exit(1);
	}
	this->is_ready_ = 1;
}

/*
*	---------------------------------------------------------------------------
*	Delay Stuff
*	---------------------------------------------------------------------------
*/
Delay::Delay(const char *server, int vc) : Module(server, MDELAY, vc){
	
	jack_options_t options_2 = JackNullOption;
	jack_status_t status_2;
	
	jack_client_t *client_2;
	
	const char* name_2 = "Delay_Part_2";
	
	/*
	*	Creating jack client_2 with name "name_2", in server "server"
	*/
	client_2 = jack_client_open (name_2, options_2, &status_2, server);
	if (client_2 == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status_2);
		if (status_2 & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	
	/*
	*	Upgrade name_2 if given name is not unique
	*/
	if (status_2 & JackNameNotUnique) {
		name_2 = jack_get_client_name(client_2);
		fprintf (stderr, "Unique name `%s' assigned\n", name_2);
	}
	
	this->client_2_ = client_2;
	this->name_2_ = (char*)name_2;
	
	jack_set_process_callback(client_2, delay_Process_Callback, this);

	for(int i = 0; i < vc; i++){
		Delay_voice *mod = new Delay_voice(this->client_, this->client_2_, i);
		this->voice_.push_back(mod);
	}
	
	/*
	*	Activation of jack clients
	*/
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}

	if (jack_activate (this->client_2_)) {
		fprintf (stderr, "Echec de l'activation du client 2\n");
		exit (1);
	}
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		((Delay_voice*)*itr)->connect(this->client_);
	}
}

Delay::~Delay(){
	
	jack_client_close(this->client_2_);
}
		
int Delay::process(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(!!(*itr)->is_ready()){
			
			sample_t *in;	
			in 	= 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);
			
			sample_t *out;
			out = 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
				
			sample_t *send;
			send = 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 1), nframes);
			
			sample_t *p_out;
			p_out = (sample_t*)jack_port_get_buffer(((Delay_voice*)*itr)->get_p_send(), nframes);
			
			float dw = (*itr)->get_param(DELAY_DRYWET);

			for(jack_nframes_t i = 0; i < nframes; i++){
					
				//read buffer
				sample_t w = ((Delay_voice*)*itr)->get_buffer()->read();
				
				//send buffer output to send port
				send[i] = w;
				
				//send input to write head
				p_out[i] = in[i];
				
				//mix buffer and input to regular output
				out[i] = spi_dry_wet(in[i], w, dw);
			}
		}
	}
	return 0;
}

int Delay::process_2(jack_nframes_t nframes, void *arg){
	
	if(!this->is_bypassed_){
		for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
			if(!!((Delay_voice*)*itr)->is_ready()){
			
				sample_t *rtrn;
				rtrn = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 1), nframes);

				sample_t *p_in;
				p_in = (sample_t*)jack_port_get_buffer(((Delay_voice*)*itr)->get_p_return(), nframes);
				
				float f = (*itr)->get_param(DELAY_FEEDB);

				for(jack_nframes_t i = 0; i < nframes; i++){
					
					//write clear input + feedbackloop to buffer
					((Delay_voice*)*itr)->get_buffer()->write(p_in[i] + f*rtrn[i]);
				}
			}
		}
	}
	return 0;
}

int Delay::bypass(jack_nframes_t nframes, void *arg){

	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		if(((Delay_voice*)*itr)->is_ready()){
		
			sample_t *in;	
			in = 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);
			
			sample_t *out;
			out = 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);
				
			sample_t *send;
			send = 	(sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 1), nframes);
			
			sample_t *p_out;
			p_out = (sample_t*)jack_port_get_buffer(((Delay_voice*)*itr)->get_p_send(), nframes);
			
			/*
			*	Copy input to output and set sends to 0
			*/
			memcpy(out, in, sizeof(sample_t) * nframes);
			
			memset(send, 0.0f, sizeof(sample_t) * nframes);
			memset(p_out, 0.0f, sizeof(sample_t) * nframes);
			
		}
	}
	return 0;
}

void Delay::add_voice(){
	
	int idx = this->voice_.size();
	Delay_voice *mod = new Delay_voice(this->client_, this->client_2_, idx);
	mod->connect(this->client_);
	this->voice_.push_back(mod);
}

const char* Delay::get_param_name(int p) const{
	
	if(p < this->get_voice(0)->get_param_count())
		return delay_param_names[p];
	return "NULL";
}