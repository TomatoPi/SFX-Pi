/**********************************************************************
 * @file EndUnit.h
 * @author TomatoPi
 * @version 1.0
 *
 * Specific effect unit placed at the beginning and at the end of the
 * process graph
 * Provide Unique functions for monitoring io
 **********************************************************************/
#include "EndUnit.h"
/* ****************************************************************** */
#define OFFP    EUCST::C_PARAM_COUNT

#define WID     (OFFP + 0)
#define PAN     (WID  + 1)

/*
 * Param array differs from stored bank
 * Voices Weigth are calculated from Pan and Width
 */
#define WL      (PAN)
#define WR      (PAN + 1) 
/* ****************************************************************** */
template<> const std::string EndReg::NAME = "EndUnit";
const std::string EndUnit::CAPTURE = "Capture";
const std::string EndUnit::PLAYBACK = "Playback";

template<> AbstractEffectUnit* EndReg::BUILDER(id1_t id, id1_t type){

    return new EndUnit( id, type );
}

template<> const std::string EndReg::PARNAMES[] = {
    "Width",
    "Panoramic"
    };
template<> const size_t EndReg::PARCOUNT = 2;

template<> const std::string EndReg::PORNAMES[] = {
    "Input-L", "Input-R",
    "Output-L", "Output-R"
    };

template<> const size_t EndReg::AI = 2;
template<> const size_t EndReg::AO = 2;
template<> const size_t EndReg::MI = 0;
template<> const size_t EndReg::MO = 0;

template<> const size_t EndReg::PARSIZE = EndReg::PARCOUNT + 1;
/* ****************************************************************** */
EndUnit::EndUnit( id1_t id, id1_t type ):
    AbstractEffectUnit( id, type, EndUnit::PARCOUNT, EndUnit::PARSIZE ),
    m_isSatL(false),
    m_isSatR(false),
    m_maxL(0),
    m_maxR(0)
{
    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit(
            EUCST::JACK_SERVER,
            (id==SFXP::TC_CAPTURE)?EndUnit::CAPTURE:EndUnit::PLAYBACK
            );

        m_jackU->registerPorts( EndUnit::PORNAMES, EndUnit::AI, EndUnit::AO);

        m_jackU->registerCallback( EndUnit::process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }

/*
    float def[] = { 0, 1, 1, 0.5f };

    this->setParam( 4, def );
    * */
}
EndUnit::~EndUnit(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();
}

int EndUnit::process(jack_nframes_t nframes, void* arg){

    EndUnit* unit = (EndUnit*)(arg);

    sample_t *inL, *inR, *outL, *outR;
    inL  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    inR  = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );
    outL = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[2], nframes );
    outR = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[3], nframes );

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        float *p = unit->m_paramArray;

        sample_t maxL = unit->m_maxL;
        sample_t maxR = unit->m_maxR;
        
        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            outL[i] = p[WL] * inL[i];
            outR[i] = p[WR] * inR[i];

            outL[i] =  p[EUCST::IDXVOL]*0.5f*( (1+p[WID])*outL[i] + (1-p[WID])*outR[i] );
            outR[i] =  p[EUCST::IDXVOL]*0.5f*( (1+p[WID])*outR[i] + (1-p[WID])*outL[i] );

            // Store maximum sample ( negative ones are ignored )
            maxL = (outL[i]>maxL)?outL[i]:maxL;
            maxR = (outR[i]>maxR)?outR[i]:maxR;
        }
        
        unit->m_maxL = maxL;
        unit->m_maxR = maxR;
    }
    else{

        memcpy( outL, inL, nframes * sizeof(sample_t) );
        memcpy( outR, inR, nframes * sizeof(sample_t) );
    }
    
    return 0;
}

void EndUnit::update(){

    memcpy( m_paramArray, m_currentBank->second, m_paramSize * sizeof( float ) );

    // Calculate voices weights
    float pan = m_paramArray[PAN];
    float panL = (pan>0.5f)? 2.0f*(1-pan): 1;
    float panR = (pan<0.5f)? 2.0f*pan: 1;

    m_paramArray[WL] = panL;
    m_paramArray[WR] = panR;
}

bool EndUnit::isSaturated(bool chanel) const{

    if ( chanel )
        return m_isSatR;
        
    return m_isSatL;
}
void EndUnit::resetSaturated(bool chanel){

    if ( chanel ) {
            
        m_isSatR = false;
        m_maxR = 0;
    }
    else{

        m_isSatL = false;
        m_maxL = 0;
    }
}
sample_t EndUnit::getMax(bool chanel) const{

    if ( chanel )
        return m_maxR;

    return m_maxL;
}
