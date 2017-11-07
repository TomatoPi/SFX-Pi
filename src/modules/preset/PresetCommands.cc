/**********************************************************************
 * @file PresetCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset Commands bodies
 **********************************************************************/
#include "PresetCommands.h"

CmdLoadPreset::CmdLoadPreset():
    StrictArgCommand("load-preset",
        "load-preset [filename] : load preset inside file files/ directory with given name",
        1)
{
}
void CmdLoadPreset::perform(std::vector<std::string> arg){

    if ( Preset::loadPreset(arg[0]) ){

        printf("use \"list-preset\" to get list of available preset files");
    }
}

CmdSavePreset::CmdSavePreset():
    StrictArgCommand("save-preset",
        "save-preset [filename] : save preset file inside files/ directory with given name",
        1)
{
}
void CmdSavePreset::perform(std::vector<std::string> arg){

    if ( Preset::savePreset(arg[0]) ){

        printf("try another filename or verify permissions inside preset directory");
    }
}

CmdLoadEffect::CmdLoadEffect():
    VarArgCommand("load-effect",
        "load-effect [filename] {override} : load effect file inside files/ directory with given name, any char as second argument will enable effect overriding",
        1, 2)
{
}
void CmdLoadEffect::perform(std::vector<std::string> arg){

    AbstractEffectUnit* unit = Preset::loadEffect(arg[0]);

    if ( unit == NULL ){

        printf("use \"list-preset\" to get list of available preset files");
    }
    else{

        bool override = arg.size() == 2;
        if ( ProcessGraph::Get().addEffect(unit, override) && !override ){

            printf("Given effect is already present, delete it, see help to permit override\n");
            delete unit;
        }
    }
}

CmdSaveEffect::CmdSaveEffect():
    StrictArgCommand("save-effect",
        "save-effect [filename] [ID] : save effect file inside files/ directory with given name",
        2)
{
}
void CmdSaveEffect::perform(std::vector<std::string> arg){

    if ( Preset::saveEffect(arg[0], ProcessGraph::Get().getEffect(std::stoi(arg[1]))) ){

        printf("try another filename or verify permissions inside preset directory");
    }
}

CmdListPreset::CmdListPreset():
    StrictArgCommand("list-preset",
        "list-preset : list available files indide files/ directory",
        0)
{
}
void CmdListPreset::perform(std::vector<std::string> arg){

    printf("List files in directory \"%s\" :\n", SFXP::PRESET_PATH.c_str());

    DIR *rep = opendir(SFXP::PRESET_PATH.c_str());

    if ( rep != NULL ){

        struct dirent *ent;

        while( (ent = readdir(rep)) != NULL ){

            if ( ent->d_name[0] != '.')
                printf("   - \"%s\"\n", ent->d_name);
        }
    }
    else{

        printf("Error : can't open given directory\n");
    }
    closedir(rep);
}
