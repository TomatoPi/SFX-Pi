#include "Modules.h"
#include "Utility.h"
/*
*	---------------------------------------------------------------------------
*	IO Accessor stuff
*	---------------------------------------------------------------------------
*/
Accessor::Accessor(int source, int target, float min, float max, IO_CURVE curve, bool isdb, bool isrelative):
	source_(source),
    target_(target),
    min_(min),
    max_(max),
    isdb_(isdb),
    isrlt_(isrelative)
{

    set_curve(curve);
}

float Accessor::compute( float input, float mod ){
	
    
    float param = ( (*this->curve_func_)( (mod+1.0f)/2.0f ) * (max_ - min_) ) + min_;
    
    if ( isrlt_ ){
        
        if(isdb_){
            
            return input * spi_dbtorms(param);
        }else{
            
            return input + param;
        }
        
    }else{
        
        if(isdb_){
            
            return spi_dbtorms(param);
        }else{
            
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
            
		case CURVE_HAN :
        
			curve_func_ = curve_han;
			break;
            
		case CURVEIHAN :
        
			curve_func_ = curveihan;
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
	
	return (0.5f*( tanh( 5*((v)-0.5f)) + 1 ));
}

inline float curve_han(float v){
	
	return (0.5f*( 1 - cos(2*M_PI*(v)) ));
}

inline float curveihan(float v){
	
	return (1 - curve_han(v));
}
