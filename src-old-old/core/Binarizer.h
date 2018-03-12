/**********************************************************************
 * @file Binarizer.h
 * @author TomatoPi
 * @version 1.0
 *
 * Provide functions to write variables to binary flux
 **********************************************************************/
#ifndef DEF_BINARIZER_H
#define DEF_BINARIZER_H

#include <fstream>
#include <string>
#include "Typedefs.h"

template <typename T>
inline void writeBits(std::ofstream& flux, T data){

    printf("Generic\n");
    flux.write((char*)&data, sizeof(T));
}

template<>
inline void writeBits<flag_t>(std::ofstream& flux, flag_t flag){

    printf("Flag\n");
    flux.write((char*)&flag, sizeof(flag_t));
}

template<>
inline void writeBits<id1_t>(std::ofstream& flux, id1_t id1){

    printf("ID1\n");
    flux.write((char*)&id1, sizeof(id1_t));
}

template<>
inline void writeBits<id2_t>(std::ofstream& flux, id2_t id2){

    printf("ID2\n");
    flux.write((char*)&id2, sizeof(id2_t));
}

template<>
inline void writeBits<size_t>(std::ofstream& flux, size_t size){

    printf("Size\n");
    flux.write((char*)&size, sizeof(size_t));
}

template<>
inline void writeBits<std::string>(std::ofstream& flux, std::string text){

    printf("Text\n");
    writeBits<size_t>(flux, text.size());
    flux.write(text.c_str(), text.size());
}000

#endif
