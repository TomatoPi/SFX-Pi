#!/bin/bash
echo "Software Space-FX : Compilation launched"

g++ ./source/SFX_Pi.cpp ./source/io/io.cpp ./source/core/Utility.cpp ./source/core/Ringbuffer.cpp ./source/core/Modules.cpp ./source/modules/Delay.cpp ./source/modules/Drive.cpp ./source/modules/LFO.cpp ./source/modules/Ringmod.cpp ./source/modules/Tonestack.cpp -I ./source `pkg-config --cflags --libs jack` -lwiringPi -o ./SFX_Pi

echo "Compilation completed successfully"
