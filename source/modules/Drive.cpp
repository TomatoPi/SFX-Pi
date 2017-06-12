#include "Drive.h"

Drive_voice::Drive_voice(jack_client_t *client, int idx): Module_voice(client, DRIVE_PARAMS, 1, 1, 0, 0){

	this->filter = new Filter_3EQ(200, 1000, jack_get_sample_rate(client));
	this->set_param_list(DRIVE_PARAMS, (float*)default_drive_values);
	
	char n[10];
	sprintf(n, "In_%d", idx);
	register_port(client, this->port_audio_in_, PORT_AI, 1, n);
	sprintf(n, "Out_%d", idx);
	register_port(client, this->port_audio_out_, PORT_AO, 1, n);
}

Drive_voice::~Drive_voice(){
	
	delete this->filter;
}

void Drive_voice::set_param(int param, float var){

	if(param == DRIVE_F_BASS){
		
		this->filter->set_lf(var);
		this->param_[DRIVE_F_BASS] = var;
	}else if(param == DRIVE_F_HIGH){
		
		this->filter->set_hf(var);
		this->param_[DRIVE_F_HIGH] = var;
	}else{
		
		return this->Module_voice::set_param(param, var);
	}
}

void Drive_voice::set_param_list(int size, float *pl){

	if(size == this->param_count_){
		
		this->Module_voice::set_param_list(size, pl);
		this->filter->set_freq(this->param_[DRIVE_F_BASS], this->param_[DRIVE_F_HIGH]);
	}
}

Filter_3EQ* Drive_voice::get_filter() const{
	
	return this->filter;
}

/*
*	Distortion effect constructor
*	EQ creation
*/
Drive::Drive(const char *server, int vc): Module(server, MDRIVE, DRIVE_PARAMS, vc){

	for(int i = 0; i < vc; i++){
		
		Module_voice *mod = new Drive_voice(this->client_, i);
		this->voice_.push_back(mod);
	}
	
	if (jack_activate (this->client_)) {
		fprintf (stderr, "Failed activating Client\n");
		exit (1);
	}
}

Drive::~Drive(){
	
}

/*
*	Drive effect's Callback
*/
int Drive::process(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){ //For each voice
		
		sample_t *in, *out;	
		in  = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AI, 0), nframes);	// Collecting input buffer
		out = (sample_t*)jack_port_get_buffer((*itr)->get_port(PORT_AO, 0), nframes);	// Collecting output buffer

		float gp, sp, shp, gn, sn, shn;			// Collecting drive settings
		int is_abs, isp, isn;
		
		is_abs = (int)!!(*itr)->get_param(DRIVE_ABS);
		int n = ((int)!!(*itr)->get_param(DRIVE_ASM))?0:4;
		
		gp = 		(*itr)->get_param(DRIVE_GAIN_P);
		isp = (int)!!(*itr)->get_param(DRIVE_TYPE_P);
		sp = 		(*itr)->get_param(DRIVE_SOFT_P);
		shp = 		(*itr)->get_param(DRIVE_SHAPE_P);
		
		gn = 		(*itr)->get_param(DRIVE_GAIN_P +n);
		isn = (int)!!(*itr)->get_param(DRIVE_TYPE_P +n);
		sn = 		(*itr)->get_param(DRIVE_SOFT_P +n);
		shn = 		(*itr)->get_param(DRIVE_SHAPE_P +n);
		
		float gl, gm, gh;
		gl = (*itr)->get_param(DRIVE_F_GBASS);
		gm = (*itr)->get_param(DRIVE_F_GMID);
		gh = (*itr)->get_param(DRIVE_F_GHIGH);
			
		for(jack_nframes_t i = 0; i < nframes; i++){									//For each sample
			
			sample_t l = (Drive_voice*)(*itr)->get_filter()->compute(in[i], gl, gm, gh);	//Compute 3bands EQ
			
			if(l>0){ 																	//if positive
				if(isp){																	//soft-clipping or hard-clipping
					out[i] = spi_soft(l*gp, 1.0, sp, shp);										//soft-clipping
				}else{																		//else
					out[i] = spi_clip(l*gp, -1.0, 1.0);											//hard-clipping
				}																
			}else{																		//if negative	
				if(isn){                                      								//soft-clipping or hard-clipping
					out[i] = spi_soft(l*gn, 1.0, sn, shn);          							//soft-clipping
				}else{                                                              		//else
					out[i] = spi_clip(l*gn, -1.0, 1.0);                         				//hard-clipping
				}
			}
			
			if(is_abs){							//if absolute value mode is true, full-wave rectification
				out[i] = spi_abs(out[i]);
			}
		}
	}
	return 0;
}

void Drive::add_voice(){
	
	int idx = this->voice_.size();
	Module_voice *mod = new Drive_voice(this->client_, idx);
	this->voice_.push_back(mod);
}

int Drive::bypass(jack_nframes_t nframes, void *arg){
	
	for(Voice_array::iterator itr = this->voice_.begin(); itr != this->voice_.end(); itr++){
		
		mod_generic_bypass_callback(nframes, (*itr)->get_port(PORT_AI, 0), (*itr)->get_port(PORT_AO, 0));
	}
}

const char* Drive::get_param_name(int p) const{
	
	if(p < ->get_voice(0)->get_param_count())
		return drive_param_names[p];
	return "NULL";
}