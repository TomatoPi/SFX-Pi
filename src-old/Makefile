CXX=g++
CXXFLAGS= -g -O3 -Wunused-variable -Wall -Wwrite-strings -Wreturn-type -Isrc/
LDFLAGS=-ljack -lpthread -ltinyxml2 -ldl -Llib/
LIBFLAGS= -fPIC -shared -ljack -Llib/
EXEC=SFX-Pi
vpath %.cc src/

ifndef RELEASE
	CXXFLAGS += -D__DEBUG__
endif

### Shared Files For Plugins ###
PLUGIN_SRC = $(addprefix lib/, plugin/Plugin.cc unit/AbstractEffectUnit.cc unit/JACKUnit.cc unit/EffectPrototype.cc)
PLUGIN_LIB = lib/Plugin.so

FILTER_SRC = $(addprefix lib/filter/, FilterBase.cc GraphicEQ.cc MonoPoleFilter.cc MultibandEQ.cc ParametricEQ.cc)
FILTER_LIB = $(patsubst %.cc, %.so, $(FILTER_SRC))

DRIVE_SRC = $(addprefix lib/driveBase/, DriveBase.cc)
DRIVE_LIB = $(patsubst %.cc, %.so, $(DRIVE_SRC))

### Shared Files For SFX-PE ###
PLGLOAD_SRC = $(addprefix lib/plugin/, TypeCodeTable.cc PluginConfigLoader.cc)
PLGLOAD_OBJ = $(patsubst %.cc, obj/%.o, $(PLGLOAD_SRC))
PLGLOAD_LIB = lib/plugin/PluginLoading.a

PRESET_SRC = $(addprefix lib/preset/, PresetParser.cc Preset.cc)
PRESET_OBJ = $(patsubst %.cc, obj/%.o, $(PRESET_SRC))
PRESET_LIB = lib/preset/Preset.a

IOCORE_SRC = $(addprefix lib/io/, Notification.cc)
IOCORE_OBJ = $(patsubst %.cc, obj/%.o, $(IOCORE_SRC))

CMDLIB_SRC = lib/io/command/CommandSequencer.cc
CMDLIB_OBJ = $(patsubst %.cc, obj/%.o, $(CMDLIB_SRC))

LOGLIB_SRC = $(addprefix lib/io/logic/, LED.cc FootSwitch.cc)
LOGLIB_OBJ = $(patsubst %.cc, obj/%.o, $(LOGLIB_SRC))

IOLIB_SRC = $(IOCORE_SRC) $(CMDLIB_SRC) $(LOGLIB_SRC)
IOLIB_OBJ = $(IOCORE_OBJ) $(CMDLIB_OBJ) $(LOGLIB_OBJ)
IOLIB_LIB = lib/io/io.a

### Libraries ###
SHARED_LIB = $(PLGLOAD_LIB) $(PRESET_LIB) $(IOLIB_LIB)
SFXPI_LIB  = $(PLUGIN_LIB)
EXTERN_LIB = $(FILTER_LIB) $(DRIVE_LIB)

LIBRARIES = $(SHARED_LIB) $(SFXPI_LIB) $(EXTERN_LIB)

### SFX-Pi Core ###
HANDLER_SRC = $(addprefix handler/, AbstractHandler.cc)
HANDLER_OBJ = $(patsubst %.cc, obj/%.o, $(HANDLER_SRC))

### Preset Handling ###
PRESET_H_SRC = $(addprefix handler/, PresetHandler.cc)
PRESET_H_OBJ = $(patsubst %.cc, obj/%.o, $(PRESET_H_SRC))

### Plugin Management System ###
PMS_SRC = $(addprefix core/, PluginSourceLoader.cc)
PMS_OBJ = $(patsubst %.cc, obj/%.o, $(PMS_SRC))

GRAPH_SRC = $(addprefix core/, EffectFactory.cc EffectHandler.cc)
GRAPH_OBJ = $(patsubst %.cc, obj/%.o, $(GRAPH_SRC))

### IO Management System ###
CMD_SRC = $(addprefix io/command/, Commands.cc CommandListener.cc CommandFactory.cc CommandManager.cc)
CMD_OBJ = $(patsubst %.cc, obj/%.o, $(CMD_SRC))

LOGIC_SRC = $(addprefix io/logic/, LogicManager.cc mcp23017.cc)
LOGIC_OBJ = $(patsubst %.cc, obj/%.o, $(LOGIC_SRC))

UI_SRC = $(addprefix io/, UIManager.cc)
UI_OBJ = $(patsubst %.cc, obj/%.o, $(UI_SRC))

IO_SRC = $(CMD_SRC) $(LOGIC_SRC) $(UI_SRC)
IO_OBJ = $(CMD_OBJ) $(LOGIC_OBJ) $(UI_OBJ)

