#!/bin/bash
echo "Software Space-FX : Compilation launched"

g++ ./source/SFX_Pi.cpp ./source/core/Utility.cpp ./source/core/Module.cpp ./source/modules/Drive.cpp ./source/modules/LFO.cpp -I ./source `pkg-config --cflags --libs jack` -o ./SFX_Pi

echo "Compilation completed successfully"
