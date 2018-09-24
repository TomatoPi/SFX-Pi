#ifndef DEF_MCP23017_H
#define DEF_MCP23017_H

#ifndef __SFX_PI__
#error This File is Specific To Processing Environement
#endif

#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <exception>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "noyau/types.h"
#include "noyau/log.h"

class mcp23017
{
    public :
        
        static const sfx::hex_t HEX_IODIRA = 0x00; // IO configuration register
        static const sfx::hex_t HEX_IODIRB = 0x01; // 1:input 0:output
        
        static const sfx::hex_t HEX_IPOLA = 0x02;  // Reverse input polarity
        static const sfx::hex_t HEX_IPOLB = 0x03;  // 1:reverse 0:normal
        
        static const sfx::hex_t HEX_GPIOA = 0x12;  // Read status of an input
        static const sfx::hex_t HEX_GPIOB = 0x13;
        static const sfx::hex_t HEX_OLATA = 0x14;  // Write status of a pin
        static const sfx::hex_t HEX_OLATB = 0x15;

        static const sfx::hex_t HEX_MCP_0 = 0x20;  // First MCP23017 adress
        static const sfx::hex_t HEX_MCP_1 = 0x24;  // Second MCP23017 adress

        /**
         * Function used to change given bits inside a sequence
         **/
        static inline sfx::hex_t changeBit(sfx::hex_t& reg, sfx::hex_t val, sfx::hex_t mask){

            return reg = (reg & (~mask)) | (val & mask);
        }
        
        struct Adress
        {
            Adress(sfx::hex_t reg, bool ab, sfx::hex_t adr):reg(reg), adr(adr), ab(ab)
            {
            }
            Adress():Adress(0,0,0)
            {
            }
            sfx::hex_t reg, adr;
            bool ab;
        };
        
    public :
    
        mcp23017(void); // default constructor
        mcp23017(sfx::hex_t dev_addr, std::string i2cfilename);
        //over loaded constructor
        ~mcp23017(void); // destructor
        int writeReg(sfx::hex_t reg_addr, sfx::hex_t data) const;
                // function to write byte data into a register of an I2C device
        int writeReg(sfx::hex_t reg_addr, sfx::hex_t mask, sfx::hex_t data) const;
                // function to write data into a register masking unused values
        int readReg(sfx::hex_t reg_addr, sfx::hex_t &data) const;
                // function to read byte data from a register of an I2C device
        
        const sfx::hex_t& adress() const { return this->deviceAddress; }
        const bool& errored() const { return this->err; }

    private :
    
        //private member functions
        int openI2C(); //open an I2C device. Called only in constructors
        int closeI2C(); // close an I2C device. Called only in destructor

        // private member variables
        std::string  i2cFileName; //i2c device name e.g."/dev/i2c-0" or "/dev/i2c-1"
                int i2cDescriptor;  // i2c device descriptor
        sfx::hex_t deviceAddress; // i2c device address
        bool err;
};
        
static inline bool operator<(const mcp23017::Adress& a, const mcp23017::Adress& b)
{
    return a.reg < b.reg || a.ab < b.ab || a.adr < b.adr;
}

#endif
