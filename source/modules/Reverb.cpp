#include "Reverb.h"

using namespace PROG_CONST;
/**********************************************************************
 *                       Register Reverb Module
 **********************************************************************/
Module* build_rev( int id ){

    return new Reverb( id );
}
/**********************************************************************
 *                          Reverb Functions
 **********************************************************************/
Reverb::Reverb( int id ) : Module(MN_REV, TC_REV, id, REVERB_PARAMS_COUNT,
    1, 2, 0, 0, "In", "Out", "Rev"),
    samplerate_(jack_get_sample_rate(client_)),
    gain_(0.015f)
{

    for(int i = 0; i < COMBCOUNT; i++){

        comb_[i] = Filter_comb(damp_, room_, combsize[i] * samplerate_ / 44100, samplerate_, 0);
    }
    for(int i = 0; i < ALLPCOUNT; i++){

        allp_[i] = Filter_allpass(allpf_, allpsize[i]);
    }

    this->set_param(MOD_COUNT + REVERB_PARAMS_COUNT, REVERB_DEFAULT_PARAMS);

    if (jack_activate (client_)) {

        throw string("Failed activate client");
    }
}

inline int Reverb::do_process(jack_nframes_t nframes){

    sample_t *in;
    in = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);

    sample_t *out;
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);

    sample_t *rev;
    rev = (sample_t*)jack_port_get_buffer(audio_out_[1], nframes);

    float vol = param_[MOD_VOLUME];

    float gain = gain_;

    float dry = param_[REV_DRY];
    float wet = param_[REV_WET];

    for(jack_nframes_t i = 0; i < nframes; i++){

        sample_t input = in[i]* gain;
        rev[i] = 0;

        for(int k = 0; k < COMBCOUNT; k++){

            rev[i] += comb_[k].compute((is_bypassed_)?0:input);
        }

        for(int k = 0; k < ALLPCOUNT; k++){

            rev[i] = allp_[k].compute(rev[i]);
        }

        out[i] = vol * ( rev[i]*wet + in[i]*dry );

    }

    return 0;
}

/*
int Reverb::bypass(jack_nframes_t nframes, void *arg){

    sample_t *in;
    in = (sample_t*)jack_port_get_buffer(audio_in_[0], nframes);

    sample_t *out;
    out = (sample_t*)jack_port_get_buffer(audio_out_[0], nframes);

    sample_t *rev;
    rev = (sample_t*)jack_port_get_buffer(audio_out_[1], nframes);

    float gain = gain_;

    float dry = param_[REV_DRY];
    float wet = param_[REV_WET];

    for(jack_nframes_t i = 0; i < nframes; i++){

        rev[i] = 0;

        for(int k = 0; k < COMBCOUNT; k++){

            rev[i] += comb_[k].compute(0);
        }

        for(int k = 0; k < ALLPCOUNT; k++){

            rev[i] = allp_[k].compute(rev[i]);
        }

        out[i] = rev[i]*wet + in[i]*dry;

    }

    return 0;
}
*/

void  Reverb::change_param(int idx, float value){

    //param_[idx] = value;

    switch(idx){

        case REV_ROOM :

            room_ = (value * scaleroom) + offsetroom;
            break;

        case REV_DAMP :

            damp_ = value * scaledamp;
            break;

        case REV_WET :

            wet_ = value * scalewet;
            break;

        case REV_DRY :

            dry_ = value * scaledry;
            break;

        case REV_ALL :

            allpf_ = value;
            break;

        default :
            break;
    }

    this->update();
}

void  Reverb::change_param(const float *values){

    //memcpy(param_, values, sizeof(sample_t) * param_c_);

    room_ = (param_[REV_ROOM] * scaleroom) + offsetroom;
    damp_  = param_[REV_DAMP] * scaledamp;
    wet_   = param_[REV_WET]  * scalewet;
    dry_   = param_[REV_DRY]  * scaledry;
    allpf_ = param_[REV_ALL];

    this->update();
}

void Reverb::new_bank(){

    this->add_bank( MOD_COUNT + REVERB_PARAMS_COUNT, REVERB_DEFAULT_PARAMS);
}

string  Reverb::return_param_name(int idx){

    return REVERB_PARAM_NAMES[idx];
}

string Reverb::return_formated_param(int idx){

    string n = REVERB_PARAM_NAMES[idx];

    switch (idx){

        case MOD_VOLUME :

            n += " ";
            n += f_ftos( param_[idx] );
            break;

        case REV_ROOM :

            n += "    ";
            n += f_ftos( param_[idx] );
            break;

        case REV_DAMP :

            n += "    ";
            n += f_ftos( param_[idx] );
            break;

        case REV_WET :

            n += "     ";
            n += f_ftos( param_[idx] );
            break;

        case REV_DRY :

            n += "     ";
            n += f_ftos( param_[idx] );
            break;

        case REV_ALL :

            n += "   ";
            n += f_ftos( param_[idx] );
            break;

        default :
            break;
    }

    return n;
}

void Reverb::update(){

    for(int i = 0; i < COMBCOUNT; i++){

        comb_[i].set_feedback(room_);
        comb_[i].set_damp(damp_);
    }

    for(int i = 0; i < ALLPCOUNT; i++){

        allp_[i].set_feedback(allpf_);
    }
}
