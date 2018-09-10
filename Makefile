CXX=g++
CXXFLAGS= -g -Wall -Isrc/ -std=c++17 -std=gnu++17
LDFLAGS = -ltinyxml2 -ldl -Llib/
Pi_LDFLAGS= -ljack -lpthread
Pe_LDFLAGS= -lSDL2 -lSDL2_ttf
LIBFLAG= -fPIC -shared -ljack -Llib/
vpath %.cc src/

### Preprocessor defines ###

ifdef RELEASE
    CXXFLAGS += -O3 -DNDEBUG
endif

ifdef ARCH
    CXXFLAGS += -D__ARCH_LINUX__ -D__UNIX__
    LDFLAGS +=  -lwiringPi
else ifdef LUBUNTU
    CXXFLAGS += -D__LUBUNTU__ -D__UNIX__
else ifdef WINDOWS
    CXXFLAGS += -Iincludes/
else
    $(error Use make ARCH LUBUNTU or WINDOWS to compile for a specific plateform)
endif

########################################################################

### Modules ###
Mod_DIR = modules/
Mod_TAR = $(Mod_DIR)

Mod_Tempo_DIR = natifs/
Mod_Tempo_SRC = $(Mod_DIR)$(Mod_Tempo_DIR)TapTempo.cc
Mod_Tempo_LIB = $(Mod_TAR)$(Mod_Tempo_DIR)TapTempo.so

Mod_Disto_DIR = gain/
Mod_Disto_SRC = $(Mod_DIR)$(Mod_Disto_DIR)Distortion.cc
Mod_Disto_LIB = $(Mod_TAR)$(Mod_Disto_DIR)Distortion.so

Mod_Midi_DIR = midi/
Mod_Midi_SRC = $(Mod_DIR)$(Mod_Midi_DIR)Polysynth.cc
Mod_Midi_LIB = $(Mod_TAR)$(Mod_Midi_DIR)Polysynth.so

Mod_Temps_DIR = temps/
Mod_Temps_SRC = $(addprefix $(Mod_DIR)$(Mod_Temps_DIR), Delay.cc Chorus.cc)
Mod_Temps_LIB = $(Mod_TAR)$(Mod_Temps_DIR)Temps.so

Mod_Modulation_DIR = modulation/
Mod_Modulation_SRC = $(Mod_DIR)$(Mod_Modulation_DIR)LFO.cc
Mod_Modulation_LIB = $(Mod_TAR)$(Mod_Modulation_DIR)LFO.so

Mod_All_TAR = $(Mod_TAR) \
    $(Mod_TAR)$(Mod_Tempo_DIR)\
    $(Mod_TAR)$(Mod_Disto_DIR)\
    $(Mod_TAR)$(Mod_Midi_DIR) \
    $(Mod_TAR)$(Mod_Temps_DIR)\
    $(Mod_TAR)$(Mod_Modulation_DIR)

Mod_All_LIB = $(Mod_Tempo_LIB)\
    $(Mod_Disto_LIB)\
    $(Mod_Midi_LIB)\
    $(Mod_Temps_LIB)\
    $(Mod_Modulation_LIB)

### Librairies ###

Lib_DIR = lib/
Lib_OBJ = obj/lib/
Lib_TAR = lib/lib/

Lib_Buffer_DIR = buffer/
Lib_Buffer_SRC = $(Lib_DIR)$(Lib_Buffer_DIR)Buffer.cc
Lib_Buffer_LIB = $(Lib_TAR)$(Lib_Buffer_DIR)Buffer.so

Lib_Filter_DIR = filter/
Lib_Filter_Base_SRC = $(Lib_DIR)$(Lib_Filter_DIR)FilterBase.cc

Lib_Filter_GEQ_SRC = $(Lib_DIR)$(Lib_Filter_DIR)GraphicEQ.cc
Lib_Filter_GEQ_LIB = $(Lib_TAR)$(Lib_Filter_DIR)GraphicEQ.so

Lib_Filter_MPF_SRC = $(Lib_DIR)$(Lib_Filter_DIR)MonoPoleFilter.cc
Lib_Filter_MPF_LIB = $(Lib_TAR)$(Lib_Filter_DIR)MonoPoleFilter.so

Lib_Filter_MEQ_SRC = $(Lib_DIR)$(Lib_Filter_DIR)MultibandEQ.cc
Lib_Filter_MEQ_LIB = $(Lib_TAR)$(Lib_Filter_DIR)MultibandEQ.so

Lib_Filter_PEQ_SRC = $(Lib_DIR)$(Lib_Filter_DIR)ParametricEQ.cc
Lib_Filter_PEQ_LIB = $(Lib_TAR)$(Lib_Filter_DIR)ParametricEQ.so

Lib_Envelope_DIR = envelope/

Lib_Env_ADSR_SRC = $(Lib_DIR)$(Lib_Envelope_DIR)ADSR.cc
Lib_Env_ADSR_LIB = $(Lib_TAR)$(Lib_Envelope_DIR)ADSR.so

Lib_All_TAR = $(Lib_TAR) \
    $(Lib_TAR)$(Lib_Buffer_DIR) \
    $(Lib_TAR)$(Lib_Filter_DIR) \
    $(Lib_TAR)$(Lib_Envelope_DIR) \
    $(Lib_OBJ)

Lib_All_LIB = $(Lib_Buffer_LIB) \
    $(Lib_Filter_GEQ_LIB) \
    $(Lib_Filter_MPF_LIB) \
    $(Lib_Filter_MEQ_LIB) \
    $(Lib_Filter_PEQ_LIB) \
    $(Lib_Env_ADSR_LIB) \
    $(Lib_XML_LIB)

