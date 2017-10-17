#include "Compressor.h"

using namespace PROG_CONST;
/**********************************************************************
 *                  Register Compressor Module
 **********************************************************************/
Module* build_comp( int id ){

    return new Compressor( id );
}
/**********************************************************************
 *                      Compressor Functions
 **********************************************************************/
Compressor::Compressor( int id ): Module(MN_COMP, TC_COMP, id, COMP_PARAMS_COUNT,
    2, 2, 0, 0, "In_L", "In_R", "Out_L", "Out_R"),
    buffer_( COMP_WINDOW, jack_get_sample_rate( client_ ) ),
    ramp_(0),
    ik_(0),
    isr_(0),
    thr2_(0),
    last_(false)
{

    this->set_param(MOD_COUNT + COMP_PARAMS_COUNT, COMP_DEFAULT_PARAM);

    if (jack_activate (this->client_)) {

        throw string("Failed activate client");
    }
}

inline int Compressor::do_process(jack_nframes_t nframes){

    sample_t *inl, *inr, *outl, *outr;

    inl = (sample_t*) jack_port_get_buffer( audio_in_[0], nframes );
    inr = (sample_t*) jack_port_get_buffer( audio_in_[1], nframes );

    outl = (sample_t*) jack_port_get_buffer( audio_out_[0], nframes );
    outr = (sample_t*) jack_port_get_buffer( audio_out_[1], nframes );

    if ( !is_bypassed_ ){

        float vol, rat, att, rel, thr;

        vol = param_[MOD_VOLUME];

        rat = param_[COMP_RATIO];
        att = param_[COMP_ATT];
        rel = param_[COMP_REL];
        thr = param_[COMP_THR];

        bool gate = !!param_[COMP_GATE];

        float cut = param_[COMP_CUT];
        float dw = param_[COMP_CUT];

        for( jack_nframes_t i = 0; i < nframes; i++ ){

            /*
            *   First compute running summ
            */
            buffer_.write( (inl[i] + inr[i])/2.0f );
            float summ = buffer_.get_rms2();

            bool t = summ < thr2_;
            // If threshold have been passed ( no matter the direction ) reset ramp, and store rate
            if( last_ != t ){

                ramp_ = 0;
            }

            //If below threshold, decay compression rate
            if ( t ){

                // If under gate level, cut output
                if ( gate && cut * cut > thr2_){

                    outl[i] = 0;
                    outr[i] = 0;
                }
                // Else decay compression rate
                else{

                    //Calculate current compression rate
                    ramp_ += isr_;
                    if ( ramp_ > 15.0f * rel ) ramp_ = 15.0f * rel;

                    float r = ik_ + (1 - ik_) * (1 - exp(-ramp_/rel));

                    //Apply compression
                    outl[i] = spi_dry_wet( inl[i], vol * r * inl[i], dw );
                    outr[i] = spi_dry_wet( inr[i], vol * r * inr[i], dw );

                }

            }
            // If beyond, increase compression rate
            else{

                //Calculate current compression rate
                ramp_ += isr_;
                if ( ramp_ > 15.0f * att ) ramp_ = 15.0f * att;

                float r = ik_ + (1 - ik_) * exp(-ramp_/att);

                //Apply compression
                outl[i] = spi_dry_wet( inl[i], vol * r * inl[i], dw );
                outr[i] = spi_dry_wet( inr[i], vol * r * inr[i], dw );
            }

            last_ = t;

        }

    }else{

        memcpy( outl, inl, sizeof(sample_t) * nframes );
        memcpy( outr, inr, sizeof(sample_t) * nframes );
    }

    return 0;

}

void Compressor::change_param(int idx, float value){

    if( idx == COMP_RATIO ){

        ik_ = 1.0f/value;
    }
    else if ( idx == COMP_THR ){

        thr2_ = value * value;
    }
}

void Compressor::change_param(const float *values){

    ik_ = 1.0f / param_[COMP_RATIO];
    thr2_ = param_[COMP_THR] * param_[COMP_THR];
}
/*
string Compressor::return_param_name(int idx){

    return COMP_PARAM_NAMES[idx];
}

string Compressor::return_formated_param(int idx){

    string n = COMP_PARAM_NAMES[idx];
    n.resize(7);

    if( idx == COMP_RATIO ){

        n += f_ftos( spi_dbtorms(param_[idx]) );

    }else{

        n += f_ftos( param_[idx] );
    }

    return n;
}
*/
void Compressor::new_bank(){

    this->add_bank( MOD_COUNT + COMP_PARAMS_COUNT, COMP_DEFAULT_PARAM );
}
