#include "Delay.h"

int delay_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Delay*>(u)->process_2(nframes, u);
}

Delay::Delay(const char *server) : Module(server, MDELAY, 6, 0, 0, 0, 0){
	
	jack_options_t options_2 = JackNullOption;
	jack_status_t status_2;
	
	jack_client_t *client_2;
	
	const char* name_2 = "Delay_Part_2";
	
	//Creating jack client_2 with name "name_2", in server "server"
	client_2 = jack_client_open (name_2, options_2, &status_2, server);
	if (client_2 == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status_2);
		if (status_2 & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	
	//upgrade name_2 if given name is not unique
	if (status_2 & JackNameNotUnique) {
		name_2 = jack_get_client_name(client_2);
		fprintf (stderr, "Unique name `%s' assigned\n", name_2);
	}
	
	this->client_2 = client_2;
	this->name_2 = (char*)name_2;
	
	this->port_count = 8;
	this->port = (jack_port_t**)malloc(this->port_count * sizeof(jack_port_t*));
	if(this->port == NULL)
		return 1;
	
	this->port[0] = jack_port_register( this->client, "in_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	this->port[1] = jack_port_register( this->client, "in_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	
	this->port[2] = jack_port_register( this->client_2, "return_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	this->port[3] = jack_port_register( this->client_2, "return_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	
	
	this->port[4] = jack_port_register( this->client, "out_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	this->port[5] = jack_port_register( this->client, "out_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	this->port[6] = jack_port_register( this->client, "send_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	this->port[7] = jack_port_register( this->client, "send_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	
	this->private_port_count = 4;
	this->private_port = (jack_port_t**)malloc(this->private_port_count * sizeof(jack_port_t*));
	if(this->private_port == NULL)
		return 1;
	
	this->private_port[0] = jack_port_register( this->client, "p_out_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	this->private_port[1] = jack_port_register( this->client, "p_out_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	this->private_port[2] = jack_port_register( this->client_2, "p_in_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	this->private_port[3] = jack_port_register( this->client_2, "p_in_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	

	this->buffer_L = new Ringbuffer(D_DMAX, jack_get_sample_rate(this->client));
	this->buffer_R = new Ringbuffer(D_DMAX, jack_get_sample_rate(this->client));

	if(this->buffer_L == NULL || this->buffer_R == NULL){
		exit(1);
	}
	
	jack_set_process_callback(client_2, delay_Process_Callback, this);

	this->reader_L = this->buffer_L->new_read_head(D_DELAY);
	this->reader_R = this->buffer_R->new_read_head(D_DELAY);

	this->params[0] = D_DMAX;

	this->params[1] = D_DELAY;
	this->params[2] = D_DELAY;
	this->params[3] = D_FEED;
	this->params[4] = D_FEED;

	this->params[5] = 1.0;


	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}	
	if (jack_activate (this->client_2)) {
		fprintf (stderr, "Echec de l'activation du client 2\n");
		exit (1);
	}
	if (jack_connect (this->client, this->private_port[0], this->private_port[2])) {
		fprintf (stderr, "cannot make private connection L\n");
		return 1;
	}
	if (jack_connect (this->client, this->private_port[1], this->private_port[3])) {
		fprintf (stderr, "cannot make private connection R\n");
		return 1;
	}
}

Delay::~Delay(){
	
	delete this->buffer_L;
	delete this->buffer_R;
}
		
int Delay::process(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *in_R;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	
	sample_t *out_L, *out_R;
	out_L = (sample_t*)jack_port_get_buffer(this->port[4], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[5], nframes);
		
	sample_t *send_L, *send_R;
	send_L = (sample_t*)jack_port_get_buffer(this->port[6], nframes);
	send_R = (sample_t*)jack_port_get_buffer(this->port[7], nframes);
	
	sample_t *p_out_L, *p_out_R;
	p_out_L = (sample_t*)jack_port_get_buffer(this->private_port[0], nframes);
	p_out_R = (sample_t*)jack_port_get_buffer(this->private_port[1], nframes);
	
	
	if( (int)(this->params[1]) != this->reader_L.delay)
		this->reader_L = this->buffer_L->new_read_head(this->params[1]);
	
	if( (int)(this->params[2]) != this->reader_R.delay)
		this->reader_R = this->buffer_R->new_read_head(this->params[2]);
	
	float dw = this->params[5];

	for(jack_nframes_t i = 0; i < nframes; i++){
		
		this->buffer_L->read_value(&(this->reader_L));
		this->buffer_R->read_value(&(this->reader_R));
			
		sample_t wl = this->reader_L.value;
		sample_t wr = this->reader_R.value;
		
		send_L[i] = wl;
		send_R[i] = wr;
		
		p_out_L[i] = in_L[i];
		p_out_R[i] = in_R[i];
		
		out_L[i] = spi_dry_wet(in_L[i], wl, dw);
		out_R[i] = spi_dry_wet(in_R[i], wr, dw);
	}
	return 0;
}

int Delay::process_2(jack_nframes_t nframes, void *arg){
	
	sample_t *return_L, *return_R;
	return_L = (sample_t*)jack_port_get_buffer(this->port[2], nframes);
	return_R = (sample_t*)jack_port_get_buffer(this->port[3], nframes);
	
	sample_t *p_in_L, *p_in_R;
	p_in_L = (sample_t*)jack_port_get_buffer(this->private_port[2], nframes);
	p_in_R = (sample_t*)jack_port_get_buffer(this->private_port[3], nframes);
		
	float fl = this->params[3];
	float fr = this->params[4];
	
	for(jack_nframes_t i = 0; i < nframes; i++){
				
		this->buffer_L->write_value(p_in_L[i] + fl*return_L[i]);
		this->buffer_R->write_value(p_in_R[i] + fr*return_R[i]);
	}
	return 0;
}

int Delay::bypass(jack_nframes_t nframes, void *arg){

	sample_t *in_L, *in_R;	
	in_L = (sample_t*)jack_port_get_buffer(this->port[0], nframes);
	in_R = (sample_t*)jack_port_get_buffer(this->port[1], nframes);
	
	sample_t *out_L, *out_R;
	out_L = (sample_t*)jack_port_get_buffer(this->port[4], nframes);
	out_R = (sample_t*)jack_port_get_buffer(this->port[5], nframes);
		
	sample_t *send_L, *send_R;
	send_L = (sample_t*)jack_port_get_buffer(this->port[6], nframes);
	send_R = (sample_t*)jack_port_get_buffer(this->port[7], nframes);
	
	sample_t *p_out_L, *p_out_R;
	p_out_L = (sample_t*)jack_port_get_buffer(this->private_port[0], nframes);
	p_out_R = (sample_t*)jack_port_get_buffer(this->private_port[1], nframes);
	
	memcpy(out_L, in_L, sizeof(sample_t) * nframes);
	memcpy(out_R, in_R, sizeof(sample_t) * nframes);
	
	memset(send_L, 0.0, sizeof(sample_t) * nframes);
	memset(send_R, 0.0, sizeof(sample_t) * nframes);
	memset(p_out_L, 0.0, sizeof(sample_t) * nframes);
	memset(p_out_R, 0.0, sizeof(sample_t) * nframes);
	
	return 0;
}
