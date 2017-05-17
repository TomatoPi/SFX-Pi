#include "Fx_Unit.h"

/**
* 	Constructeur de base pour les effets
*	Setup du client JACK, enregistrement de la fonction de callback
*/
Fx_Unit::Fx_Unit(const char *server, const char *unit_name) : Fx_Client(server, unit_name){
	
	jack_port_t *in_l, *in_r, *out_l, *out_r;
	
	in_l = jack_port_register (this->client, "in_l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	in_r= jack_port_register (this->client, "in_r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	
	out_l = jack_port_register (this->client, "out_l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	out_r = jack_port_register (this->client, "out_r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	if ((in_l == NULL) || (out_l == NULL) || (in_r == NULL) || (out_r == NULL)) {
		fprintf(stderr, "Aucun port JACK disponible\n");
		exit (1);
	}
	
	this->a_port = (jack_port_t**)malloc(4 * sizeof(jack_port_t*));
	if(this->a_port == NULL){
		fprintf(stderr, "Echec de creation de la liste de ports\n");
		exit(1);
	}
	this->a_port[0] = in_l;
	this->a_port[1] = in_r;
	this->a_port[2] = out_l;
	this->a_port[3] = out_r;
	
	fprintf (stderr, "FX_Unit constructor --> Fin\n");
}

/**
*	Constructeur du fx de distortion
*/
Drive::Drive(const char *server, const char *name): Fx_Unit(server, name), gp(70), gn(30), sp(10), sn(5), shp(0.29), shn(0.8), is_abs(0), is_asm(1), is_soft_clip_p(1), is_soft_clip_n(0){
	
	fprintf (stderr, "Creation d'un hardClipper\n");
	
	this->filter = (sfx_tripole*)malloc(sizeof(sfx_tripole));
	fprintf (stderr, "Filtre cree\n");
	sfx_init_tripole(this->filter, 200, 1000, (int)jack_get_sample_rate(this->client), 0.75, 3.0, 7.0);
	
	if (jack_activate (this->client)) {
		fprintf (stderr, "Echec de l'activation du client\n");
		exit (1);
	}
	
	fprintf (stderr, "Client OK\n");
}

int Drive::process(jack_nframes_t nframes, void *arg){
	
	sample_t *s_in_l, *s_out_l;	
	s_in_l = (sample_t*)jack_port_get_buffer(this->a_port[0], nframes);
	s_out_l = (sample_t*)jack_port_get_buffer(this->a_port[2], nframes);
	
	sample_t *s_in_r, *s_out_r;	
	s_in_r = (sample_t*)jack_port_get_buffer(this->a_port[1], nframes);
	s_out_r = (sample_t*)jack_port_get_buffer(this->a_port[3], nframes);
	
	float g1 = this->gp, g2 = this->gn;
	
	for(jack_nframes_t i = 0; i < nframes; i++){							//Pour chaque sample
		
		sample_t l = sfx_do_tripole(this->filter, s_in_l[i]);				//sample actuel gauche
		sample_t r = sfx_do_tripole(this->filter, s_in_r[i]);				//--------------droite
		
		if(this->is_asm){ 													//si le clipping est asymetrique
			if(l>0){ 															//si positif
				if(this->is_soft_clip_p){											//softclip ou hardclip
					s_out_l[i] = sfx_soft(l*g1, 0.5, this->sp, this->shp);				//softclip
				}else{																//sinon
					s_out_l[i] = sfx_clip(l*g1, -0.5, 0.5);								//hardclip
				}																
			}else{																//si negatif	
				if(this->is_soft_clip_n){                                       	//softclip ou hardclip
					s_out_l[i] = sfx_soft(l*g2, 0.5, this->sn, this->shn);          	//softclip
				}else{                                                              //sinon
					s_out_l[i] = sfx_clip(l*g2, -0.5, 0.5);                         	//hardclip
				}
			}
			if(r>0){															//Pareil pour la voie droite
				if(this->is_soft_clip_p){
					s_out_r[i] = sfx_soft(r*g1, 0.5, this->sp, this->shp);
				}else{
					s_out_r[i] = sfx_clip(r*g1, -0.5, 0.5);
				}
			}else{
				if(this->is_soft_clip_n){
					s_out_r[i] = sfx_soft(r*g2, 0.5, this->sn, this->shn);
				}else{
					s_out_r[i] = sfx_clip(r*g2, -0.5, 0.5);
				}
			}
		}else{																//si le clipping est symetrique
			if(this->is_soft_clip_p){											//softclip ou hardclip
					s_out_l[i] = sfx_soft(l*g1, 0.5, this->sp, this->shp);			//softclip
				}else{															//sinon
					s_out_l[i] = sfx_clip(l*g1, -0.5, 0.5);							//hardclip
				}
			if(this->is_soft_clip_p){											//pareil pour la droite
					s_out_r[i] = sfx_soft(r*g1, 0.5, this->sp, this->shp);
				}else{
					s_out_r[i] = sfx_clip(r*g1, -0.5, 0.5);
				}
				
		}
		if(this->is_abs){													//si le mode valeur absolue est activé, redressage du signal
			s_out_l[i] = sfx_abs(s_out_l[i]);
			s_out_r[i] = sfx_abs(s_out_r[i]);
		}
	}
	return 0;
}