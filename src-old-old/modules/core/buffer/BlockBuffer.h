/**********************************************************************
 * @file BlockBuffer.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff to store blocks of datas
 **********************************************************************/
#ifndef DEF_BLOCK_BUFFER_H
#define DEF_BLOCK_BUFFER_H

#include "../../../core/Utility.h"

/**
 * BlockBuffer class
 * Store N block of S Datas
 **/
template <typename Data, usize_t S, usize_t N>
class BlockBuffer{

    public :

        BlockBuffer();
        ~BlockBuffer();

        /**
         * Write given block inside buffer
         * given block must be the same size as buffer template parameter
         **/
        void write( Data const *block, usize_t size );
        /**
         * Get block at given delay from current block
         * given delay is in number of blocks
         **/
        Data* get( usize_t r );

    protected :

        Data *m_buffer;
        usize_t m_currentBlock;
};

#include "BlockBuffer.hpp"

#endif
