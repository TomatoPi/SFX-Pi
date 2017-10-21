/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff for Band Filtering.
 **********************************************************************/
#ifndef DEF_FILTER_CORE_H
#define DEF_FILTER_CORE_H

#include <cstring>
#include <math.h>

/**
 * Interface for encapsulate FilterPole class
 * Herited by Single and Multi pole filters
 * It has a Pure Virtual Destructor used to make class non instanciable
 **/
class AFilterBase{

    protected :

        AFilterBase();
        virtual ~AFilterBase() = 0;
        
        float m_sm1, m_sm2, m_sm3; /**< Filter's last samples */

        inline void suffleBuffer( float in ){

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
                FilterPole(float freq, float samplerate);
                
            private :

                static const float vsa;
                
                inline static float scaleFrequency(float f, float sr){

                    return 2.0f * sin( M_PI * ( f/sr ) );
                }

            private :

                float m_f;
                float m_fBak;
                
                float m_p0, m_p1, m_p2, m_p3;

            public :
            
                inline float compute(float in){

                    this->m_p0 += ( this->m_f * ( in - this->m_p0 ) ) + vsa;
                    this->m_p1 += ( this->m_f * ( this->m_p0 - this->m_p1 ) );
                    this->m_p2 += ( this->m_f * ( this->m_p1 - this->m_p2 ) );
                    this->m_p3 += ( this->m_f * ( this->m_p2 - this->m_p3 ) );

                    return this->m_p3;
                }

                inline void setFrequency(float f, float sr){
                    
                    this->m_fBak = f;
                    this->m_f = scaleFrequency( f, sr );
                }

                inline float getFrequency() const{

                    return m_fBak;
                }
        };
};

#endif
