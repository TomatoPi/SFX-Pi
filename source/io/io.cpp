#include "io.h"

const int IO_Potentiometer::D_NAME = 0x01;
const int IO_Potentiometer::D_VAL  = 0x02;
const int IO_Potentiometer::D_ALL  = 0x03;
        
void io_init_spi(){

	if (wiringPiSetup () == -1)
	  	exit (1) ;

	mcp3004Setup (SPI_BASE, SPI_CHAN1);
}

inline float io_map(float val, float fmin, float fmax, float tmin, float tmax){
	
	return ( ( ((val-fmin)/(fmax-fmin))*(tmax-tmin) ) + tmin );
}

void io_init_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT] ){
	
    // Set basic setup for potentiometers
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
        string name = "Potar-";
        name += to_string(i);
        
        pot[i] = IO_Potentiometer( i, name, 0, 999);
	}
    
    // Setup 6th potentiometer as Master volume Control
    
    const int vmin = -30;
    const int vmax = 10;
    
    pot[5] = IO_Potentiometer( 5, string("Volume"), vmin, vmax);
    
    pot[5].add_accessor( Accessor( BEGIN_NODE, END_RIGHT, vmin, vmax, CURVE_LIN, true, false ) );
    pot[5].add_accessor( Accessor( BEGIN_NODE, END_LEFT, vmin, vmax, CURVE_LIN, true, false ) );
    
    pot[5].add_accessor( Accessor( END_NODE, END_RIGHT, vmin, vmax, CURVE_LIN, true, false ) );
    pot[5].add_accessor( Accessor( END_NODE, END_LEFT, vmin, vmax, CURVE_LIN, true, false ) );
}

void io_update_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT], Module_Node_List* graph ){
	
	for(int i = 0; i < SPI_POTAR_COUNT; i++){
		
		pot[i].update( graph, IO_Potentiometer::D_ALL );
	}
}

IO_Potentiometer::IO_Potentiometer( int idx, string name, float min, float max ):
    name_(name),
    idx_( (idx>=0&&idx<SPI_POTAR_COUNT)?idx:-1 ),
    min_(min),
    max_(max)
{
    
    if ( idx_ != -1 ){
        
        value_ = analogRead(SPI_BASE + idx_);
    }
    else{
        
        cout << "Error : Bad Potar ID : " << idx << endl;
    }
}

void IO_Potentiometer::set_index( int idx ){
    
    idx_ = idx;
}

void IO_Potentiometer::set_name( string name ){
    
    name_ = name;
    name_.resize( 11, ' ' );
}

string IO_Potentiometer::get_name() const{
    
    return name_;
}

void IO_Potentiometer::set_plage( float min, float max ){
    
    min_ = min;
    max_ = max;
}

float IO_Potentiometer::get_min() const{
    
    return min_;
}

float IO_Potentiometer::get_max() const{
    
    return max_;
}

void IO_Potentiometer::update( Module_Node_List* graph , int disp){
    
    // Read potentiometer's value
    float value = analogRead(SPI_BASE + this->idx_);
	float diff = value - (float)value_;
    
    // If value has changed enougth
	if( (diff > SPI_HYSTERESIS * SPI_MAX && diff > 0) || (diff < -SPI_HYSTERESIS * SPI_MAX && diff < 0)){
	
        // Update value
		value_ = value;
        
        // Display new value if requested
        if ( !!disp ) this->display( disp );
        
        // Update accessor list
        for( vector<Accessor>::iterator itr = accs_.begin(); itr != accs_.end(); itr++ ){
            
            Accessor cr = *itr;
            
            Module* mod = NULL;
            bool ok = false;
            
            /*
            * Get pointer to target module
            */
            if ( graph->get( cr.target_) != NULL ){
             
                mod = graph->get( cr.target_ )->get_module();
            }
            else{
                
                cout << "Error Invalid Accessor target : " << cr.target_;
                return ;
            }
            
            if (ok ){
                
                // Get current param value
                float old = mod->get_param( cr.targetp_ );
                
                // Update it
                mod->set_param( cr.targetp_, cr.compute( io_map( value, 0, SPI_MAX, -1.0f, 1.0f ), old ) );
            }
        }
	}
}

float IO_Potentiometer::get_value() const{
    
    return io_map( value_, 0, SPI_MAX, -1, 1 );
}

void IO_Potentiometer::display( int flag ){
    
    if ( flag & IO_Potentiometer::D_NAME )
        IOS::printm( name_, IOS::DEFAULT_TEMP, IOS::TEMP );
    
    if ( flag & IO_Potentiometer::D_VAL )
        IOS::printm( to_string( io_map( value_, 0, SPI_MAX, min_, max_ ) ), IOS::DEFAULT_TEMP, IOS::SEG7 | IOS::TEMP );
}

void IO_Potentiometer::add_accessor( Accessor acc ){
    
    accs_.push_back( acc );
}

void IO_Potentiometer::del_accessor( int idx ){
    
    if ( idx < accs_.size() ){
        
        accs_.erase( accs_.begin() + idx );
    }
}

int IO_Potentiometer::get_accessor_count(){
    
    return accs_.size();
}

vector<Accessor> IO_Potentiometer::get_accessor(){
    
    return accs_;
}