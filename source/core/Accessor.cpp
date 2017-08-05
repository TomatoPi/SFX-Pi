#include "Modules.h"
#include "Utility.h"
/*
*	---------------------------------------------------------------------------
*	IO Accessor stuff
*	---------------------------------------------------------------------------
*/
IO_Accessor::IO_Accessor(Module *module, int target_idx, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv):
	module_(module),
	curve_type(curve),
	target_idx(target_idx),
	target_param(target_param),
	potentiometer(potentiometer),
	min(min),
	max(max),
	is_db(is_db),
	is_inv(is_inv),
	value(0),
	state(1)
{
	
	switch(curve){
		case CURVE_LIN :
			this->curve = curve_lin;
			break;
		case CURVE_SIG :
			this->curve = curve_sig;
			break;
		case CURVE_HAN :
			this->curve = curve_han;
			break;
		case CURVEIHAN :
			this->curve = curveihan;
			break;
		default :
			this->curve = curve_lin;
			break;
	}

	this->state = this->potentiometer >= 0 && this->potentiometer < SPI_POTAR_COUNT;
	this->state = target_param < module_->get_param_count();
}

int IO_Accessor::update(int *potar_tab){
	
	if(this->state && this->module_ != NULL){
		
		int value = potar_tab[this->potentiometer];
		float diff = (float)value - (float)this->value;
		if( (diff > SPI_HYSTERESIS * SPI_MAX && diff > 0) || (diff < -SPI_HYSTERESIS * SPI_MAX && diff < 0)){
			
			this->value = value;
			if(this->is_inv)value = SPI_MAX - value;
			
			float param = (((*this->curve)((float)value/SPI_MAX))*(this->max - this->min)) + this->min;

			if(is_db) param = spi_dbtorms(param);
			
			this->module_->set_param(this->target_param, param);
			return 1;
		}
		return 0;
	}
	return -1;
}

int IO_Accessor::is_dead() const{
	
	return !this->state;
}

int IO_Accessor::get_target_idx() const{
	
	return this->target_idx;
}

int IO_Accessor::get_target_param() const{
	
	return this->target_param;
}

int IO_Accessor::get_potar() const{
	
	return this->potentiometer;
}

float IO_Accessor::get_min() const{
	
	return this->min;
}

float IO_Accessor::get_max() const{
	
	return this->max;
}

int IO_Accessor::get_curve() const{
	
	return static_cast<int>(this->curve_type);
}

int IO_Accessor::get_db() const{
	
	return this->is_db;
}

int IO_Accessor::get_inv() const{
	
	return this->is_inv;
}

inline float curve_lin(float v){
	
	return (v);
}

inline float curve_sig(float v){
	
	return 0.5f*( tanh( 5*((v)-0.5f)) + 1 );
}

inline float curve_han(float v){
	
	return 0.5f*( 1 - cos(2*M_PI*(v)) );
}

inline float curveihan(float v){
	
	return 1 - curve_han(v);
}
