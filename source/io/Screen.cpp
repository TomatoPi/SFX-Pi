#include "io.h"

namespace {
        
    int 	io_screen_port 	= 16; 				/* /dev/ttyUSB0 */
    int 	io_screen_bdrate = 57600;			/* 9600 baud */
    char 	io_screen_mode[]={'8','N','1',0};	/* 8 data bits, no parity, 1 stop bit */
    
    bool IOS_OK = false;
}

void IOS::init_screen( string msg, string version ){
    
    cout << "Open connection with arduino -- ";
	
	if(RS232_OpenComport(io_screen_port, io_screen_bdrate, io_screen_mode)){
		cout << "Can\'t open comport" << endl;
		return;
	}
	
	usleep(2000000);  			/* waits 2000ms for stable condition */
    
    cout << "Connection ok !" << endl;
    IOS_OK = true;
	
    IOS::printm( string("Hy Tomato"), 3000, IOS::CLEAR );
	IOS::printm( msg , IOS::STACK);
    IOS::printm( version, IOS::SEG7 );
}

void IOS::printm( string msg ){
	
    printm( msg, 0, IOS::OVERIDE );
}

void IOS::printm( string msg, int flag ){
    
    printm( msg, 0, flag );
}

void IOS::printm( string msg, int t , int flag){
    
    if ( IOS_OK ){
        
        string buffer = "[";
        
        /*
        * Parse flags
        */
        
        // If message for 7 Segment display
        if ( flag & IOS::SEG7 ){
            
            msg.resize( 4, ' ' );
            
            buffer += "7[";
            buffer += msg;
            buffer += "]]";
        }
        // Else if switch screen Message
        else if ( flag & IOS::NEXT ){
            
            buffer += "fn]";
        }
        else if ( flag & IOS::PREV ){
            
            buffer += "fp]";
        }
        else if ( flag & IOS::FORCE ){
            
            buffer += "f[";
            buffer += to_string( (int)t );
            buffer += "]]";
        }
        // Else message for 14 Segment display
        else{
            
            // Clear screen's buffer
            if ( flag & IOS::CLEAR ){
                
                buffer += 'c';
            }
            
            // If Temp message
            if ( flag & IOS::TEMP ){
                
                buffer += 't';
            }
            // Else normal message
            else{
            
                // Prevent have stack and OVERIDE flags at the same time
                if ( flag & IOS::STACK ){
                    
                    buffer += 's';
                }
                else if ( flag & IOS::OVERIDE ){
                    
                    buffer += 'o';
                }
            }
            
            // If a duration have been set
            if ( t > 0 ){
                
                buffer += "d[";
                buffer += to_string( (int)t );
                buffer += "]";
            }
            
            // Finaly add the message
            msg.resize( 11, ' ' );
            buffer += "m[";
            buffer += msg;
            buffer += "]]";
        }
        
        /*
        *	Send data
        */
        cout << "New Message sent : " << buffer << endl;
        RS232_cputs(io_screen_port, buffer.c_str());
    }
    else{
        
        cout << "Failed send message : Screen not OK" << endl;
    }
}