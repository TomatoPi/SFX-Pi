/**********************************************************************
 * @file CommandManagerCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Command Manager
 **********************************************************************/
#ifndef DEF_COMMAND_MANAGER_COMMANDS_H
#define DEF_COMMAND_MANAGER_COMMANDS_H

#include "Commands.h"
#include "CommandManager.h"

/* ***** Command Manager ***** */
class CmdPrintCMDManager : public StrictArgCommand{

    public :
    
        CmdPrintCMDManager();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove and Add Sequencers ***** */
class CmdAddSequencer : public StrictArgCommand{

    public :
    
        CmdAddSequencer();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSequencer : public StrictArgCommand{

    public :
    
        CmdDelSequencer();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdPrintSequencer : public StrictArgCommand{

    public :
    
        CmdPrintSequencer();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove or add Link to sequencers ***** */
class CmdAddSeqLink : public StrictArgCommand{

    public :
    
        CmdAddSeqLink();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSeqLink : public StrictArgCommand{

    public :
    
        CmdDelSeqLink();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove or add Command to sequencers ***** */
class CmdAddSeqCmd : public StrictArgCommand{

    public :
    
        CmdAddSeqCmd();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSeqCmd : public StrictArgCommand{

    public :
    
        CmdDelSeqCmd();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
