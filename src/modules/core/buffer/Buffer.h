/**********************************************************************
 * @file Buffer.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff to store datas
 **********************************************************************/
#ifndef DEF_BUFFER_H
#define DEF_BUFFER_H

#include "../../../core/Utility.h"

/**
 * BlockBuffer class
 * Store N Datas
 **/
template <typename Data, size_t N>
class Buffer{

    public :

        Buffer();
        ~Buffer();

        /**
         * Write given block inside buffer
         * given block must be the same size as buffer template parameter
         **/
        void write( Data const in );
        /**
         * Get block at given delay from current block
         * given delay is in number of blocks
         **/
        Data get( size_t r );

    protected :

        Data *m_buffer;
        size_t m_currentPos;
};

#include "Buffer.hpp"

#endif
