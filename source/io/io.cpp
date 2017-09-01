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
*	Screen stuff
*	---------------------------------------------------------------------------
*/
static int 		io_screen_port 	= 16; 				/* /dev/ttyUSB0 */
static int 		io_screen_bdrate = 57600;			/* 9600 baud */
static char 	io_screen_mode[]={'8','N','1',0};	/* 8 data bits, no parity, 1 stop bit */

static const char* io_screen_default = "Space-Fx--!";

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
	
	//fprintf(stderr, "Received message : [%s] -- ", text);
	sprintf(buffer, "%s!", text);
	//fprintf(stderr, "Message : [%s]\n", buffer);
	
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