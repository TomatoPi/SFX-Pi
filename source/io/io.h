#ifndef DEF_IO_H
#define DEF_IO_H

#include <stdio.h>
#include <unistd.h>
#include "rs232.h"
#include <bits/stdc++.h>

#include "../consts.h"

namespace IOS{
    
    static const int SEG7   = 0x01; //**< Given message will be displayed on the 7Segment display */
    static const int STACK  = 0x02; //**< Given message will be stacked in screen's buffer */
    static const int OVERIDE= 0x04; //**< Given message will replace current one */
    static const int CLEAR  = 0x08; //**< Clear screen's messages buffer */
    static const int NEXT   = 0x10; //**< Force screen to switch to next message */
    static const int PREV   = 0x20; //**< Force screen to switch to previous message */
    static const int FORCE  = 0x40; //**< Force screen to switch to given message ( index given instead of time ) */
    static const int TEMP   = 0x80; //**< Given message will be a temp message */
    
    static const int DEFAULT_TEMP = 2000;
    
    /**
    * Open connection with arduino
    * @param msg Splash message sent to arduino
    * @param version current program version printed to 7Seg display
    */
    void init_screen();
    
    /**
    * Send given message to the 14Segment display.
    * Given message will be displayed instantly to screen and will erase current message buffer
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    */
    void printm( std::string msg );
    /**
    * Send given message to the 14Segments display
    * Given message will be send with given flags, viewing delay will be set to forever
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    * @param flag special params for parsing and viewing message
    */
    void printm( std::string msg, int flag );
    /**
    * Send given message to the 14Segments display
    * Given message will be send with given flags, and given viewing delay
    * The message is truncated if longer than 11 characters length
    * @param msg message to print
    * @param t viewing time in ms ( 0 means forever )
    * @param flag special params for parsing and viewing message
    */
    void printm( std::string msg, int t , int flag);
}

#endif
