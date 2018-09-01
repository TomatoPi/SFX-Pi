#!/bin/sh

chown root.alarm /dev/gpiomem
chown root.alarm /dev/spidev0.0
chown root.alarm /dev/spidev0.1

chmod g+rw /dev/gpiomem
chmod g+rw /dev/spidev0.0
chmod g+rw /dev/spidev0.1

chmod 666 /dev/ttyUSB0
chmod 666 /dev/i2c-1

modprobe i2c-dev
modprobe i2c-bcm2708
