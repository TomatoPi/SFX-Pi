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

/*  
*   -----------------------------------------------------------------
*                     MCP's Register addresses
*   -----------------------------------------------------------------
*/
#define HEX_IODIRA 0x00 // IO configuration register
#define HEX_IODIRB 0x01 // 1:input 0:output
#define HEX_GPIOA 0x12  // Read status of an input
#define HEX_GPIOB 0x13
#define HEX_OLATA 0x14  // Write status of a pin
#define HEX_OLATB 0x15
#define HEX_IPOLA 0x02  // Reverse input polarity
#define HEX_IPOLB 0x03  // 1:reverse 0:normal

#define HEX_MCP_0 0x20  // First MCP23017 adress
#define HEX_MCP_1 0x24  // Second MCP23017 adress

class mcp23017
{
    public:
        mcp23017(void); // default constructor
        mcp23017(unsigned char dev_addr, std::string i2cfilename);
        //over loaded constructor
        ~mcp23017(void); // destructor
        int writeReg(unsigned char reg_addr, unsigned char data);
                // function to write byte data into a register of an I2C device
        int writeReg(unsigned char reg_addr, unsigned char mask, unsigned char data);
                // function to write data into a register masking unused values
        int readReg(unsigned char reg_addr, unsigned char &data);
                // function to read byte data from a register of an I2C device

    private:
        //private member functions
        int openI2C(); //open an I2C device. Called only in constructors
        int closeI2C(); // close an I2C device. Called only in destructor

        // private member variables
        std::string  i2cFileName; //i2c device name e.g."/dev/i2c-0" or "/dev/i2c-1"
                int i2cDescriptor;  // i2c device descriptor
        unsigned char deviceAddress; // i2c device address
};


#endif