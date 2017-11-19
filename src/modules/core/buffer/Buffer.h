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
template <size_t N>
class Buffer{

    public :

        Buffer();
        ~Buffer();

        /**
         * Write given block inside buffer
         * given block must be the same size as buffer template parameter
         **/
        void write( sample_t const in );
        /**
         * Get block at given delay from current block
         * given delay is in number of blocks
         **/
        sample_t get( size_t r );

        /**
         * Get linear interpolation at wanted delay from current block
         * given delay is number of blocks
         **/
        sample_t getLI( float r );

    protected :

        sample_t *m_buffer;
        size_t m_currentPos;
};

#include "Buffer.hpp"

#endif
