#include "io.h"

void io_init_spi(){

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

int io_get_potentiometer(int potentiometer){
	
	if(potentiometer < 8 && potentiometer >= 0) return analogRead(SPI_BASE + potentiometer);
	return -1;
}

IO_Accessor::IO_Accessor(Module *target, int param, int potentiometer, float min, float max, int is_db, int is_inv):target(target), param(param), min(min), max(max), is_db(is_db), is_inv(is_inv), value(0){

	if(potentiometer < 0 || potentiometer > 7)
		this->potentiometer = -1;
	else
		this->potentiometer = potentiometer;
	
	if(target == NULL)
		this->param = -1;
	if(param >= target->get_param_count())
		this->param = -1;
}

void IO_Accessor::update(){
	
	int value = io_get_potentiometer(this->potentiometer);
	
	if(this->target != NULL){
		if( ((float)value > (float)this->value * (1.0f + SPI_HYSTERESIS)) || ((float)value  < (float)this->value * (1.0f - SPI_HYSTERESIS)) ){
			
			this->value = value;
			if(this->is_inv)value = SPI_MAX - value;
			
			float param = (((float)value/(float)SPI_MAX) * (this->max - this->min)) + this->min;

			if(is_db){
				if(this->target->set_param(this->param, spi_dbtorms(param)))
					this->param = -1;
			}else{
				if(this->target->set_param(this->param, param))
					this->param = -1;
			}
		}
	}
}

void IO_Accessor::set_target(Module *target, int param){
	
	if(param >= target->get_param_count()){
		
		this->target = NULL;
		this->param = -1;
	}else{
		
		this->target = target;
		this->param = param;
	}
}

int IO_Accessor::is_dead() const{
	
	return this->param == -1 || this->target == NULL;
}