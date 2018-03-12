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

namespace Parser{

    static std::vector<std::string> parseSimpleString( std::string input,
        const char* sperator, const char antiseparator );

}
#include "Parser.hpp"

#endif
