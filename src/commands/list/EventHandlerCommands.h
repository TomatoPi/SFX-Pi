/**********************************************************************
 * @file CommandManagerCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Command Manager
 **********************************************************************/
#ifndef DEF_COMMAND_MANAGER_COMMANDS_H
#define DEF_COMMAND_MANAGER_COMMANDS_H

#include "core/Connection.h"
#include "commands/Commands.h"

/* ***** Command Manager ***** */
class CmdPrintCMDManager : public StrictArgCommand{

    public :
    
        CmdPrintCMDManager(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove and Add Sequencers ***** */
class CmdAddSequencer : public StrictArgCommand{

    public :
    
        CmdAddSequencer(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSequencer : public StrictArgCommand{

    public :
    
        CmdDelSequencer(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdPrintSequencer : public StrictArgCommand{

    public :
    
        CmdPrintSequencer(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove or add Link to sequencers ***** */
class CmdAddSeqLink : public StrictArgCommand{

    public :
    
        CmdAddSeqLink(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSeqLink : public StrictArgCommand{

    public :
    
        CmdDelSeqLink(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ***** Remove or add Command to sequencers ***** */
class CmdAddSeqCmd : public StrictArgCommand{

    public :
    
        CmdAddSeqCmd(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelSeqCmd : public StrictArgCommand{

    public :
    
        CmdDelSeqCmd(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
