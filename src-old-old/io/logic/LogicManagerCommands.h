/**********************************************************************
 * @file LogicManagerCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Logic Manager
 **********************************************************************/
#ifndef DEF_LOGIC_MANAGER_COMMANDS_H
#define DEF_LOGIC_MANAGER_COMMANDS_H

#include "../command/Commands.h"
#include "LogicManager.h"

/* **** Print Logic Manager **** */
class CmdPrintMCPConfig : public StrictArgCommand{

    public :
    
        CmdPrintMCPConfig();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdPrintLogicManager : public StrictArgCommand{

    public :
    
        CmdPrintLogicManager();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* **** Edit Footswitches **** */
class CmdAddFootTarget : public StrictArgCommand{

    public :
    
        CmdAddFootTarget();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdDelFootTarget : public StrictArgCommand{

    public :
    
        CmdDelFootTarget();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowFoot : public StrictArgCommand{

    public :
    
        CmdShowFoot();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
