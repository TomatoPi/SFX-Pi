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

int CommandManager::addSequencer(id2_t id){

    id =  (id & ~SFXP::ID_MASK) | SFXP::ID_SEQ;

    if ( m_seqList.find(id) == m_seqList.end() ){

        m_seqList[id] = new CommandSequencer(id);
    }
    else{

        printf("Command Manager : Error : ID( 0x%04x ) is already used\n"
            ,id);

        return 1;
    }
    
    return 0;
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

int CommandManager::addSequencer(CommandSequencer* seq){

    if ( seq == NULL ){

        printf("Command Manager : Error : Null Pointer passed\n");
        return 1;
    }

    id2_t id = seq->getID();

    if ( m_seqList.find(id) == m_seqList.end() ){

        m_seqList[id] = seq;
    }
    else{

        printf("Command Manager : Error : ID( 0x%04x ) is already used\n"
            ,id);
            
        return 1;
    }
    
    return 0;
}

std::vector<id2_t> CommandManager::getSequencerList(){

    std::vector<id2_t> ids;

    for ( auto & cur : m_seqList ){

        ids.push_back(cur.first);
    }

    return ids;
}

void CommandManager::printManager(){

    printf("Command Manager :\n");
    
    for ( auto & seq : m_seqList ){

        printf("   - Command Sequencer : ID( 0x%04x )\n", seq.first);
    }

    printf("End\n");
}

/**
 * Clear Command Manager Configuration
 **/
void CommandManager::clearManager(){
    
    for ( auto & seq : m_seqList ){

        delete seq.second;
        m_seqList.erase(seq.first);
    }
    m_seqList.clear();
}
