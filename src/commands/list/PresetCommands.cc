/**********************************************************************
 * @file PresetCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset Commands bodies
 **********************************************************************/
#include "PresetCommands.h"
#include "commands/CommandHandler.h"

using namespace std;

CmdLoadPreset::CmdLoadPreset(CommandHandler* h):
    StrictArgCommand(h, "load-preset",
        "load-preset [filename] : load preset inside file files/ directory with given name",
        1)
{
}
void CmdLoadPreset::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_LoadPreset);
    event._preset = PresetEvent(arg[0], nullptr);

    _owner->presetHandler()->pushEvent(&event);
}

CmdSavePreset::CmdSavePreset(CommandHandler* h):
    StrictArgCommand(h, "save-preset",
        "save-preset [filename] : save preset file inside files/ directory with given name",
        1)
{
}
void CmdSavePreset::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_SavePreset);
    event._preset = PresetEvent(arg[0], nullptr);

    _owner->presetHandler()->pushEvent(&event);
}

CmdListPreset::CmdListPreset(CommandHandler* h):
    StrictArgCommand(h, "list-preset",
        "list-preset : list available files indide files/ directory",
        0)
{
}
void CmdListPreset::perform(std::vector<std::string> arg){

    printf("List files in directory \"%s\" :\n", SFXP::DIR_PRST.c_str());

    std::vector<std::string> files = utils::listFiles(SFXP::DIR_PRST);
    for (auto& f : files) {
        
        printf("   - \"%s\"\n", f);
    }
}
