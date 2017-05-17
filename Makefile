all: SFX_Pi.o Fx_Unit.o LFO.o
	g++ SFX_Pi.o Fx_Unit.o LFO.o `pkg-config --cflags --libs jack` -o ./SFX_Pi
	
SFX_Pi.o: Fx_Unit.cpp LFO.cpp
	g++ ./source/fx/Fx_Unit.cpp ./source/mod/LFO.cpp `pkg-config --cflags --libs jack` -o SFX_Pi.o
	
Fx_Unit.o: ./source/fx/Fx_Unit.cpp ./source/utility/utility.cpp
