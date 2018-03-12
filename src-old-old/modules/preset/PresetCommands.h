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

#include "../../io/command/Commands.h"

#include "Preset.h"

class CmdLoadPreset : public StrictArgCommand{

    public :
    
        CmdLoadPreset();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdSavePreset : public StrictArgCommand{

    public :
    
        CmdSavePreset();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdLoadEffect : public VarArgCommand{

    public :
    
        CmdLoadEffect();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdSaveEffect : public StrictArgCommand{

    public :
    
        CmdSaveEffect();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdListPreset : public StrictArgCommand{

    public :
    
        CmdListPreset();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
