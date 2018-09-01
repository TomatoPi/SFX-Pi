/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff for Band Filtering.
 **********************************************************************/
#include "FilterBase.h"

const float AFilterBase::FilterPole::vsa = 1.0f / 4294967295.0f;

AFilterBase::AFilterBase(sfx::usize_t order):_order(order)
    ,m_sm1(0), m_sm2(0), m_sm3(0)
{
}

AFilterBase::~AFilterBase()=default;

AFilterBase::FilterPole::FilterPole():FilterPole(1,1){

}
AFilterBase::FilterPole::FilterPole(float freq, float samplerate, sfx::usize_t order):
    _poles(nullptr)
{
    this->setFrequency(freq, samplerate);
    this->setOrder(order);
} 
AFilterBase::FilterPole::~FilterPole() {

    delete[] _poles;
}
