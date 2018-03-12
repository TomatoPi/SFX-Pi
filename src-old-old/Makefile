CXX=g++
CXXFLAGS= -g -O3 -Wunused-variable -Wall -Wwrite-strings -Wreturn-type
LDFLAGS=-ljack -lpthread 
EXEC=SFX-Pi

SOURCE=src/
OBJECT=obj/

## Core ##
CORE_DIR= core/
CORE_SRC= $(addprefix $(SOURCE)$(CORE_DIR), Singleton.h Singleton.hpp ReminderTree.h ReminderTree.hpp Parser.h Parser.hpp)

## IO Files ##
IO_DIR= io/
IO_CORE_SRC= $(addprefix $(SOURCE)$(IO_DIR)core/, Listener.cc Message.cc)
IO_CORE_OBJ= $(IO_CORE_SRC:.cc=.o)

COMMAND_SRC= $(addprefix $(SOURCE)$(IO_DIR)command/, CommandListener.cc CommandParser.cc Commands.cc CommandSequencer.cc CommandManager.cc CommandManagerCommands.cc)
COMMAND_OBJ= $(COMMAND_SRC:.cc=.o=)

LOGIC_SRC= $(addprefix $(SOURCE)$(IO_DIR)logic/, mcp23017.cc FootSwitch.cc LED.cc LogicManager.cc LogicManagerCommands.cc)
LOGIC_OBJ= $(LOGIC_SRC:.cc=.o=)

IO_MAIN_SRC= $(addprefix $(SOURCE)$(IO_DIR), UIManager.cc UIManagerCommands.cc)
IO_MAIN_OBJ= $(IO_MAIN_SRC:.cc=.o=)

IO_SRC= $(COMMAND_SRC) $(LOGIC_SRC) $(IO_MAIN_SRC)
IO_OBJ= $(IO_SRC:.cc=.o)
## Module Core Files ##
MODULE_DIR= modules/
MODULE_CORE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)core/, AbstractEffectUnit.cc JACKUnit.cc)
MODULE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR), UnitFactory.cc ProcessGraph.cc ProcessGraphCommands.cc)

MODULE_CORE_OBJ = $(MODULE_CORE_SRC:.cc=.o)
MODULE_OBJ= $(MODULE_SRC:.cc=.o)

PRESET_DIR= preset/
PRESET_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(PRESET_DIR), Preset.cc PresetCommands.cc)
PRESET_OBJ= $(PRESET_SRC:.cc=.o)

## Effects Dependencies Files ( Filter / buffer ) ##
FILTER_DIR= core/filter/
FILTER_CORE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(FILTER_DIR), FilterBase.cc)
FILTER_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(FILTER_DIR), MonoPoleFilter.cc GraphicEQ.cc ParametricEQ.cc)

FILTER_CORE_OBJ= $(FILTER_CORE_SRC:.cc=.o)
FILTER_OBJ= $(FILTER_SRC:.cc=.o)

BUFFER_DIR= core/buffer/
BUFFER_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(BUFFER_DIR), )
BUFFER_OBJ= $(BUFFER_SRC:.cc=.o)

MIDI_DIR= midi/
MIDI_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(MIDI_DIR), MIDIBase.cc)
MIDI_OBJ= $(MIDI_SRC:.cc=.o)

EFFECT_DEP_SRC = $(FILTER_CORE_SRC) $(FILTER_SRC) $(BUFFER_SRC) $(MIDI_SRC)
EFFECT_DEP_OBJ = $(EFFECT_DEP_SRC:.cc=.o)

## Effects Files ##
EFFECT_DIR= effects/
DRIVE_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(EFFECT_DIR)drive/, DriveBase.cc Drive.cc MatrixDrive.cc)
DRIVE_OBJ= $(DRIVE_SRC:.cc=.o)

FFT_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(EFFECT_DIR)fft/, FFTransformer.cc FFTFilter.cc FFTDelay.cc)
FFT_OBJ= $(FFT_SRC:.cc=.o)

DELAY_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(EFFECT_DIR)delay/, DelayBase.cc Delay.cc Reverb.cc Chorus.cc)
DELAY_OBJ= $(DELAY_SRC:.cc=.o)

MOD_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(EFFECT_DIR)mod/, LFO.cc)
MOD_OBJ= $(MOD_SRC:.cc=.o)

MISC_SRC= $(addprefix $(SOURCE)$(MODULE_DIR)$(EFFECT_DIR), EndUnit.cc)
MISC_OBJ= $(MISC_SRC:.cc=.o)

EFFECT_SRC= $(DRIVE_SRC) $(FFT_SRC) $(DELAY_SRC) $(MOD_SRC) $(MISC_SRC)
EFFECT_OBJ= $(EFFECT_SRC:.cc=.o)

## Listing of All Files ##
SRC= $(MODULE_CORE_SRC) $(MODULE_SRC) $(MIDI_SRC) $(PRESET_SRC) $(FILTER_CORE_SRC) $(FILTER_SRC) $(EFFECT_SRC) $(IO_CORE_SRC) $(IO_SRC)
OBJ= $(SRC:.cc=.o)

##########################
##   	   RULES		##
##########################
all: SFX-Pi

SFX-Pi: $(OBJ) $(SOURCE)SFX-Pi.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SOURCE)SFX-Pi.o: $(SOURCE)SFX-Pi.cc $(CORE_SRC) $(SRC)

## Build IO ##
$(IO_CORE_OBJ): $(IO_CORE_SRC)

$(IO_OBJ): $(IO_CORE_SRC) $(IO_SRC)

$(COMMAND_OBJ): $(IO_CORE_SRC) $(COMMAND_SRC)

$(LOGIC_OBJ): $(IO_CORE_SRC) $(COMMAND_SRC) $(LOGIC_SRC)

## Build Modules Core and Factory ##
$(MODULE_CORE_OBJ): $(MODULE_CORE_SRC)

$(MODULE_OBJ): $(MODULE_CORE_SRC) $(MODULE_SRC) $(MIDI_SRC)

$(PRESET_OBJ): $(MODULE_CORE_SRC) $(MODULE_SRC) $(PRESET_SRC)

## Build Effect's Dependencies ##
$(FILTER_CORE_OBJ): $(FILTER_CORE_SRC)

$(FILTER_OBJ): $(FILTER_CORE_SRC) $(FILTER_SRC)

$(BUFFER_OBJ): $(BUFFER_SRC)

$(MIDI_OBJ): $(MIDI_SRC)

## Build Effects from EffectCore and Effect Dependencies ##
$(DRIVE_OBJ): $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC) $(DRIVE_SRC)

$(FFT_OBJ): $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC) $(FFT_SRC)

$(DELAY_OBJ): $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC) $(DELAY_SRC)

$(MOD_OBJ): $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC) $(MOD_SRC)

$(MISC_OBJ): $(MODULE_CORE_SRC) $(EFFECT_DEP_SRC) $(MISC_SRC)
## Genaral Rules ##

%.o: %.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm --recursive -v -rf ./src/*.o

mrproper:
	rm -rf $(EXEC)
