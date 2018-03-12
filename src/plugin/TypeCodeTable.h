/**********************************************************************
 * @file TypeCodeTable.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing functions to load plugin list and table that give each
 *  plugin an unique code that will identify it
 **********************************************************************/
#ifndef DEF_TYPE_CODE_TABLE_H
#define DEF_TYPE_CODE_TABLE_H

#include <tinyxml2.h>
#include <iostream>
#include <string>
#include <map>

#include "types.h"

class TCTLoader{

    public :

        typedef std::map<SFXP::tc_t,std::string> TypeCodeTable;

        /**
         * Function that load tctable.xml file
         *  if physical io are missing program must abort because audio
         *  chain will be broken
         * Duplicated type codes will be ignored and warnings will be
         *  raised on log
         * return true if physical input or output are missing or critical error
         *  happened
         **/
        static bool loadTypeCodeTable(std::string path);

        /**
         * Function to get TypeCode of Physical Input Ports
         **/
        static SFXP::id1_t physicalInput();

        /**
         * Function to get TypeCode of Physical Output Ports
         **/
        static SFXP::id1_t physicalOutput();

        /**
         * Function to get map of (TypeCode, Plugin directory) pairs
         **/
        static TypeCodeTable typeCodeTable();

    private :

        static std::string TypeCodeFile;    /**< Path to Typecode File*/
    
        static SFXP::id1_t m_physicalInput; /**< ID of Physical Input Ports */
        static SFXP::id1_t m_physicalOutput;/**< ID of Physical Output Ports */
        
        static TypeCodeTable m_tctable; /**< Map of TypeCode with Plugin's directory */
};

#endif
