#ifndef DEF_MCP23017_H
#define DEF_MCP23017_H

#include <string>
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <iostream>

#include "types.h"
#include "log.h"

class mcp23017
{
    public :
        
        static const SFXP::hex_t HEX_IODIRA = 0x00; // IO configuration register
        static const SFXP::hex_t HEX_IODIRB = 0x01; // 1:input 0:output
        
        static const SFXP::hex_t HEX_IPOLA = 0x02;  // Reverse input polarity
        static const SFXP::hex_t HEX_IPOLB = 0x03;  // 1:reverse 0:normal
        
        static const SFXP::hex_t HEX_GPIOA = 0x12;  // Read status of an input
        static const SFXP::hex_t HEX_GPIOB = 0x13;
        static const SFXP::hex_t HEX_OLATA = 0x14;  // Write status of a pin
        static const SFXP::hex_t HEX_OLATB = 0x15;

/* 
 * Deprecated Now loaded from xml file
        static const SFXP::hex_t HEX_MCP_0 = 0x20;  // First MCP23017 adress
        static const SFXP::hex_t HEX_MCP_1 = 0x24;  // Second MCP23017 adress
*/

        /**
         * Function used to change given bits inside a sequence
         **/
        static inline SFXP::hex_t changeBit(SFXP::hex_t& reg, SFXP::hex_t val, SFXP::hex_t mask){

            return reg = (reg & (~mask)) | (val & mask);
        }
        
    public :
    
        mcp23017(void); // default constructor
        mcp23017(SFXP::hex_t dev_addr, std::string i2cfilename);
        //over loaded constructor
        ~mcp23017(void); // destructor
        int writeReg(SFXP::hex_t reg_addr, SFXP::hex_t data);
                // function to write byte data into a register of an I2C device
        int writeReg(SFXP::hex_t reg_addr, SFXP::hex_t mask, SFXP::hex_t data);
                // function to write data into a register masking unused values
        int readReg(SFXP::hex_t reg_addr, SFXP::hex_t &data);
                // function to read byte data from a register of an I2C device

    private :
    
        //private member functions
        int openI2C(); //open an I2C device. Called only in constructors
        int closeI2C(); // close an I2C device. Called only in destructor

        // private member variables
        std::string  i2cFileName; //i2c device name e.g."/dev/i2c-0" or "/dev/i2c-1"
                int i2cDescriptor;  // i2c device descriptor
        SFXP::hex_t deviceAddress; // i2c device address
};

#endif
