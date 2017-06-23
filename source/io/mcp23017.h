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

#define HEX_GPIOA 0x12
#define HEX_GPIOB 0x13
#define HEX_OLATA 0x14
#define HEX_OLATB 0x15

#define HEX_MCP_0 0x20

class mcp23017
{
    public:
        mcp23017(void); // default constructor
        mcp23017(unsigned char dev_addr, std::string i2cfilename);
        //over loaded constructor
        ~mcp23017(void); // destructor
        int writeReg(unsigned char reg_addr, unsigned char data);
                // function to write byte data into a register of an I2C device
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