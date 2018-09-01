/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.1
 *
 * File Providing Stuff for Band Filtering.
 **********************************************************************/
#ifndef DEF_FILTER_CORE_H
#define DEF_FILTER_CORE_H

#include <cstring>
#include <math.h>

#include "noyau/types.h"

/**
 * Interface for encapsulate FilterPole class
 * Herited by Single and Multi pole filters
 * It has a Pure Virtual Destructor used to make class non instanciable
 **/
class AFilterBase{

    protected :

        AFilterBase(sfx::usize_t order);
        virtual ~AFilterBase() = 0;

        sfx::usize_t _order;
        sfx::sample_t m_sm1, m_sm2, m_sm3; /**< Filter's last samples */

        inline void shuffleBuffer( sfx::sample_t in ){

            m_sm3 = m_sm2;
            m_sm2 = m_sm1;
            m_sm1 = in;
        }

        /**
         * Filter Pole centered at a given frequency
         * Compute 4 pole filtering of a given sample
         **/
        class FilterPole{

            public :

                FilterPole();
                FilterPole(float freq, float samplerate, sfx::usize_t order=4);
                ~FilterPole();
                
            private :
                
                inline static float scaleFrequency(float f, float sr){

                    return 2.0f * sin( M_PI * ( f/sr ) );
                }

            private :

                float _f;
                float _fBak;
                sfx::usize_t _order;
                
                sfx::sample_t* _poles;

            public :

                static const float vsa;
            
                inline sfx::sample_t compute(sfx::sample_t in){

                    _poles[0] += ( _f * ( in - _poles[0] ) ) + vsa;

                    for (sfx::usize_t i = 1; i < _order; i++) {
                        
                        _poles[i] += ( _f * ( _poles[i-1] - _poles[i] ) );
                    }
                    
                    return _poles[_order-1];
                }

                inline void setFrequency(float f, float sr){
                    
                    this->_fBak = f;
                    this->_f = scaleFrequency( f, sr );
                }

                inline float getFrequency() const{

                    return _fBak;
                }

                inline void setOrder(sfx::usize_t order) {

                    if (order < 1) order = 1;

                    if (_poles) delete[] _poles;
                    
                    _poles = new sfx::sample_t[order];
                    _order = order;

                    memset(_poles, 0, order * sizeof(sfx::sample_t));
                }
                
                inline sfx::usize_t getOrder() const {

                    return _order;
                }
        };
};

#endif
