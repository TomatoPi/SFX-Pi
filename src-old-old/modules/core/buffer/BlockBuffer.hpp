/**********************************************************************
 * @file BlockBuffer.hpp
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff to store blocks of datas
 **********************************************************************/
/**
 * BlockBuffer class
 * Store N block of S Datas
 **/
template <typename Data, usize_t S, usize_t N> BlockBuffer<Data,S,N>::BlockBuffer():
    m_buffer(new Data[S*N]),
    m_currentBlock(0)
{
    printf("Created buffer of %luo datas, with %lu blocks of %lu objects : total is %lu objects\n",
        sizeof(Data), N, S, S*N);
}
template <typename Data, usize_t S, usize_t N> BlockBuffer<Data,S,N>::~BlockBuffer(){

    delete[] m_buffer;
}

/**
 * Write given block inside buffer
 * given block must be the same size as buffer template parameter
 **/
template <typename Data, usize_t S, usize_t N>
void BlockBuffer<Data,S,N>::write( Data const *block, usize_t size ){

    if ( size == S ){

        //printf("Write : Before %6lu, ", m_currentBlock);
        // First move current block position
        m_currentBlock = utils::absmod( m_currentBlock + S, S*N );
        //printf("After %6lu\n", m_currentBlock);
        
        // Then copy blocks
        std::memcpy( m_buffer + m_currentBlock, block, S * sizeof(Data));
    }
    else{
        printf("ERROR : Invalid Block Size !!! %lu %lu\n", S, size);
    }
}
/**
 * Get block at given delay from current block
 * given delay is in number of blocks
 **/
template <typename Data, usize_t S, usize_t N>
Data* BlockBuffer<Data,S,N>::get( usize_t r ){
/*
    printf("Get : Wanted %6lu, Pos %6lu (%6lu), Returned %6lu (%6lu) ... ",
        r,
        m_currentBlock, m_currentBlock / S,
        utils::absmod((long)m_currentBlock - (long)S*r, S*N),
        utils::absmod((long)m_currentBlock - (long)S*r, S*N)/S
        );
        */
    return m_buffer + utils::absmod((long)m_currentBlock - (long)S*r, S*N);
}
