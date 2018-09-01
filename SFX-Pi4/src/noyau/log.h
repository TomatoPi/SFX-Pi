/*
 * Copyright (C) 2018 Space-Computer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   log.h
 * Author: Space-Computer
 *
 * Created on 27 juin 2018, 01:31
 */

#ifndef LOG_H
#define LOG_H

#include <iomanip>
#include <iostream>
#include <string>
#include <ctime>

namespace sfx
{
    static std::ostream& sfxStream = std::cout;
    static const int stringFormatDefaultBufferSize = 127;
    
    enum ModifierCode 
    {
        RESET       = 0,
        // Font Modifiers
        FONT_BOLD   = 1,
        FONT_FAINT  = 2,
        FONT_ITALIC = 3,
        // Text color modifiers
        FG_BLACK    = 30,
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_MAGENTA  = 35,
        FG_CYAN     = 36,
        FG_WHITE    = 37,
        FG_DEFAULT  = 39
    };
    
    class LogModifier {
        ModifierCode code;
    public:
        LogModifier(ModifierCode pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const LogModifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
    
    template<typename T> T inline formatArgs(const T& a)
    {
        return a;
    }
    
    template<typename T> const T* formatArgs(const std::basic_string<T>& a)
    {
        return a.c_str();
    }

    template <typename ...Args>
    static inline void source(const std::string& s, const std::string& prior, const ModifierCode& code, const char* fmt, const Args& ...args)
    {
        std::time_t t = std::time(nullptr);
        sfxStream << LogModifier(code)
                << std::put_time(std::gmtime(&t), "%T : ")
                << std::setw(7) << std::left << prior 
                << " : [" << s << "] : ";
        printf(fmt, formatArgs(args)...);
        sfxStream << LogModifier(RESET);
    }

    template <typename ...Args>
    static inline void log(const std::string& s, const char* fmt, const Args& ...args)
    {
        source(s, "INFO", FG_DEFAULT, fmt, args...);
    }

    template <typename ...Args>
    static inline void wrn(const std::string& s, const char* fmt, const Args& ...args)
    {
        source(s, "WARNING", FG_MAGENTA, fmt, args...);
    }

    template <typename ...Args>
    static inline void err(const std::string& s, const char* fmt, const Args& ...args)
    {
        source(s, "ERROR", FG_RED, fmt, args...);
    }

    template <typename ...Args>
    static inline void fatal(const std::string& s, const char* fmt, const Args& ...args)
    {
        sfxStream << LogModifier(FONT_BOLD);
        source(s, "FATAL", FG_RED, fmt, args...);
    }

    template <typename ...Args>
    static inline void todo(const std::string& s, const char* fmt, const Args& ...args)
    {
        source(s, "TODO", FG_CYAN, fmt, args...);
    }
    
    template <typename ...Args>
    static inline void debug(const std::string& s, const char* fmt, const Args& ...args)
    {
        #ifndef NDEBUG
        source(s, "DEBUG", FG_GREEN, fmt, args...);
        #endif
    }
    
    template <typename ...Args>
    static inline std::string formatString(const char* fmt, const Args& ...args)
    {
        char buffer[stringFormatDefaultBufferSize];
        int wsize = snprintf(buffer, stringFormatDefaultBufferSize, fmt, formatArgs(args)...);
        // If buffer was large enougth
        if (wsize < stringFormatDefaultBufferSize)
        {
            return std::string(buffer);
        }
        // Else alocate a sufficiant buffer
        debug("TomatoLogger", "Default Buffer wasn't large enought to format the string : Default:%i Required:%i\n"
                ,stringFormatDefaultBufferSize
                ,wsize);
        
        char* buffer2 = new char[wsize+1];
        snprintf(buffer2, stringFormatDefaultBufferSize, fmt, formatArgs(args)...);
        std::string res(buffer2);
        delete[] buffer2;
        return res;
    }
    
    template <typename ...Args>
    static inline void print(const char* fmt, const Args& ...args)
    {
        sfxStream << formatString(fmt, args...);
    }
    
    /*
    static inline void testMyBeautyfullLogger()
    {
        std::string name = "TomatoLogger";
        log(name, "Hello I'm the Tomato Logging Library\n");
        wrn(name, "You shouldn't continue on this way\n");
        err(name, "I don't know what happends, but it's not a good thing\n");
        fatal(name, "Chaaarlie !, Thats really hurt !!!\n");
        todo(name, "Sorry but this feature is not Available\n");
        debug(name, "Pschhhht! This is a secret message, don't talk about it\n");
    }
    //*/
};

#endif /* LOG_H */

