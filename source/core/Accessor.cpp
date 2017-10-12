#include "Modules.h"
#include "Utility.h"
/*
*   ---------------------------------------------------------------------------
*   IO Accessor stuff
*   ---------------------------------------------------------------------------
*/
Accessor::Accessor(int target, int targetp, float min, float max, IO_CURVE curve, bool isdb, bool isrelative):
    target_(target),
    targetp_(targetp),
    min_(min),
    max_(max),
    isdb_(isdb),
    isrlt_(isrelative)
{

    set_curve(curve);
}

float Accessor::compute( float input , float ref ){

    // Remap param with accessor's transfert function
    float param = (*this->curve_func_)( (input+1.0f)/2.0f ) ;

    // If accessor is relative
    if ( isrlt_ ){

        float offset = ( param < 0 )? param*min_ : param*max_; // offset is now between -min and +max;

        // If boundaries are given in dB
        if(isdb_){

            return ref * spi_dbtorms( offset );
        }
        else{

            return ref + offset;
        }
    }
    // Else if accessor is absolute
    else{

        //map param from [-1;1] to [min;max]
        param = ( (param + 1)/(2.0f) ) * ( max_ - min_ ) + min_;

        if(isdb_){

            return spi_dbtorms(param);
        }
        else{

            return param;
        }

    }
}

IO_CURVE Accessor::get_curve() const{

    return curve_;
}

void Accessor::set_curve( IO_CURVE curve ){

    curve_ = curve;

    switch(curve){

        case CURVE_LIN :

            curve_func_ = curve_lin;
            break;

        case CURVE_SIG :

            curve_func_ = curve_sig;
            break;

        case CURVE_COS :

            curve_func_ = curve_cos;
            break;
        default :

            curve_func_ = curve_lin;
            break;
    }
}

inline float curve_lin(float v){

    return (v);
}

inline float curve_sig(float v){

    return ( tanh( 3*(v) ) );
}

inline float curve_cos(float v){

    return ( cos( (M_PI/2)*(v-1) ) );
}
