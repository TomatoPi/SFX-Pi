/**********************************************************************
 * @file Buffer.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff to store datas
 **********************************************************************/
template <typename Data, size_t N>
Buffer<Data,N>::Buffer():
    m_buffer(new Data[N]),
    m_currentPos(0)
{
}
template <typename Data, size_t N>
Buffer<Data,N>::~Buffer(){

    delete[] m_buffer;
}

/**
 * Write given block inside buffer
 * given block must be the same size as buffer template parameter
 **/
template <typename Data, size_t N>
void Buffer<Data,N>::write( Data const in ){

    // Move write head
    m_currentPos += 1;
    m_currentPos %= N;

    // Write new data inside buffer
    m_buffer[m_currentPos] = in;
}
/**
 * Get block at given delay from current block
 * given delay is in number of blocks
 **/
template <typename Data, size_t N>
Data Buffer<Data,N>::get( size_t r ){

    return m_buffer[utils::absmod(m_currentPos-r, N)];
}
