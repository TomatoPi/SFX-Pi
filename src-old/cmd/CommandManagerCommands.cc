/**********************************************************************
 * @file CommandManagerCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Command Manager
 **********************************************************************/
#include "CommandManagerCommands.h"
/* ***** Command Manager ***** */
CmdPrintCMDManager::CmdPrintCMDManager():
    StrictArgCommand("cmdm-show"
        ,"cmdm-show : Print content of Command Manager"
        ,0)
{
}
void CmdPrintCMDManager::perform(std::vector<std::string> arg){

    CommandManager::printManager();
}

/* ***** Remove and Add Sequencers ***** */
CmdAddSequencer::CmdAddSequencer():
    StrictArgCommand("cmdm-add"
        ,"cmdm-add [ID] : Add a command sequencer with given id"
        ,1)
{
}
void CmdAddSequencer::perform(std::vector<std::string> arg){

    CommandManager::addSequencer(std::stoi(arg[0]));
}

CmdDelSequencer::CmdDelSequencer():
    StrictArgCommand("cmdm-remove"
        ,"cmdm-remove [ID] : Remove given command sequencer"
        ,1)
{
}
void CmdDelSequencer::perform(std::vector<std::string> arg){

    CommandManager::removeSequencer(std::stoi(arg[0]));
}

CmdPrintSequencer::CmdPrintSequencer():
    StrictArgCommand("sequencer-show"
        ,"sequencer-show [ID] : Print given sequencer content"
        ,1)
{
}
void CmdPrintSequencer::perform(std::vector<std::string> arg){
    
    CommandSequencer* seq = CommandManager::getSequencer(std::stoi(arg[0]));
    
    if ( seq != NULL ){

        seq->print();
    }
    else{

        printf(" Use \"cmdm-show\" to view available command sequencers\n");
    }
}

/* ***** Remove or add Link to sequencers ***** */

CmdAddSeqLink::CmdAddSeqLink():
    StrictArgCommand("sequencer-assign"
        ,"sequencer-assign [ID] [SOURCE] [NOTIF] [ACTION] [ARG] : link a notification to an action in given command sequencer"
        ,5)
{
}
void CmdAddSeqLink::perform(std::vector<std::string> arg){
    
    CommandSequencer* seq = CommandManager::getSequencer(std::stoi(arg[0]));
    
    if ( seq != NULL ){

        seq->assign(
              std::stoi(arg[1])
            , static_cast<Notification::NotificationType>(std::stoi(arg[2]))
            , static_cast<CommandSequencer::ActionT>(std::stoi(arg[3]))
            , std::stoi(arg[4])
            );
    }
    else{

        printf(" Use \"cmdm-show\" to view available command sequencers\n");
    }
}

CmdDelSeqLink::CmdDelSeqLink():
    StrictArgCommand("sequencer-unassing"
        ,"sequencer-unassing [ID] [SOURCE] [NOTIF] : Remove given notification link from command sequencer"
        ,3)
{
}
void CmdDelSeqLink::perform(std::vector<std::string> arg){
    
    CommandSequencer* seq = CommandManager::getSequencer(std::stoi(arg[0]));
    
    if ( seq != NULL ){
        
        seq->unassign(
             std::stoi(arg[1])
            , static_cast<Notification::NotificationType>(std::stoi(arg[2]))
            );
    }
    else{

        printf(" Use \"cmdm-show\" to view available command sequencers\n");
    }
}

/* ***** Remove or add Command to sequencers ***** */
CmdAddSeqCmd::CmdAddSeqCmd():
    StrictArgCommand("sequencer-add"
        ,"sequencer-add [ID] [IDX] [Command] : Add a comment to given command Sequencer, IDX is the id of command sequence within given command will be added"
        ,3)
{
}
void CmdAddSeqCmd::perform(std::vector<std::string> arg){
    
    CommandSequencer* seq = CommandManager::getSequencer(std::stoi(arg[0]));
    
    if ( seq != NULL ){

        seq->addCommand(std::stoi(arg[1]), arg[2]);
    }
    else{

        printf(" Use \"cmdm-show\" to view available command sequencers\n");
    }
}

CmdDelSeqCmd::CmdDelSeqCmd():
    StrictArgCommand("sequencer-remove"
        ,"sequencer-add [ID] [IDX] : Remove given command from Command sequencer"
        ,2)
{
}
void CmdDelSeqCmd::perform(std::vector<std::string> arg){
    
    CommandSequencer* seq = CommandManager::getSequencer(std::stoi(arg[0]));
    
    if ( seq != NULL ){

        seq->removeCommand(std::stoi(arg[1]), std::stoi(arg[2]));
    }
    else{

        printf(" Use \"cmdm-show\" to view available command sequencers\n");
    }
}
