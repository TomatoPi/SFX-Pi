/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Serializer.h
 * Author: Space-Computer
 *
 * Created on 11 juin 2018, 14:57
 */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <fstream>
#include <iostream>
#include <string>
#include <exception>

#include "types.h"
#include "log.h"

namespace sfx
{
namespace serial
{
    const sfx::flag_t ControlBits = 0x00;
    
    const sfx::flag_t EffectUnit_Bank_File = 0x01;
    const sfx::flag_t EffectUnit_Link_File = 0x02;
    const sfx::flag_t EffectUnit_Preset_File = 0x03;
    
    /**
     * @brief Function to write datas to a binary stream
     **/
    template<typename T>
    inline void write(std::ofstream& flux, T bits)
    {
        flux.write((char*)&bits, sizeof(T));
    }
    /**
     * @brief Function to read datas from a binary stream
     * @return readed value
     */
    template<typename T>
    inline T read(std::ifstream& flux)
    {
        T bits;
        flux.read((char*)&bits, sizeof(T));
            
        return bits;
    }

    /**
     * @brief Template Specialization to write an std::string to the stream.
     *  First write the number of characters inside the string
     *  and then write each characters
     */
    template<>
    inline void write<std::string>(std::ofstream& flux, std::string text)
    {
        write<sfx::usize_t>(flux, text.size());
        flux.write(text.c_str(), text.size());
    }
    /**
     * @biref Template Specialization to read std::string from a stream
     * @return Readed String
     */
    template<>
    inline std::string read<std::string>(std::ifstream& flux)
    {
        sfx::usize_t size = read<sfx::usize_t>(flux);
        char buffer[size+1];
        flux.read(buffer, size);

        // Add a null char at the end of the buffer to ensure correct
        //  cast to string
        buffer[size] = '\0';

        return std::string(buffer);
    }
    inline sfx::usize_t serializedStringSize(std::string txt)
    {
        return sizeof(sfx::usize_t) + txt.size();
    }

    /**
     * @brief Function to get a control value from given stream
     * @throw ios_base::failure if value readed differs from targetValue
     **/
    template<typename T>
    inline void controlValue(std::ifstream& flux, T targetValue
        ,std::string ErrString="Invalid_Control_Value")
    {
        T value = read<T>(flux);
        if ( value != targetValue )
        {
            std::string err = ErrString;
            err += ":";
            err += value;
            throw std::ios_base::failure(err);
        }
    }
    /**
     * @brief Template Specialization for string control value.
     * @throw ios_base::failure if value readed differs from targetValue
     **/
    template<>
    inline void controlValue<std::string>(std::ifstream& flux
        ,std::string targetValue, std::string ErrString)
    {

        std::string value = read<std::string>(flux);
        if ( value.compare(targetValue) )
        {
            std::string err = ErrString;
            err += ":";
            err += value;
            
            throw std::ios_base::failure(err);
        }
    }
}
}

/*
static void serialize_(std::ofstream& flux, const T* obj);
static int deserialize_(std::ifstream& flux, T* obj);
//*/

#endif /* SERIALIZER_H */