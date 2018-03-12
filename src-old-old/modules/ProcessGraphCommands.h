/**********************************************************************
 * @file ProcessGraphCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Commands Bodies for Process Graph
 **********************************************************************/
#ifndef DEF_PROCESS_GRAPH_COMMANDS_H
#define DEF_PROCESS_GRAPH_COMMANDS_H

#include "../io/command/Commands.h"

#include "ProcessGraph.h"

/* *********************** Graph Edition Commands ******************* */
class CmdAddEffect : public StrictArgCommand{

    public :
    
        CmdAddEffect();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveEffect : public StrictArgCommand{

    public :
    
        CmdRemoveEffect();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdAddConnection : public StrictArgCommand{

    public :
    
        CmdAddConnection();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveConnection : public StrictArgCommand{

    public :
    
        CmdRemoveConnection();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdClearGraph : public StrictArgCommand{

    public :
    
        CmdClearGraph();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowGraph : public StrictArgCommand{

    public :
    
        CmdShowGraph();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdListAvailable : public StrictArgCommand{

    public :
    
        CmdListAvailable();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ********************** Effects Edition Commands ****************** */
class CmdShowEffect : public StrictArgCommand{

    public :
    
        CmdShowEffect();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdAddBank : public StrictArgCommand{

    public :
    
        CmdAddBank();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveBank : public StrictArgCommand{

    public :
    
        CmdRemoveBank();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdEditParam : public StrictArgCommand{

    public :
    
        CmdEditParam();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowParam : public StrictArgCommand{

    public :
    
        CmdShowParam();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowPool : public StrictArgCommand{

    public :
    
        CmdShowPool();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdChangeBank : public StrictArgCommand{

    public :
    
        CmdChangeBank();

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
