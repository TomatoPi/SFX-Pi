/**********************************************************************
 * @file log.h
 * @author TomatoPi
 * @version 1.0
 *
 * Logging functions for the program
 **********************************************************************/
#ifndef DEF_LOG_H
#define DEF_LOG_H

#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

namespace SFXPlog{

    static inline std::ostream& source(const std::string& s) {

        return std::cout << "[" << s << "]";
    }

    static inline std::ostream& log(const std::string& s) {

        return source(s) << " : INFO : ";
    }

    static inline std::ostream& wrn(const std::string& s) {

        return source(s) << " : WARNING : ";
    }

    static inline std::ostream& err(const std::string& s) {

        return source(s) << " : ERROR : ";
    }

    static inline std::ostream& fatal(const std::string& s) {

        return source(s) << " : FATAL : ";
    }

    static inline std::ostream& todo(const std::string& s) {

        return source(s) << " : TODO : ";
    }
    
    #ifdef __DEBUG__
    static inline std::ostream& debug(const std::string& s) {
        
        return source(s) << " : DEBUG : ";
    }
    #endif
};

#ifdef __cplusplus
}
#endif

#endif
