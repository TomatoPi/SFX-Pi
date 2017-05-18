all: SFX_Pi.o Drive.o LFO.o Utility.o Modules.o
	g++ SFX_Pi.o Drive.o LFO.o Utility.o Modules.o `pkg-config --cflags --libs jack` -o ./SFX_Pi
	
SFX_Pi.o: SFX_Pi.cpp Drive.h LFO.h
	g++ ./source/modules/Drive.cpp `pkg-config --cflags --libs jack` -o SFX_Pi.o
	
Drive.o: ./source/modules/Drive.cpp ./source/core/Utility.h ./source/core/Modules.h
	g++ ./source/modules/Drive.cpp `pkg-config --cflags --libs jack` -o Drive.o
	
LFO.o: ./source/modules/LFO.cpp ./source/core/Utility.h ./source/core/Modules.h
	g++ ./source/modules/LFO.cpp `pkg-config --cflags --libs jack` -o LFO.o
	
Modules.o: ./sources/core/Modules.cpp
	g++ ./sources/core/Modules.cpp `pkg-config --cflags --libs jack` -o Modules.o
	
Utility.o: ./sources/core/Utility.cpp
	g++ ./sources/core/Utility.cpp `pkg-config --cflags --libs jack` -o Utility.o
