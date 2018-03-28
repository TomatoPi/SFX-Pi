CXX=g++
CXXFLAGS= -g -Wall -Isrc/
LDFLAGS=-ljack -lpthread -ltinyxml2 -ldl -lSDL2 -Llib/ -lSDL2_ttf
LIBFLAGS= -fPIC -shared -ljack -Llib/
EXEC=SFX-Pi
vpath %.cc src/

ifndef RELEASE
	CXXFLAGS += -D__DEBUG__
else
	CXXFLAGS += -O3
endif

ifdef ARCH
	CXXFLAGS += -D__ARCH_LINUX__
	LDFLAGS +=  -lwiringPi
else
	CXXFLAGS += -D__LUBUNTU__
endif
	

### Shared Files For Plugins ###

SHARED_PLUGIN_SRC = $(addprefix effect/, JACKUnit.cc AbstractEffectUnit.cc)
export SHARED_PLUGIN_LIB = lib/plugin/JackPlugin.so

SHARED_LIB = $(SHARED_PLUGIN_LIB)

### Shared Files For SFX-PE ###

SFXP_PLUGIN_SRC  = $(addprefix plugin/, Plugin.cc TypeCodeTable.cc)
SFXP_PLUGIN_SRC += $(addprefix effect/, EffectParamSet.cc)

SFXP_PLUGIN_OBJ  = $(patsubst %.cc, obj/%.o, $(SFXP_PLUGIN_SRC))
export SFXP_PLUGIN_LIB  = lib/plugin/NonJackPlugin.so

SFXP_PRESET_SRC = preset/PresetParser.cc
SFXP_PRESET_OBJ = obj/preset/PresetParser.o
SFXP_PRESET_LIB = lib/preset/Preset.so

SFXP_EVENT_SRC = event/EventSequencer.cc
SFXP_EVENT_OBJ = obj/event/EventSequencer.o
SFXP_EVENT_LIB = lib/event/EventSequencer.so

SFXP_LOGIC_SRC = $(addprefix logic/, Footswitch.cc Led.cc)
SFXP_LOGIC_OBJ = $(patsubst %.cc, obj/%.o, $(SFXP_LOGIC_SRC))
SFXP_LOGIC_LIB = lib/logic/Logic.so

SFXP_ANALOG_SRC = analog/AnalogLink.cc
SFXP_ANALOG_OBJ = obj/analog/AnalogLink.o
SFXP_ANALOG_LIB = lib/analog/Analog.so

SFXP_LIB = $(SFXP_PLUGIN_LIB) $(SFXP_PRESET_LIB) $(SFXP_EVENT_LIB) $(SFXP_LOGIC_LIB) $(SFXP_ANALOG_LIB)
SFXP_SLIB = lib/lib-SFX-PE.a

### Libraries Specific for Plugins ###

export EXTERN_DRIVE_LIB = lib/drive/DriveBase.so
export EXTERN_BUFF_LIB	= lib/buffer/Buffer.so

EXTERN_FILTERB_SRC = lib/filter/FilterBase.cc
EXTERN_FILTER_LIB  = $(addprefix lib/filter/, GraphicEQ.so MonoPoleFilter.so MultibandEQ.so ParametricEQ.so) 

EXTERN_LIB = $(EXTERN_DRIVE_LIB) $(EXTERN_FILTER_LIB) $(EXTERN_BUFF_LIB)

### SFX-Pi Core ###

PI_CORE_SRC = $(addprefix core/, AbstractHandler.cc)
PI_CORE_OBJ = $(patsubst %.cc, obj/%.o, $(PI_CORE_SRC))

### Preset Handling ###

PI_PRESET_SRC = preset/PresetHandler.cc
PI_PRESET_OBJ = $(patsubst %.cc, obj/%.o, $(PI_PRESET_SRC))

### Effect Handling ###

PI_EFFECT_SRC = $(addprefix effect/, EffectFactory.cc EffectHandler.cc)
PI_EFFECT_OBJ = $(patsubst %.cc, obj/%.o, $(PI_EFFECT_SRC))

### Logic IO Handling ###

PI_LOGIC_SRC = $(addprefix logic/, LogicConfigLoader.cc mcp23017.cc LogicHandler.cc)
PI_LOGIC_OBJ = $(patsubst %.cc, obj/%.o, $(PI_LOGIC_SRC))

### Analog IO Handling ###

PI_ANALOG_SRC = $(addprefix analog/, AnalogHandler.cc Potentiometer.cc)
PI_ANALOG_OBJ = $(patsubst %.cc, obj/%.o, $(PI_ANALOG_SRC))

### Dynamic Event Handling ###

PI_EVENT_SRC = event/EventHandler.cc
PI_EVENT_OBJ = $(patsubst %.cc, obj/%.o, $(PI_EVENT_SRC))

### Console IO Handling ###

PI_COMMANDS_SRC = $(addprefix commands/list/, EffectHandlerCommands.cc PresetCommands.cc EventHandlerCommands.cc)
PI_COMMANDS_OBJ = $(patsubst %.cc, obj/%.o, $(PI_COMMANDS_SRC))

PI_CMDHANDLER_SRC = $(addprefix commands/, CommandHandler.cc CommandListener.cc Commands.cc)
PI_CMDHANDLER_OBJ = $(patsubst %.cc, obj/%.o, $(PI_CMDHANDLER_SRC))

PI_CONSOLE_SRC = $(PI_COMMANDS_SRC) $(PI_CMDHANDLER_SRC)
PI_CONSOLE_OBJ = $(PI_COMMANDS_OBJ) $(PI_CMDHANDLER_OBJ)

### Gui Handling ###

