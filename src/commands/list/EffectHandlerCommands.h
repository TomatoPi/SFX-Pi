/**********************************************************************
 * @file ProcessGraphCommands.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Commands Bodies for Process Graph
 **********************************************************************/
#ifndef DEF_PROCESS_GRAPH_COMMANDS_H
#define DEF_PROCESS_GRAPH_COMMANDS_H

#include "core/Connection.h"
#include "commands/Commands.h"

#include "effect/EffectFactory.h"

/* *********************** Graph Edition Commands ******************* */
class CmdAddEffect : public StrictArgCommand{

    public :
    
        CmdAddEffect(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveEffect : public StrictArgCommand{

    public :
    
        CmdRemoveEffect(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdAddConnection : public StrictArgCommand{

    public :
    
        CmdAddConnection(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveConnection : public StrictArgCommand{

    public :
    
        CmdRemoveConnection(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdClearGraph : public StrictArgCommand{

    public :
    
        CmdClearGraph(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowGraph : public StrictArgCommand{

    public :
    
        CmdShowGraph(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdListAvailable : public StrictArgCommand{

    public :
    
        CmdListAvailable(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

/* ********************** Effects Edition Commands ****************** */
class CmdShowEffect : public StrictArgCommand{

    public :
    
        CmdShowEffect(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdAddBank : public StrictArgCommand{

    public :
    
        CmdAddBank(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdRemoveBank : public StrictArgCommand{

    public :
    
        CmdRemoveBank(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdEditParam : public StrictArgCommand{

    public :
    
        CmdEditParam(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowParam : public StrictArgCommand{

    public :
    
        CmdShowParam(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdShowPool : public StrictArgCommand{

    public :
    
        CmdShowPool(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

class CmdChangeBank : public StrictArgCommand{

    public :
    
        CmdChangeBank(CommandHandler* h);

    private :
    
        virtual void perform(std::vector<std::string> arg);
};

#endif