### Command List ###
CMD_COMMANDM_SRC = cmd/CommandManagerCommands.cc
CMD_COMMANDM_OBJ = $(patsubst %.cc, obj/%.o, $(CMD_COMMANDM_SRC))

CMD_LOGICM_SRC   = cmd/LogicManagerCommands.cc
CMD_LOGICM_OBJ   = $(patsubst %.cc, obj/%.o, $(CMD_LOGICM_SRC))

CMD_GRAPH_SRC    = cmd/ProcessGraphCommands.cc
CMD_GRAPH_OBJ    = $(patsubst %.cc, obj/%.o, $(CMD_GRAPH_SRC))

CMD_UI_SRC       = cmd/UIManagerCommands.cc
CMD_UI_OBJ       = $(patsubst %.cc, obj/%.o, $(CMD_UI_SRC))

CMDLIST_SRC = $(CMD_COMMANDM_SRC) $(CMD_LOGICM_SRC) $(CMD_GRAPH_SRC) $(CMD_UI_SRC)
CMDLIST_OBJ = $(CMD_COMMANDM_OBJ) $(CMD_LOGICM_OBJ) $(CMD_GRAPH_OBJ) $(CMD_UI_OBJ)

### All Objects ###
SRC = $(PMS_SRC) $(GRAPH_SRC) $(IO_SRC) $(CMDLIST_SRC) $(HANDLER_SRC) $(PRESET_H_SRC)
OBJ = $(PMS_OBJ) $(GRAPH_OBJ) $(IO_OBJ) $(CMDLIST_OBJ) $(HANDLER_OBJ) $(PRESET_H_OBJ)

##########################
##   	   RULES		##
##########################
all: SFX-Pi $(LIBRARIES)

SFX-Pi: obj/SFXPi.o $(OBJ) $(SFXPI_LIB) $(SHARED_LIB)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/SFXPi.o: SFXPi.cc $(SRC)

### Shared Files For Plugins ###

$(PLUGIN_LIB): $(PLUGIN_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(FILTER_LIB): $(FILTER_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

$(DRIVE_LIB): $(DRIVE_SRC)
	$(CXX) $(LIBFLAGS) $(CXXFLAGS) -o $@ $^

### Shared Files For SFX-PE ###
$(PLGLOAD_LIB): $(PLGLOAD_OBJ) $(PLUGIN_LIB)
	ar crf $@ $^
$(PLGLOAD_OBJ): $(PLGLOAD_SRC)

$(PRESET_LIB): $(PRESET_OBJ)
	ar crf $@ $^
$(PRESET_OBJ): $(PRESET_SRC)

$(IOLIB_LIB): $(IOLIB_OBJ)
	ar crf $@ $^

$(IOCORE_OBJ): $(IOCORE_SRC)
$(CMDLIB_OBJ): $(CMDLIB_SRC) $(IOCORE_SRC)
$(LOGLIB_OBJ): $(LOGLIB_SRC) $(IOCORE_SRC)

### SFX-Pi Core ###
$(HANDLER_OBJ): $(HANDLER_SRC)

### Preset Managing ###
$(PRESET_H_OBJ): $(PRESET_H_SRC) $(HANDLER_SRC)

### Plugin Management System ###

$(PMS_OBJ): $(PMS_SRC) $(PLUGIN_SRC) $(PLGLOAD_LIB)
$(GRAPH_OBJ): $(GRAPH_SRC) $(PMS_SRC) $(PLUGIN_SRC) $(HANDLER_SRC)

### Commands Management System ###
$(CMD_OBJ): $(CMD_SRC) $(IOLIB_SRC)
$(LOGIC_OBJ): $(LOGIC_SRC) $(IOLIB_SRC)

$(UI_OBJ): $(UI_SRC) $(CMD_SRC) $(LOGIC_SRC) $(IOLIB_SRC)

### Command List ###
$(CMD_COMMANDM_OBJ): $(CMD_COMMANDM_SRC) $(CMD_SRC) $(IOLIB_SRC)

$(CMD_LOGICM_OBJ): $(CMD_LOGICM_SRC) $(LOGIC_SRC) $(IOLIB_SRC)

$(CMD_GRAPH_OBJ): $(CMD_GRAPH_SRC) $(GRAPH_SRC) $(PMS_SRC) $(PLUGIN_SRC)

$(CMD_UI_OBJ): $(CMD_UI_SRC) $(UI_SRC) $(CMD_SRC) $(LOGIC_SRC) $(IOLIB_SRC)

## Genaral Rules ##

obj/%.o: %.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

%.a: %.o
	ar crf $@ $^

clean:
	rm --recursive -v -rf src/*.o

mrproper:
	rm -rf $(EXEC)
