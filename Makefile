CXX=g++
CXXFLAGS=-g -Wall -Wunused-variable -Wwrite-strings `pkg-config --cflags --libs jack` -lwiringPi
LDFLAGS=
EXEC=SFX_Pi
CORE= $(shell find ./source/core -name "*.cpp")
IO= $(shell find ./source/io/ -name "*.cpp) $(shell find ./source/io/menu -name "*.cpp") 
MODULES= $(shell find ./source/modules -name "*.cpp")
PRESSET= $(shell find ./source/presset -name "*.cpp")
SRC= $(CORE) $(IO) $(MODULES) $(PRESSET)
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)

SFX_Pi: $(OBJ) SFX_Pi.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

SFX_Pi.o : $(OBJ)

%.o: %.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
