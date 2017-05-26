#ifndef DEF_REGLAGES_H
#define DEF_REGLAGES_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>

#define BASE 100
#define SPI_CHAN1 0
#define SPI_CHAN2 1

void io_init_spi();

int io_get_param(int param);

#endif
