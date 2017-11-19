/**********************************************************************
 * @file Buffer.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff to store datas
 **********************************************************************/
template <size_t N>
Buffer<N>::Buffer():
    m_buffer(new sample_t[N]),
    m_currentPos(0)
{
}
template <size_t N>
Buffer<N>::~Buffer(){

    delete[] m_buffer;
}

/**
 * Write given block inside buffer
 * given block must be the same size as buffer template parameter
 **/
template <size_t N>
void Buffer<N>::write( sample_t const in ){

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
template <size_t N>
sample_t Buffer<N>::get( size_t r ){

    return m_buffer[utils::absmod(m_currentPos-r, N)];
}

/**
 * Get linear interpolation at wanted delay from current block
 * given delay is number of blocks
 **/
template <size_t N>
sample_t Buffer<N>::getLI( float r ){

    return utils::linearInterpolation( get(floor(r)), get(ceil(r)), r );
}
