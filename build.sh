#!/bin/bash
echo "Software Space-FX : Compilation launched"

g++ ./source/SFX_Pi.cpp ./source/utility/utility.cpp ./source/fx/drive.cpp ./source/fx/Fx_Unit.cpp ./source/mod/LFO.cpp -I ./source `pkg-config --cflags --libs jack` -o ./SFX_Pi

echo "Compilation completed successfully"
