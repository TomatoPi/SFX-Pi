#include "io.h"

void io_init_spi(){

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

void io_get_potentiometer(int *potar_tab){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		potar_tab[i] = analogRead(SPI_BASE + i);
	}
}

IO_Accessor::IO_Accessor(Module *target, int target_voice, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv):
	target(target),
	target_voice(target_voice),
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
	this->state = target_param < target->get_voice(target_voice)->get_param_count();
}

int IO_Accessor::update(int *potar_tab){
	
	if(this->state){
		
		int value = potar_tab[this->potentiometer];
		if( ((float)value > (float)this->value * (1.0f + SPI_HYSTERESIS)) || ((float)value  < (float)this->value * (1.0f - SPI_HYSTERESIS)) ){
			
			this->value = value;
			if(this->is_inv)value = SPI_MAX - value;

			if(is_db){
				this->target->get_voice(this->target_voice)->set_param( this->target_param, spi_dbtorms( (((*this->curve)((float)value/(float)SPI_MAX))*(this->max - this->min)) + this->min ) );
			}else{
				this->target->get_voice(this->target_voice)->set_param( this->target_param, (((*this->curve)((float)value/(float)SPI_MAX))*(this->max - this->min)) + this->min );
			}
			return 1;
		}
		return 0;
	}
	return -1;
}

void IO_Accessor::set_target(Module *target, int param){
	
	if(param >= target->get_voice(this->target_voice)->get_param_count()){
		
		this->state = 0;
	}else{
		
		this->target = target;
		this->target_param = param;
	}
}

int IO_Accessor::is_dead() const{
	
	return !this->state;
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

void io_init_screen(){

	int i=0;
	int cport_nr=16; /* /dev/ttyUSB0 */
	int bdrate=57600; /* 9600 baud */

	char mode[]={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
	char str_send[BUF_SIZE]; // send data buffer
	strcpy(str_send, "Space-Fx!");

	if(RS232_OpenComport(cport_nr, bdrate, mode)){
		printf("Can not open comport\n");
		return;
	}

	usleep(2000000);  /* waits 2000ms for stable condition */

	RS232_cputs(cport_nr, str_send); // sends string on serial

}