### Noyau ###
Ker_DIR = noyau/
Ker_TAR = lib/noyau/
ker_OBJ = obj/noyau/

Ker_Module_DIR = modules/
Ker_Module_lib_SRC = $(addprefix $(Ker_DIR)$(Ker_Module_DIR), ModuleBase.cc)
Ker_Module_lib_LIB = $(Ker_TAR)$(Ker_Module_DIR)ModuleBase.so

Ker_Module_load_SRC = $(addprefix $(Ker_DIR)$(Ker_Module_DIR), ModuleLoader.cc ModulePreset.cc)
Ker_Module_load_OBJ = $(patsubst %.cc, obj/%.o, $(Ker_Module_load_SRC))

Ker_All_SRC = $(Ker_Module_lib_SRC) $(Ker_Module_load_SRC)
ker_All_OBJ = $(Ker_Module_load_OBJ)
Ker_All_LIB = $(Ker_Module_lib_LIB)
Ker_All_TAR = $(Ker_TAR)$(Ker_Module_DIR) \
    $(ker_OBJ)$(Ker_Module_DIR)

### SFX-Pi ###

SFX_Pi_DIR = process/

Pi_GPIO_DIR = $(SFX_Pi_DIR)gpio/
Pi_GPIO_SRC = $(addprefix $(Pi_GPIO_DIR), Footswitch.cc mcp23017.cc)
Pi_GPIO_OBJ = $(patsubst %.cc, obj/%.o, $(Pi_GPIO_SRC))

SFX_Pi_All_TAR = obj/$(Pi_GPIO_DIR)

### SFX-Pe ###

### All ###
Pi_DIR = obj/ $(Mod_All_TAR) $(Ker_All_TAR) $(Lib_All_TAR) $(SFX_Pi_All_TAR)
Pi_SRC = $(Ker_All_SRC) $(Pi_GPIO_SRC)
Pi_OBJ = $(ker_All_OBJ) $(Pi_GPIO_OBJ)
Pi_LIB = $(Ker_All_LIB)

Pe_DIR = obj/ $(Mod_All_TAR) $(Ker_All_TAR) $(Lib_All_TAR)
Pe_SRC = $(Ker_All_SRC)
Pe_OBJ = $(ker_All_OBJ)
Pe_LIB = $(Ker_All_LIB)

########################################################################
##   	                         RULES		                      ##
########################################################################
Modules : Pi_dirs $(Mod_All_LIB) $(Lib_All_LIB)
	
SFX-Pi: obj/SFXPi.o $(Pi_OBJ) $(Pi_LIB)
	$(CXX) -o $@ $^ $(LDFLAGS) $(Pi_LDFLAGS)

SFX-PE: obj/SFXPe.o $(Pe_OBJ) $(Pe_LIB)
	$(CXX) -o $@ $^ $(LDFLAGS) $(Pe_LDFLAGS)
	
obj/SFXPi.o: Pi_dirs SFXPi.cc $(Pi_SRC)
obj/SFXPe.o: Pe_dirs SFXPe.cc $(Pe_SRC)

Pi_dirs : 
	mkdir -p $(Pi_DIR)
Pe_dirs : 
	mkdir -p $(Pe_DIR)

########################################################################

###### Noyau ######

$(Ker_Module_lib_LIB) : $(Ker_Module_lib_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
$(Ker_Module_load_OBJ): $(Ker_Module_load_SRC)

###### Modules ######

$(Mod_Tempo_LIB) : $(Mod_Tempo_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
$(Mod_Disto_LIB) : $(Mod_Disto_SRC) $(Lib_Filter_GEQ_SRC) src/noyau/modules/ModuleBase.h src/noyau/utils.h
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^ $(Lib_Filter_GEQ_LIB)
	
$(Mod_Midi_LIB) : $(Mod_Midi_SRC) $(Lib_Env_ADSR_SRC) src/noyau/modules/ModuleBase.h src/noyau/utils.h
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^ $(Lib_Env_ADSR_LIB)
	
$(Mod_Temps_LIB) : $(Mod_Temps_SRC) $(Lib_Buffer_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^ $(Lib_Buffer_LIB)
	
$(Mod_Modulation_LIB) : $(Mod_Modulation_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
###### Librairies ######

## Buffer ##
$(Lib_Buffer_LIB) : $(Lib_Buffer_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
## Filter ##
$(Lib_Filter_GEQ_LIB) : $(Lib_Filter_GEQ_SRC) $(Lib_Filter_Base_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
$(Lib_Filter_MPF_LIB) : $(Lib_Filter_MPF_SRC) $(Lib_Filter_Base_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
$(Lib_Filter_MEQ_LIB) : $(Lib_Filter_MEQ_SRC) $(Lib_Filter_Base_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
$(Lib_Filter_PEQ_LIB) : $(Lib_Filter_PEQ_SRC) $(Lib_Filter_Base_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
## ADSR ##
$(Lib_Env_ADSR_LIB) : $(Lib_Env_ADSR_SRC)
	$(CXX) $(LIBFLAG) $(CXXFLAGS) -o $@ $^
	
## XML ##
$(Lib_XML_OBJ): $(Lib_XML_SRC)
	
$(Lib_XML_LIB): $(Lib_XML_OBJ)
	ar crf $@ $^
	
###### SFX-Pi ######

$(Pi_GPIO_OBJ): $(Pi_GPIO_SRC)

###### SFX-Pe ######

########################################################################
##                          Genaral Rules                             ##
########################################################################

obj/%.o: %.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

%.a: %.o
	ar crf $@ $^

%.so: %.cc
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm --recursive -v -rf obj/*.o

mrproper:
	rm -rf $(EXEC)