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

    static std::vector<std::string> parseSimpleString( std::string input,
        const char* sperator, const char antiseparator );

    template <typename T>
    static bool vectorContains(T obj, std::vector<T> list);

}
#include "Parser.hpp"

#endif
