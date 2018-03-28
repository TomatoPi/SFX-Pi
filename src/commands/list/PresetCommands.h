/**********************************************************************
 * @file PresetCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Preset Commands bodies
 **********************************************************************/
#ifndef DEF_PRESET_COMMANDS_H
#define DEF_PRESET_COMMANDS_H

#include <dirent.h>
#include "commands/Commands.h"
#include "core/FileUtils.h"

class CmdLoadPreset : public StrictArgCommand{

    public :
    
        CmdLoadPreset(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdSavePreset : public StrictArgCommand{

    public :
    
        CmdSavePreset(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdListPreset : public StrictArgCommand{

    public :
    
        CmdListPreset(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
