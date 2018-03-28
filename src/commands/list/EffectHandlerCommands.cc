/**********************************************************************
 * @file ProcessGraphCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * File Containing Commands Bodies for Process Graph
 **********************************************************************/
#include "EffectHandlerCommands.h"
#include "commands/CommandHandler.h"

using namespace std;

/**
 * Add An Effect To Process Graph
 **/
CmdAddEffect::CmdAddEffect(CommandHandler* h):
    StrictArgCommand(h, "graph-add",
        "graph-add [typeCode] [ID] : Add given effect type to graph with given ID",
        2)
{
}
void CmdAddEffect::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_AddEffect);
    event._edit = EditionEvent(std::stoi(arg[0]), std::stoi(arg[1]));

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Remove An Effect From Process Graph
 **/
CmdRemoveEffect::CmdRemoveEffect(CommandHandler* h):
    StrictArgCommand(h, "graph-remove",
        "graph-remove [ID] : Remove given effect from the graph",
        1)
{
}
void CmdRemoveEffect::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_RemoveEffect);
    event._edit = EditionEvent(std::stoi(arg[0]), 0);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Add A Connection To Process Graph
 **/
CmdAddConnection::CmdAddConnection(CommandHandler* h):
    StrictArgCommand(h, "graph-connect",
        "graph-connect [Source-ID] [Source-Port] [Target-ID] [Target-Port] [MIDI] : Connect two effects together, nonzero value for midi set port to midi type",
        5)
{
}
void CmdAddConnection::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_GraphConnect);
    event._graph = GraphEvent(
        std::stoi(arg[0]), std::stoi(arg[1]),
        std::stoi(arg[2]), std::stoi(arg[3]),
        std::stoi(arg[4]) );

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Remove A Connection From Process Graph
 **/
CmdRemoveConnection::CmdRemoveConnection(CommandHandler* h):
    StrictArgCommand(h, "graph-disconnect",
        "graph-disconnect [Source-ID] [Source-Port] [Target-ID] [Target-Port] [MIDI] : Disconnect two effects, nonzero value for midi set port to midi type",
        5)
{
}
void CmdRemoveConnection::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_GraphDisconnect);
    event._graph = GraphEvent(
        std::stoi(arg[0]), std::stoi(arg[1]),
        std::stoi(arg[2]), std::stoi(arg[3]),
        std::stoi(arg[4]) );

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Remove All Effects From Graph
 **/
CmdClearGraph::CmdClearGraph(CommandHandler* h):
    StrictArgCommand(h, "graph-clear",
        "graph-clear : Delete all effects from graph",
        0)
{
}
void CmdClearGraph::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_ClearGraph);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Print Current Graph Content
 **/
CmdShowGraph::CmdShowGraph(CommandHandler* h):
    StrictArgCommand(h, "graph-show",
        "graph-show : Print current graph content",
        0)
{
}
void CmdShowGraph::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_ShowGraph);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Print Available Effects type
 **/
CmdListAvailable::CmdListAvailable(CommandHandler* h):
    StrictArgCommand(h, "graph-available",
        "graph-available : Print list of available effects",
        0)
{
}
void CmdListAvailable::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_ListAvailable);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Print Effect Content
 **/
CmdShowEffect::CmdShowEffect(CommandHandler* h):
    StrictArgCommand(h, "effect-show",
        "effect-show [ID] : Print content of given effect",
        1)
{
}
void CmdShowEffect::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectShow);
    event._effect = EffectEvent(std::stoi(arg[0]), 0, 0);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Add bank to given Effect
 **/
CmdAddBank::CmdAddBank(CommandHandler* h):
    StrictArgCommand(h, "effect-add",
        "effect-add [ID] [BankID] : Add new bank to given effect",
        2)
{
}
void CmdAddBank::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectAddBank);
    event._effect = EffectEvent(std::stoi(arg[0]), std::stoi(arg[1]), 0);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Remove given bank from effect
 **/
CmdRemoveBank::CmdRemoveBank(CommandHandler* h):
    StrictArgCommand(h, "effect-remove",
        "effect-remove [ID] [BankID] : Remove given bank from effect",
        2)
{
}
void CmdRemoveBank::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectRemoveBank);
    event._effect = EffectEvent(std::stoi(arg[0]), std::stoi(arg[1]), 0);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Change given param's value
 **/
CmdEditParam::CmdEditParam(CommandHandler* h):
    StrictArgCommand(h, "effect-edit",
        "effect-edit [ID] [ParamIDX] [Value] : Change given effect's parameter",
        3)
{
}
void CmdEditParam::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectEditParam);
    event._effect = EffectEvent(std::stoi(arg[0]), std::stoi(arg[1]), std::stof(arg[2]));

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Get Names and index of parameters for a given type
 **/
CmdShowParam::CmdShowParam(CommandHandler* h):
    StrictArgCommand(h, "effect-list",
        "effect-list [TypeCode] : Get names and indexes of given effect type",
        1)
{
}
void CmdShowParam::perform(std::vector<std::string> arg){

    Plugin* p = EffectFactory::getPlugin(std::stoi(arg[0]));

    if (p != NULL ) {
        
        const Plugin::ParamConfig* params  = p->paramList();
        SFXP::usize_t count = p->paramCount();

        printf("Effect : %s\n", p->name().c_str());
        
        for ( SFXP::usize_t i = 0; i < count; i++ ){

            printf("   - %2u : %s\n", i, params[i].name.c_str());
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
CmdShowPool::CmdShowPool(CommandHandler* h):
    StrictArgCommand(h, "effect-pool",
        "effect-pool [ID] : Get current effect's pool used in process callback",
        1)
{
}
void CmdShowPool::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectShowPool);
    event._effect = EffectEvent(std::stoi(arg[0]), 0, 0);

    _owner->effectHandler()->pushEvent(&event);
}

/**
 * Change current bank
 **/
CmdChangeBank::CmdChangeBank(CommandHandler* h):
    StrictArgCommand(h, "effect-bank",
        "effect-bank [ID] [BankID] : Change effect's current bank",
        2)
{
}
void CmdChangeBank::perform(std::vector<std::string> arg){

    SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_EffectChangeBank);
    event._effect = EffectEvent(std::stoi(arg[0]), std::stoi(arg[1]), 0);

    _owner->effectHandler()->pushEvent(&event);
}
