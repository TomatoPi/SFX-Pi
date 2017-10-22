/**********************************************************************
 * @file FilterBase.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Stuff for Band Filtering.
 **********************************************************************/
#include "FilterBase.h"

const float AFilterBase::FilterPole::vsa = 1.0f / 4294967295.0f;

AFilterBase::AFilterBase(){

    // Set last samples to 0
    memset( this, 0, sizeof(AFilterBase) );
}

AFilterBase::~AFilterBase()=default;

AFilterBase::FilterPole::FilterPole():FilterPole(1,1){

}
AFilterBase::FilterPole::FilterPole(float freq, float samplerate){

    // Init all poles to 0
    memset( this, 0, sizeof(AFilterBase::FilterPole) );

    this->setFrequency( freq, samplerate );
} 