PI_GUICORE_SRC = $(addprefix gui/base/, GraphicObject.cc Container.cc Clickable.cc Layout.cc Dialog.cc Label.cc Button.cc InputBox.cc)
PI_GUICORE_OBJ = $(patsubst %.cc, obj/%.o, $(PI_GUICORE_SRC))

PI_GUICOMP_SRC = $(addprefix gui/comps/, main/MainContainer.cc menu/Menu.cc menu/MenuItem.cc)
PI_GUICOMP_OBJ = $(patsubst %.cc, obj/%.o, $(PI_GUICOMP_SRC))

PI_GUIHANDLER_SRC = $(addprefix gui/, GuiHandler.cc)
PI_GUIHANDLER_OBJ = $(patsubst %.cc, obj/%.o, $(PI_GUI_SRC))

PI_GUI_SRC = $(PI_GUIHANDLER_SRC) $(PI_GUICORE_SRC) $(PI_GUICOMP_SRC)
PI_GUI_OBJ = $(PI_GUIHANDLER_OBJ) $(PI_GUICORE_OBJ) $(PI_GUICOMP_OBJ)

### All Objects ###

SRC = $(PI_CORE_SRC) $(PI_PRESET_SRC) $(PI_EFFECT_SRC) $(PI_LOGIC_SRC) $(PI_ANALOG_SRC) $(PI_CONSOLE_SRC) $(PI_EVENT_SRC) $(PI_GUI_SRC)
OBJ = $(PI_CORE_OBJ) $(PI_PRESET_OBJ) $(PI_EFFECT_OBJ) $(PI_LOGIC_OBJ) $(PI_ANALOG_OBJ) $(PI_CONSOLE_OBJ) $(PI_EVENT_OBJ) $(PI_GUI_OBJ)

########################################################################
##   	                         RULES		                          ##
########################################################################
SFX-Pi: obj/SFXPi.o $(OBJ) $(SFXP_LIB) $(SHARED_LIB)
	$(CXX) -o $@ $^ $(LDFLAGS)

SFX-PE: $(SFXP_SLIB)

obj/SFXPi.o: SFXPi.cc $(SRC)

all: SFX-Pi lib plugins

lib: $(EXTERN_LIB) $(SFXP_LIB) $(SHARED_LIB)

plugins: $(EXTERN_LIB) $(SHARED_LIB)
	$(MAKE) -fMakeEffects

########################################################################

### Shared Files For Plugins ###
#	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^
$(SHARED_PLUGIN_LIB): $(SHARED_PLUGIN_SRC) $(SFXP_PLUGIN_LIB)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

### Shared Files For SFX-PE ###
#	ar crf $@ $^
$(SFXP_PLUGIN_OBJ): $(SFXP_PLUGIN_SRC)
$(SFXP_PLUGIN_LIB): $(SFXP_PLUGIN_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(SFXP_PRESET_OBJ): $(SFXP_PRESET_SRC)
$(SFXP_PRESET_LIB): $(SFXP_PRESET_SRC) $(SFXP_PLUGIN_LIB) $(SFXP_EVENT_LIB) $(SFXP_ANALOG_LIB)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(SFXP_EVENT_OBJ): $(SFXP_EVENT_SRC)
$(SFXP_EVENT_LIB): $(SFXP_EVENT_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(SFXP_LOGIC_OBJ): $(SFXP_LOGIC_SRC)
$(SFXP_LOGIC_LIB): $(SFXP_LOGIC_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(SFXP_ANALOG_OBJ): $(SFXP_ANALOG_SRC)
$(SFXP_ANALOG_LIB): $(SFXP_ANALOG_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(SFXP_SLIB): $(SFXP_ANALOG_OBJ) $(SFXP_EVENT_OBJ) $(SFXP_LOGIC_OBJ) $(SFXP_PLUGIN_OBJ) $(SFXP_PRESET_OBJ)
	ar crf $@ $^

### Libraries Specific for Plugins ###
$(EXTERN_DRIVE_LIB): lib/driveBase/DriveBase.cc
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(EXTERN_BUFF_LIB): lib/buffer/Buffer.cc
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

lib/filter/GraphicEQ.so: lib/filter/GraphicEQ.cc $(EXTERN_FILTERB_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

lib/filter/MonoPoleFilter.so: lib/filter/MonoPoleFilter.cc $(EXTERN_FILTERB_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

lib/filter/MultibandEQ.so: lib/filter/MultibandEQ.cc $(EXTERN_FILTERB_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

lib/filter/ParametricEQ.so: lib/filter/ParametricEQ.cc $(EXTERN_FILTERB_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

### SFX-Pi Core ###

### Preset Handling ###
$(PI_PRESET_OBJ): $(PI_PRESET_SRC) $(SFXP_PRESET_LIB)

### Effect Handling ###
$(PI_EFFECT_OBJ): $(PI_EFFECT_SRC) $(SHARED_PLUGIN_LIB)

### Logic IO Handling ###
$(PI_LOGIC_OBJ): $(PI_LOGIC_SRC) $(SFXP_LOGIC_LIB)

### Analog IO Handling ###
$(PI_ANALOG_OBJ): $(PI_ANALOG_SRC) $(SFXP_ANALOG_LIB)

### Dynamic Event Handling ###
$(PI_EVENT_OBJ): $(PI_EVENT_SRC) $(SFXP_EVENT_LIB)

### Console IO Handling ###
$(PI_COMMANDS_OBJ): $(PI_COMMANDS_SRC)

$(PI_CMDHANDLER_OBJ): $(PI_CMDHANDLER_SRC) $(PI_COMMANDS_SRC)

### Gui Handling ###
$(PI_GUIHANDLER_OBJ): $(PI_GUIHANDLER_SRC)

$(PI_GUICORE_OBJ): $(PI_GUICORE_SRC)

$(PI_GUICOMP_OBJ): $(PI_GUICOMP_SRC)

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
