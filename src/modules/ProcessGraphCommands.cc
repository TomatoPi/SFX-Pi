/**********************************************************************
 * @file ProcessGraphCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Commands Bodies for Process Graph
 **********************************************************************/
#include "ProcessGraphCommands.h"

/**
 * Add An Effect To Process Graph
 **/
CmdAddEffect::CmdAddEffect():
    StrictArgCommand("graph-add",
        "graph-add [typeCode] [ID] : Add given effect type to graph with given ID",
        2)
{
}
void CmdAddEffect::perform(std::vector<std::string> arg){

    int status = ProcessGraph::addEffect( std::stoi(arg[0]), std::stoi(arg[1]) );

    if ( status & ProcessGraph::ErrIdNotUnique ){
        printf("Use \"graph-show\" to show used id\n");
    }
    else if ( status & ProcessGraph::ErrInvalidEffect ){
        printf("Use \"graph-available\" to show available effect types\n");
    }
}

/**
 * Remove An Effect From Process Graph
 **/
CmdRemoveEffect::CmdRemoveEffect():
    StrictArgCommand("graph-remove",
        "graph-remove [ID] : Remove given effect from the graph",
        1)
{
}
void CmdRemoveEffect::perform(std::vector<std::string> arg){

    int status = ProcessGraph::removeEffect( std::stoi(arg[0]));

    if ( status & ProcessGraph::ErrInvalidEffect ){
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Add A Connection To Process Graph
 **/
CmdAddConnection::CmdAddConnection():
    StrictArgCommand("graph-connect",
        "graph-connect [Source-ID] [Source-Port] [Target-ID] [Target-Port] : Connect two effects together",
        4)
{
}
void CmdAddConnection::perform(std::vector<std::string> arg){

    ProcessGraph::Connection c = ProcessGraph::Connection(
        std::stoi(arg[0]), std::stoi(arg[1]),
        std::stoi(arg[2]), std::stoi(arg[3]) );
                                
    if ( ProcessGraph::addConnection(c) ){
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Remove A Connection From Process Graph
 **/
CmdRemoveConnection::CmdRemoveConnection():
    StrictArgCommand("graph-disconnect",
        "graph-disconnect [Source-ID] [Source-Port] [Target-ID] [Target-Port] : Disconnect two effects",
        4)
{
}
void CmdRemoveConnection::perform(std::vector<std::string> arg){

    ProcessGraph::Connection c = ProcessGraph::Connection(
        std::stoi(arg[0]), std::stoi(arg[1]),
        std::stoi(arg[2]), std::stoi(arg[3]) );
                                
    if ( ProcessGraph::removeConnection(c) ){
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Remove All Effects From Graph
 **/
CmdClearGraph::CmdClearGraph():
    StrictArgCommand("graph-clear",
        "graph-clear : Delete all effects from graph",
        0)
{
}
void CmdClearGraph::perform(std::vector<std::string> arg){

    ProcessGraph::clearGraph();
}

/**
 * Print Current Graph Content
 **/
CmdShowGraph::CmdShowGraph():
    StrictArgCommand("graph-show",
        "graph-show : Print current graph content",
        0)
{
}
void CmdShowGraph::perform(std::vector<std::string> arg){

    ProcessGraph::printGraph();
}

/**
 * Print Available Effects type
 **/
CmdListAvailable::CmdListAvailable():
    StrictArgCommand("graph-available",
        "graph-available : Print list of available effects",
        0)
{
}
void CmdListAvailable::perform(std::vector<std::string> arg){

    UnitFactory::printRegister();
}

/**
 * Print Effect Content
 **/
CmdShowEffect::CmdShowEffect():
    StrictArgCommand("effect-show",
        "effect-show [ID] : Print content of given effect",
        1)
{
}
void CmdShowEffect::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        unit->printEffect();
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Add bank to given Effect
 **/
CmdAddBank::CmdAddBank():
    StrictArgCommand("effect-add",
        "effect-add [ID] [BankID] : Add new bank to given effect",
        2)
{
}
void CmdAddBank::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        unit->addBank(std::stoi(arg[1]));
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Remove given bank from effect
 **/
CmdRemoveBank::CmdRemoveBank():
    StrictArgCommand("effect-remove",
        "effect-remove [ID] [BankID] : Remove given bank from effect",
        2)
{
}
void CmdRemoveBank::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        unit->removeBank(std::stoi(arg[1]));
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Change given param's value
 **/
CmdEditParam::CmdEditParam():
    StrictArgCommand("effect-edit",
        "effect-edit [ID] [EffectIDX] [Value] : Change given effect's parameter",
        3)
{
}
void CmdEditParam::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        unit->setParam(std::stoi(arg[1]), std::stof(arg[2]));
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Get Names and index of parameters for a given type
 **/
CmdShowParam::CmdShowParam():
    StrictArgCommand("effect-list",
        "effect-list [TypeCode] : Get names and indexes of given effect type",
        1)
{
}
void CmdShowParam::perform(std::vector<std::string> arg){

    std::vector<std::string> name = UnitFactory::getParamNames(std::stoi(arg[0]));

    if ( name.size() != 0 ){

        printf("Effect : %s\n", UnitFactory::getName(std::stoi(arg[0])).c_str());
        for ( uint8_t i = 0; i < name.size(); i++ ){

            printf("   - %2u : %s\n", i, name[i].c_str());
        }
        printf("End\n");
    }
    else{
        printf("Use \"graph-available\" to show available effect types\n");
    }
}

/**
 * Show effect current param pool
 * pool is the set of values used inside process callback
 **/
CmdShowPool::CmdShowPool():
    StrictArgCommand("effect-pool",
        "effect-pool [ID] : Get current effect's pool used in poress callback",
        1)
{
}
void CmdShowPool::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        unit->printPool();
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}

/**
 * Change current bank
 **/
CmdChangeBank::CmdChangeBank():
    StrictArgCommand("effect-bank",
        "effect-bank [ID] [BankID] : Change effect's current bank",
        2)
{
}
void CmdChangeBank::perform(std::vector<std::string> arg){

    AbstractEffectUnit *unit = ProcessGraph::getEffect(std::stoi(arg[0]));
    
    if ( unit != NULL ){
        
        if ( unit->changeBank(std::stoi(arg[1])) ){

            printf("Use \"effect-show\" to show available banks\n");
        }
    }
    else{
        printf("Use \"graph-show\" to show available effects\n");
    }
}
