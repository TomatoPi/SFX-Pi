CXX=g++
CXXFLAGS= -g -O3
LDFLAGS=-ljack
EXEC=SFX-Pi

SOURCE=src/

## Module Core Files ##
MODULE_DIR= modules/
MODULE_CORE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)core/, AbstractEffectUnit.cc JACKUnit.cc)
MODULE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR), UnitFactory.cc)

MODULE_CORE_OBJ = $(MODULE_CORE_SRC:.cc=.o)
MODULE_OBJ= $(MODULE_SRC:.cc=.o)

## Effects Dependencies Files ( Filter / buffer ) ##
FILTER_DIR= core/filter/
FILTER_CORE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(FILTER_DIR), FilterBase.cc)
FILTER_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(FILTER_DIR), MonoPoleFilter.cc GraphicEQ.cc ParametricEQ.cc)

FILTER_CORE_OBJ= $(FILTER_CORE_SRC:.cc=.o)
FILTER_OBJ= $(FILTER_SRC:.cc=.o)

EFFECT_DEP_SRC = $(FILTER_CORE_SRC) $(FILTER_SRC)
EFFECT_DEP_OBJ = $(EFFECT_DEP_SRC:.cc=.o)

## Effects Files ##
EFFECT_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)effects/, Drive.cc )
EFFECT_OBJ= $(EFFECT_SRC:.cc=.o)

## Listing of All Files ##
SRC= $(MODULE_CORE_SRC) $(MODULE_SRC) $(FILTER_CORE_SRC) $(FILTER_SRC) $(EFFECT_SRC)
OBJ= $(SRC:.cc=.o)

##########################
##   	   RULES		##
##########################
all: SFX-Pi

SFX-Pi: $(OBJ) $(SOURCE)SFX-Pi.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SOURCE)SFX-Pi.o: $(SOURCE)SFX-Pi.cc $(SRC)

## Build Modules Core and Factory ##
$(MODULE_CORE_OBJ): $(MODULE_CORE_SRC)

$(MODULE_OBJ): $(MODULE_CORE_SRC) $(MODULE_SRC)

## Build Effect's Dependencies ##
$(FILTER_CORE_OBJ): $(FILTER_CORE_SRC)

$(FILTER_OBJ): $(FILTER_CORE_SRC) $(FILTER_SRC)

## Build Effects from EffectCore and Effect Dependencies ##
$(EFFECT_OBJ): $(EFFECT_SRC) $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC)

%.o: %.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm --recursive -v -rf ./src/*.o

mrproper:
	rm -rf $(EXEC)
