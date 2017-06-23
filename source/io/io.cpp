#include "io.h"

void io_init_spi(){

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

inline float io_map(float val, float fmin, float fmax, float tmin, float tmax){
	
	return ( ( ((val-fmin)/(fmax-fmin))*(tmax-tmin) ) + tmin );
}

/*
*	---------------------------------------------------------------------------
*	IO Potentiometer stuff
*	---------------------------------------------------------------------------
*/
IO_Potentiometer::IO_Potentiometer(int idx, char* name, float min, float max): idx_(idx), min_(min), max_(max){
	
	this->value_ = analogRead(SPI_BASE + idx);
	stpcpy(this->name_, name);
}

void IO_Potentiometer::set_name(char* name){
	
	strcpy(this->name_, name);
}

void IO_Potentiometer::set_plage(float min, float max){
	
	this->min_ = min;
	this->max_ = max;
	this->update();
}

void IO_Potentiometer::update(){
	
	float value = analogRead(SPI_BASE + this->idx_);
	float diff = (float)value - (float)this->value_;
	if( (diff > SPI_HYSTERESIS * SPI_MAX && diff > 0) || (diff < -SPI_HYSTERESIS * SPI_MAX && diff < 0)){
	
		this->value_ = value;
		float display = io_map((float)value, 0.0f, SPI_MAX, this->min_, this->max_);
		
		char p[BUF_SIZE] = {};
		sprintf(p, "%s %d", this->name_, (int)display);
		MAIN_SCREEN->print(p, 1);
	}
}

float IO_Potentiometer::get_value() const{
	
	return this->value_;
}

void io_init_potar_tab(IO_Potentiometer **pot){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		char n[BUF_SIZE];
		sprintf(n, "Potar-%d", i);
		pot[i] = new IO_Potentiometer(i, n, 0, 999);
	}
}

void io_update_potar_tab(IO_Potentiometer **pot, int *tab){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		tab[i] = analogRead(SPI_BASE + i);
		pot[i]->update();
	}
}

/*
*	---------------------------------------------------------------------------
*	IO Accessor stuff
*	---------------------------------------------------------------------------
*/
IO_Accessor::IO_Accessor(Module *module, Module_voice *target, int target_idx, int target_voice, int target_param, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv):
	module_(module),
	curve_type(curve),
	target(target),
	target_idx(target_idx),
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
	this->state = target_param < target->get_param_count();
}

int IO_Accessor::update(int *potar_tab){
	
	if(this->state && this->target != NULL){
		
		int value = potar_tab[this->potentiometer];
		float diff = (float)value - (float)this->value;
		if( (diff > SPI_HYSTERESIS * SPI_MAX && diff > 0) || (diff < -SPI_HYSTERESIS * SPI_MAX && diff < 0)){
			
			this->value = value;
			if(this->is_inv)value = SPI_MAX - value;
			
			float param = (((*this->curve)((float)value/SPI_MAX))*(this->max - this->min)) + this->min;

			if(is_db) param = spi_dbtorms(param);
			
			this->target->set_param(this->target_param, param);
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

int IO_Accessor::get_target_voice() const{
	
	return this->target_voice;
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

/*
*	---------------------------------------------------------------------------
*	Screen stuff
*	---------------------------------------------------------------------------
*/

IO_screen::IO_screen():time_(time(NULL)), fix_(-1){
	
	char str_send[BUF_SIZE]; 				/* send data buffer */
	strcpy(str_send, "Space-Fx");

	if(RS232_OpenComport(io_screen_port, io_screen_bdrate, io_screen_mode)){
		printf("Can\'t open comport\n");
		return;
	}
	
	usleep(2000000);  			/* waits 2000ms for stable condition */
	
	this->print(str_send, 2); 	/* sends string on serial */
}

void IO_screen::print(const char* text, int t){
	
	/*
	*	Clear the buffer and write text
	*/
	char buffer[BUF_SIZE] = {};
	
	fprintf(stderr, "Received message : [%s] -- ", text);
	sprintf(buffer, "%s!", text);
	fprintf(stderr, "Message : [%s]\n", buffer);
	
	/*
	*	Send data
	*/
	RS232_cputs(io_screen_port, buffer);
	
	/*
	*	Set timer
	*/
	//this->time_ = time(NULL);
	//this->fix_ = t;
}

void IO_screen::update(){
	
	if( difftime(time(NULL), this->time_) > this->fix_ && this->fix_ != -1){
		
		RS232_cputs(io_screen_port, io_screen_default);
		this->time_ = time(NULL);
		this->fix_ = -1;
	}
}