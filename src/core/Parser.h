/**********************************************************************
 * @file Parser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
#ifndef DEF_PARSER_H
#define DEF_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace Parser{

    template <typename Key> using key_cast = Key (*)(std::string);
    template <typename Arg> using arg_cast = Arg (*)(std::string);
    
    /**
     * Template struct used to return parsed objects
     * Key will be an identifier containing first parsed token
     * Arg an object contained at second position
     **/
    template <typename Key, typename Arg>
    struct ParsedObject{

        ParsedObject(Key k);

        Key                             m_key;
        std::vector<std::pair<Key,Arg>> m_arg;
        std::vector<ParsedObject>       m_child;
    };

    static std::vector<std::string> parseSimpleString( std::string input,
        const char* sperator );

    template <typename T>
    static bool vectorContains(T obj, std::vector<T> list);

    /**
     * Function that parse a file in key=value format
     * where value can be a block containing several other blocs
     * 
     * @param flux : input stream of file to parse
     * @param sep : token sepearators
     * @param equal : char separating key and values
     * @param blocko : char indicating block begining
     * @param blocke : char indicating block end
     *
     * @return vector of parsed blocks
     **/
    template <typename Key, typename Arg>
    static std::vector<ParsedObject<Key,Arg>> parseBlockFile(
        std::ifstream& flux,
        const char sep,
        const char equal,
        const char blocko,
        const char blocke );

    template <typename Key, typename Arg>
    static ParsedObject<Key,Arg> parseBlock(
        std::ifstream& flux,
        std::string k,
        const char sep,
        const char equal,
        const char blocko,
        const char blocke );

}
#include "Parser.hpp"

#endif
