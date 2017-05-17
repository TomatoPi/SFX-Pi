#include "utility.h"

/**
*	Fonction utile pour enregistrer la fonction callback des effets.
*/
int fx_Process_Callback(jack_nframes_t nframes, void *u){
	
	return static_cast<Fx_Client*>(u)->process(nframes, u);
}

/*
*	Constructeur de base des clients audio
*/
Fx_Client::Fx_Client(const char *server, const char *name){

	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client_t *client;
	
	//Creation du client JACK, "name" dans le serveur "server"
	client = jack_client_open (name, options, &status, server);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	//si un client porte deja ce nom, mise a jour du nom
	if (status & JackNameNotUnique) {
		name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", name);
	}
	
	//enregistrement de la fonction callback
	jack_set_process_callback(client, fx_Process_Callback, this);
	
	this->client = client;
	this->name = (char*)name;
}

sample_t sfx_clip(sample_t in, float min, float max){
	
	return (in > max)? max : ((in < min)?min : in);
}

sample_t sfx_soft(sample_t in, float max, float soft, float shape){
	
	return max * ((1 - shape)*tanh(in) + shape * tanh(in/soft));
}

sample_t sfx_abs(sample_t in){
	
	return (in < 0)?-in:in;
}

float sfx_dbtorms(float d){
	
	return pow(10, d/20.0);
}

float sfx_rmstodb(float g){
	
	return 20 * log10(g);
}

void sfx_init_tripole(sfx_tripole *f, int fl, int fh, int sr, float gl, float gm, float gh){
	
	//initialisation de toutes les valuers a 0
	memset(f,0,sizeof(sfx_tripole));
	
	//initialisation des gains
	f->gl = gl;
	f->gm = gm;
	f->gh = gh;

	//Scalling filters frequencies with samplerate
	f->fl = 2 * sin(M_PI * ((float)fl / (float)sr)); 
	f->fh = 2 * sin(M_PI * ((float)fh / (float)sr));
}

sample_t sfx_do_tripole(sfx_tripole* f, sample_t sample){
	
	sample_t l,m,h; // Low / Mid / High

	// Calcul du passe bas si bande basse presente
	if(f->gl != 0.0 || f->gm != 0){
		f->f1p0 += (f->fl * (sample - f->f1p0)) + vsa;
		f->f1p1 += (f->fl * (f->f1p0 - f->f1p1));
		f->f1p2 += (f->fl * (f->f1p1 - f->f1p2));
		f->f1p3 += (f->fl * (f->f1p2 - f->f1p3));

		l = f->f1p3;
	}else{
		l = 0.0;
	}
	
	// Calcul du passe haut si la bande est presente
	if(f->gh != 0.0 || f->gm != 0){
		f->f2p0 += (f->fh * (sample - f->f2p0)) + vsa;
		f->f2p1 += (f->fh * (f->f2p0 - f->f2p1));
		f->f2p2 += (f->fh * (f->f2p1 - f->f2p2));
		f->f2p3 += (f->fh * (f->f2p2 - f->f2p3));

		h = f->sm3 - f->f2p3;
	}else{
		h = 0.0;
	}

	// Calcul de la bande medium ( sample - (low + high))
	m = f->sm3 - (h + l);

	// Application des gains
	l *= f->gl;
	m *= f->gm;
	h *= f->gh;

	// Roullement du buffer
	f->sm3 = f->sm2;
	f->sm2 = f->sm1;
	f->sm1 = sample; 

	return(l + m + h);
}
