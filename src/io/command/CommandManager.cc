/**********************************************************************
 * @file CommandParser.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Manager of all commands related stuff
 **********************************************************************/
#include "CommandManager.h"

/* ****************************** Members *************************** */
CommandManager::SequencerList CommandManager::m_seqList
    = CommandManager::SequencerList();

/* ************************** CommandManager ************************ */
int CommandManager::create(){

    printf("Command Manager : Start\n");

    CommandListener::create();

    return 0;
}
int CommandManager::kill(){

    printf("Command Manager : Stop\n");

    CommandListener::kill();

    printf("\n");

    CommandParser::clearRegister();

    return 0;
}

void CommandManager::pushNotification(Notification notif){

    // Search for targeted Command Sequencer
    if ( m_seqList.find(notif.target()) != m_seqList.end() ){

        if ( SFXP::GlobalIsDebugEnabled ){
            printf("Command Manager : Update : Push notification to Command Sequencer ( 0x%04x:%04x:%02x )\n"
                ,notif.source()
                ,notif.target()
                ,static_cast<id1_t>(notif.type())
                );
        }
        m_seqList[notif.target()]->pushNotification(notif);
    }
    else{

        printf("Command Manager : Error : Invalid Notification Target ( 0x%04x:%04x:%02x )\n"
            ,notif.source()
            ,notif.target()
            ,static_cast<id1_t>(notif.type())
            );
    }
}

void CommandManager::addSequencer(id2_t id){

    id =  (id & ~SFXP::ID_MASK) | SFXP::ID_SEQ;

    if ( m_seqList.find(id) == m_seqList.end() ){

        m_seqList[id] = new CommandSequencer(id);
    }
    else{

        printf("Command Manager : Error : ID( 0x%04x ) is already used\n"
            ,id);
    }
}
void CommandManager::removeSequencer(id2_t id){
    
    id =  (id & ~SFXP::ID_MASK) | SFXP::ID_SEQ;

    if ( m_seqList.find(id) != m_seqList.end() ){

        delete m_seqList[id];
        m_seqList.erase(id);
    }
    else{

        printf("Command Manager : Error : ID( 0x%04x ) not found\n"
            ,id);
    }
}

CommandSequencer* CommandManager::getSequencer(id2_t id){
    
    id =  (id & ~SFXP::ID_MASK) | SFXP::ID_SEQ;

    if ( m_seqList.find(id) != m_seqList.end() ){

        return m_seqList[id];
    }
    return NULL;
}

void CommandManager::printManager(){

    printf("Command Manager :\n");
    
    for ( auto & seq : m_seqList ){

        printf("   - Command Sequencer : ID( 0x%04x )\n", seq.first);
    }

    printf("End\n");
}
