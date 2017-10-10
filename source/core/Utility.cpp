#include "Utility.h"

string f_ftos(float f){
    
    string s = string("");
    
    if( f < 0.0f ){
        
        f *= -1;
        s += "-";
        
    }else{
        
        s += " ";
    }
    
    if( f < 0.1f ){
        
        s += "f0";
        s += to_string( (int)(f*100) );
        
    }else if ( f < 1.0f ){
        
        s += "f";
        s += to_string( (int)(f*100) );
        
    }else if ( f < 10.0f ){
        
        s += to_string( (int)f );
        s += "f";
        s += to_string( (int)((f - (int)(f))*10) );
        
    }else if ( f < 100 ){
        
        s += to_string( (int)(f) );
        s += "f";
        
    }else if ( f < 1000 ){
        
        s += to_string( (int)(f) );
        
    }else if ( f < 10000 ){
        
        s += to_string( ((int)f - ((int)f % 1000)) / 1000 );
        s += "k";
        s += to_string( ((int)f%1000) / 100 );
        
    }else if ( f < 100000 ){
        
        s += to_string( ((int)f - ((int)f % 1000)) / 1000 );
        s += "k";
    }else{
        
        s+= "xxx";
    }
    
    return s;
}
