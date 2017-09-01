#!/bin/bash
echo "Software Space-FX : Compilation launched"

g++ -g -Wall -Wunused-variable -Wwrite-strings ./source/SFX_Pi.cpp ./source/presset/Presset.cpp ./source/io/rs232.c ./source/io/mcp23017.cpp ./source/io/io.cpp ./source/io/FrontPanel.cpp ./source/core/Utility.cpp ./source/core/Buffer.cpp ./source/core/Filter.cpp ./source/core/Modules.cpp ./source/core/ModulesGraph.cpp ./source/core/Accessor.cpp ./source/modules/Delay.cpp ./source/modules/Drive.cpp ./source/modules/LFO.cpp ./source/modules/Ringmod.cpp ./source/modules/Reverb.cpp ./source/modules/Tonestack.cpp ./source/modules/Chorus.cpp ./source/modules/EndModule.cpp ./source/modules/Compressor.cpp -I ./source `pkg-config --cflags --libs jack` -lwiringPi -o ./SFX_Pi

echo "Compilation completed successfully"