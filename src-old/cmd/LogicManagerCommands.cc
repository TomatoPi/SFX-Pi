/**********************************************************************
 * @file LogicManagerCommands.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Commands for Logic Manager
 **********************************************************************/
 #include "LogicManagerCommands.h"
/* *********************** Logic IO Commands ************************ */

/**
 * Function to show Current MCP Configuration
 **/
CmdPrintMCPConfig::CmdPrintMCPConfig():
    StrictArgCommand("logic-mcpconfig",
        "logic-mcpconfig : Show current mcp configuration : print IODIR and IPOL registries",
        0)
{
}
void CmdPrintMCPConfig::perform(std::vector<std::string> arg){

    LogicManager::printMCPConfig();
}

CmdPrintLogicManager::CmdPrintLogicManager():
    StrictArgCommand("logic-show"
        ,"logic-show : Print Logic Manager Configuration and Content"
        ,0)
{
}
void CmdPrintLogicManager::perform(std::vector<std::string> arg){

    LogicManager::printManager();
}

/* **** Edit Footswitches **** */
CmdAddFootTarget::CmdAddFootTarget():
    StrictArgCommand("foot-add"
        ,"foot-add [ID] [TARGET] : Add a notification target to given footswitch"
        ,2)
{
}
void CmdAddFootTarget::perform(std::vector<std::string> arg){

    FootSwitch* foot = LogicManager::getFootSwitch(std::stoi(arg[0]));

    if ( foot != NULL ){

        foot->addTarget(std::stoi(arg[1]));
    }
    else{

        printf("Use \"logic-show\" to view list of footswitches\n");
    }
}

CmdDelFootTarget::CmdDelFootTarget():
    StrictArgCommand("foot-remove"
        ,"foot-remove [ID] [TARGET] : Remove a notification target to given footswitch"
        ,2)
{
}
void CmdDelFootTarget::perform(std::vector<std::string> arg){

    FootSwitch* foot = LogicManager::getFootSwitch(std::stoi(arg[0]));

    if ( foot != NULL ){

        foot->removeTarget(std::stoi(arg[1]));
    }
    else{

        printf("Use \"logic-show\" to view list of footswitches\n");
    }
}

CmdShowFoot::CmdShowFoot():
    StrictArgCommand("foot-show"
        ,"foot-show [ID] : print given footswitch configuration"
        ,1)
{
}
void CmdShowFoot::perform(std::vector<std::string> arg){

    FootSwitch* foot = LogicManager::getFootSwitch(std::stoi(arg[0]));

    if ( foot != NULL ){

        foot->print();
    }
    else{

        printf("Use \"logic-show\" to view list of footswitches\n");
    }
}
