/**********************************************************************
 * @file CommandManagerCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Command Manager
 **********************************************************************/
#include "EventHandlerCommands.h"
#include "commands/CommandHandler.h"

using namespace std;

/* ***** Command Manager ***** */
CmdPrintCMDManager::CmdPrintCMDManager(CommandHandler* h):
    StrictArgCommand(h, "evh-show"
        ,"evh-show : Print list of event sequencers"
        ,0)
{
}
void CmdPrintCMDManager::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHPrint);

    _owner->eventHandler()->pushEvent(&event);
}

/* ***** Remove and Add Sequencers ***** */
CmdAddSequencer::CmdAddSequencer(CommandHandler* h):
    StrictArgCommand(h, "evh-add"
        ,"evh-add [ID] : Add an event sequencer with given id"
        ,1)
{
}
void CmdAddSequencer::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHAddEVS);
    event._evh = EVHEvent(std::stoi(arg[0]), 0, 0, 0, 0);

    _owner->eventHandler()->pushEvent(&event);
}

CmdDelSequencer::CmdDelSequencer(CommandHandler* h):
    StrictArgCommand(h, "evh-remove"
        ,"evh-remove [ID] : Remove given event sequencer"
        ,1)
{
}
void CmdDelSequencer::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHRemoveEVS);
    event._evh = EVHEvent(std::stoi(arg[0]), 0, 0, 0, 0);

    _owner->eventHandler()->pushEvent(&event);
}

CmdPrintSequencer::CmdPrintSequencer(CommandHandler* h):
    StrictArgCommand(h, "eseq-show"
        ,"eseq-show [ID] : Print given sequencer content"
        ,1)
{
}
void CmdPrintSequencer::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHPrintEVS);
    event._evh = EVHEvent(std::stoi(arg[0]), 0, 0, 0, 0);

    _owner->eventHandler()->pushEvent(&event);
}

/* ***** Remove or add Link to sequencers ***** */

CmdAddSeqLink::CmdAddSeqLink(CommandHandler* h):
    StrictArgCommand(h, "eseq-assign"
        ,"eseq-assign [ID] [TRIGGER] [STATUS] [ACTION] [TARGET] : link a external input with an action for an event sequencer"
        ,5)
{
}
void CmdAddSeqLink::perform(std::vector<std::string> arg){
    

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHAssign);
    event._evh = EVHEvent(   std::stoi(arg[0])
                            ,std::stoi(arg[1])
                            ,std::stoi(arg[2])
                            ,std::stoi(arg[3])
                            ,std::stoi(arg[4]));

    _owner->eventHandler()->pushEvent(&event);
}

CmdDelSeqLink::CmdDelSeqLink(CommandHandler* h):
    StrictArgCommand(h, "eseq-unassing"
        ,"eseq-unassing [ID] [TRIGGER] [STATUS] : Remove given link from event sequencer"
        ,3)
{
}
void CmdDelSeqLink::perform(std::vector<std::string> arg){
    
    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EVHUnassign);
    event._evh = EVHEvent(   std::stoi(arg[0])
                            ,std::stoi(arg[1])
                            ,std::stoi(arg[2])
                            ,0
                            ,0);

    _owner->eventHandler()->pushEvent(&event);
}

/* ***** Remove or add Command to sequencers ***** */
CmdAddSeqCmd::CmdAddSeqCmd(CommandHandler* h):
    StrictArgCommand(h, "eseq-add"
        ,"eseq-add [ID] [IDX] [Command] : Add a comment to given an event sequencer, IDX is the id of event sequence within given command will be added"
        ,3)
{
}
void CmdAddSeqCmd::perform(std::vector<std::string> arg){
    
    cout << "TODO" << endl;
}

CmdDelSeqCmd::CmdDelSeqCmd(CommandHandler* h):
    StrictArgCommand(h, "eseq-remove"
        ,"eseq-add [ID] [IDX] : Remove given command from an event sequencer"
        ,2)
{
}
void CmdDelSeqCmd::perform(std::vector<std::string> arg){

    cout << "TODO" << endl;
}
