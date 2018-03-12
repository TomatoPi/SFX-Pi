/**********************************************************************
 * @file CommandSequencer.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File providing object that perform a serie of commands when receive
 * messages
 **********************************************************************/
#include "CommandSequencer.h"
/**
 * Constructor
 **/
CommandSequencer::CommandSequencer(id2_t id):
    m_id(id),
    m_list(CommandSequenceList()),
    m_currentSeq(m_list.begin()),
    m_actions(ActionMap())
{
}

/* **************************** Methods ***************************** */

/**
 * Assign an action to a given notification source and type
 **/
void CommandSequencer::assign(
    id2_t source
    , Notification::NotificationType type
    , ActionT act
    , id2_t arg
    )
{

    Notification key(source, m_id, type);

    //Verify that given notification is not already linked
    if ( m_actions.find(key) == m_actions.end() ){

        m_actions[key] = std::make_pair(act, arg);

        if ( SFXP::GlobalIsDebugEnabled ){

            printf("Command Sequencer : Assign : Notification( 0x%04x:%04x:%02x ) linked to ( 0x%02x:%04x )\n"
                ,source
                ,m_id
                ,static_cast<id1_t>(type)
                ,static_cast<id1_t>(act)
                ,arg
                );
        }
    }
    else{

        printf("Command Sequencer : Error : Notification( 0x%04x:%04x:%02x ) Already linked to ( 0x%02x:%04x )\n"
            ,source
            ,m_id
            ,static_cast<id1_t>(type)
            ,static_cast<id1_t>(act)
            ,arg
            );
    }
}
void CommandSequencer::unassign(id2_t source, Notification::NotificationType type){


    Notification key(source, m_id, type);

    //Search for given Notification
    if ( m_actions.find(key) != m_actions.end() ){

        ActionT act = m_actions[key].first;
        m_actions.erase(key);

        if ( SFXP::GlobalIsDebugEnabled ){

            printf("Command Sequencer : Removed : Notification( 0x%04x:%04x:%02x ) linked to ( 0x%02x )\n"
                ,source
                ,m_id
                ,static_cast<id1_t>(type)
                ,static_cast<id1_t>(act)
                );
        }
    }
    else{

        printf("Command Sequencer : Error : Notification( 0x%04x:%04x:%02x ) is not linked\n"
            ,source
            ,m_id
            ,static_cast<id1_t>(type)
            );
    }
}

/**
 * Push a notification to the command sequencer
 * If notification is linked with an action
 * Correct action will be performed
 **/
void CommandSequencer::pushNotification(Notification notif){

    // Verifiy that notification must go to this sequencer
    if ( notif.target() != m_id ){
        
        printf("Command Sequencer : Error : Target Mismatch : Notification( 0x%04x:%04x:%02x )\n"
            ,notif.source()
            ,notif.target()
            ,static_cast<id1_t>(notif.type())
            );
        return;
    }

    //Search for correct action to compute
    if ( m_actions.find(notif) != m_actions.end() ){

        this->performAction(m_actions[notif]);
    }
    else{
        
        printf("Command Sequencer : Error : Notification( 0x%04x:%04x:%02x ) is not linked\n"
            ,notif.source()
            ,notif.target()
            ,static_cast<id1_t>(notif.type())
            );
    }
}
/**
 * Edit Command Sequencies
 **/
void CommandSequencer::addCommand(id2_t seq, std::string cmd){

    if ( m_list.size() == 0 ){

        m_list[seq].push_back(cmd);
        m_currentSeq = m_list.begin();
    }
    else{

        m_list[seq].push_back(cmd);
    }
}
void CommandSequencer::removeCommand(id2_t seq, usize_t idx){

    if ( m_list.find(seq) != m_list.end() ){

        CommandSequence & sequency = m_list[seq];
        if ( idx < sequency.size() ){

            sequency.erase(sequency.begin() + idx);
        }
    }
    else{

        printf("Command Sequencer : Error : Sequency 0x%04x not found\n"
            ,seq);
    }
}

/**
 * Print Sequencer Content
 **/
void CommandSequencer::print() const{

    // Print List of Command Sequencies
    printf("Command Sequencer : ID( 0x%04x ) :\n",m_id);
    for ( auto & seq : m_list ){

        if ( seq.first == m_currentSeq->first ){
            
            printf("  >> Sequency : 0x%04x :\n", seq.first);
        }
        else{

            printf("   - Sequency : 0x%04x :\n", seq.first);
        }
        
        for ( auto & cmd : seq.second ){

            printf("      - Command : \"%s\"\n", cmd.c_str());
        }
    }

    // Print list of Links
    for ( auto & act : m_actions ){

        printf("   - Notification : 0x%04x:%04x:%02x : Action : 0x%02x:%04x\n"
            ,act.first.source()
            ,act.first.target()
            ,static_cast<id1_t>(act.first.type())
            ,static_cast<id1_t>(act.second.first)
            ,act.second.second
            );
    }

    printf("End\n");
}

/**
 * Perform given action
 **/
void CommandSequencer::performAction(Action act){

    ActionT type = act.first;
    id2_t arg = act.second;

    if ( type != DoNothing && m_list.size() != 0 ){

        /*
         * Change Current Sequence
         */
        if ( type == CycleNext ){
            
            if ( SFXP::GlobalIsDebugEnabled ){
                printf("CommandSequencer : Cycle to Next Sequency\n");
            }

            // Cycle to Next Sequency
            ++m_currentSeq;
            if ( m_currentSeq == m_list.end() ){

                m_currentSeq = m_list.begin();
            }
        }
        else if ( type == CyclePrev ){
            
            if ( SFXP::GlobalIsDebugEnabled ){
                printf("CommandSequencer : Cycle to Prev Sequency\n");
            }

            // Cycle to Prev Sequency
            if ( m_currentSeq == m_list.begin() ){

                m_currentSeq = m_list.end();
            }
            --m_currentSeq;
        }
        else if ( type == JumpTo ){
            
            if ( SFXP::GlobalIsDebugEnabled ){
                printf("CommandSequencer : Jump to Sequency : 0x%04x\n"
                    ,arg);
            }

            // Try to jump to given Sequency
            if ( m_list.find(arg) != m_list.end() ){

                m_currentSeq = m_list.find(arg);
            }
            else{

                printf("Command Sequencer : Error : Action linked to unkown sequency : 0x%02x:%04x\n"
                    ,static_cast<id1_t>(type)
                    ,arg
                    );
            }
        }
        else if ( type == Execute ){
            
            if ( SFXP::GlobalIsDebugEnabled ){
                printf("CommandSequencer : Execute Current Sequency\n");
            }
            // Don't change Current Sequency
        }
        else{

            printf("Command Sequencer : Error : Unknown action : 0x%02x\n"
                ,static_cast<id1_t>(type)
                );
            return;
        }

        // Execute current Command Sequency
        for ( auto & cmd : m_currentSeq->second ){

            CommandParser::performCommand(
                Parser::parseSimpleString(cmd, " ", '"')
                );
        }
    }
}

/**
 * Get Sequencer's ID
 **/
id2_t CommandSequencer::getID() const{

    return m_id;
}

/**
 * Get Stored Command Sequences
 **/
CommandSequencer::CommandSequenceList CommandSequencer::getSequenceList() const{

    return m_list;
}

/**
 * Get Stored Links
 **/
CommandSequencer::ActionMap CommandSequencer::getActionList() const{

    return m_actions;
}
