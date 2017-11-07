/**********************************************************************
 * @file MatrixDrive.h
 * @author TomatoPi
 * @version 1.0
 *
 * Advanced Distortion Effect
 * Segment audio Signal in several bands ( using Parametric Equalizer )
 * and perform distortion on each band
 * Output a main mixed output and each bands on secondary ports
 * Oposite to Drive effect, MatrixDrive perform the same distortion on
 * positive and Negatives Half-waves 
 **********************************************************************/
#include "MatrixDrive.h"

#define OFFP    EUCST::C_PARAM_COUNT    // Offset from Common Params
#define BC      (OFFP + 0)  // Band Count

#define W       (OFFP + 1)                       // Weigth
#define CF      (W  + MatrixDriveEffect::MXBC)   // Center Frequency
#define QF      (CF + MatrixDriveEffect::MXBC)   // QFactor
#define G       (QF + MatrixDriveEffect::MXBC)   // Gain
#define T       (G  + MatrixDriveEffect::MXBC)   // Clip Type
#define SO      (T  + MatrixDriveEffect::MXBC)   // Softness
#define SA      (SO + MatrixDriveEffect::MXBC)   // Shape

/* ************************* Register Stuff ************************* */
template<> const std::string MDriveReg::NAME = "MatrixDrive";

template<> AbstractEffectUnit* MDriveReg::BUILDER(uint8_t id, uint8_t type){

    return new MatrixDriveEffect( id, type );
}

template<> const std::string MDriveReg::PARNAMES[] = {
    "BandCount",
    "Weigth-1",     "Weigth-2",     "Weigth-3",     "Weigth-4",     "Weigth-5", 
    "CenterFreq-1", "CenterFreq-2", "CenterFreq-3", "CenterFreq-4", "CenterFreq-5",
    "QFactor-1",    "QFactor-2",    "QFactor-3",    "QFactor-4",    "QFactor-5", 
    "Gain-1",       "Gain-2",       "Gain-3",       "Gain-4",       "Gain-5",
    "Type-1",       "Type-2",       "Type-3",       "Type-4",       "Type-5", 
    "Soft-1",       "Soft-2",       "Soft-3",       "Soft-4",       "Soft-5", 
    "Shape-1",      "Shape-2",      "Shape-3",      "Shape-4",      "Shape-5"
    };
template<> const uint8_t MDriveReg::PARCOUNT = 36;

template<> const std::string MDriveReg::PORNAMES[] = {
    "Input", "MainOut",
    "Out-1", "Out-2", "Out-3", "Out-4", "Out-5"
    };

template<> const uint8_t MDriveReg::AI = 1;
template<> const uint8_t MDriveReg::AO = 6;
template<> const uint8_t MDriveReg::MI = 0;
template<> const uint8_t MDriveReg::MO = 0;

template<> const uint16_t MDriveReg::PARSIZE = MDriveReg::PARCOUNT;

/* *********************** Matrix Drive Stuff *********************** */
MatrixDriveEffect::MatrixDriveEffect(uint8_t id, uint8_t type):
    AbstractEffectUnit( id, type, PARCOUNT, PARSIZE),
    m_eq(NULL)
{
    // Setup Equaliser Params are not important
    float f[5] = {130, 261, 523, 1046, 2093};
    float q[5] = {2.51f, 2.51f, 2.51f, 2.51f, 2.51f};
    m_eq = new ParametricEQ( 5, f, q, 48000);

    // Setup JACKUnit
    try{

        m_jackU = new JACKUnit( EUCST::JACK_SERVER, MatrixDriveEffect::NAME );

        m_jackU->registerPorts( MatrixDriveEffect::PORNAMES , MatrixDriveEffect::AI, MatrixDriveEffect::AO );

        m_jackU->registerCallback( MatrixDriveEffect::process, this );

        m_jackU->activateClient();
    }
    catch( std::string const& e ){

        throw e;
    }
}
MatrixDriveEffect::~MatrixDriveEffect(){

    // First deactivate JACK Unit
    m_jackU->deactivateClient();

    // Delete EQ
    delete m_eq;
}

int MatrixDriveEffect::process(jack_nframes_t nframes, void* arg){

    MatrixDriveEffect* unit = (MatrixDriveEffect*)(arg);

    const uint8_t bandCount = (uint8_t)unit->m_paramArray[BC];

    sample_t *in, *out[bandCount];
    in     = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[0], nframes );
    out[0] = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1], nframes );

    // Collect Bands ouput Ports
    for ( uint8_t i = 0; i < bandCount; i++ ){

        out[i+1] = (sample_t*)jack_port_get_buffer( unit->m_jackU->getPorts()[1+i], nframes );
    }

    if ( !unit->m_paramArray[EUCST::IDXBYP] ){

        // Compute each Frames
        float* p = unit->m_paramArray;
        
        for ( jack_nframes_t i = 0; i < nframes; i++ ){

            // Distort each band and add add it to main output
            for ( uint8_t k = 0; k < bandCount; k++ ){

                // Compute EQ Filtering
                out[k+1][i] = unit->m_eq->compute( in[i], k );
                
                // Get correct band and distort it
                out[k+1][i] = (*unit->m_clip[k])( out[k+1][i] * p[G+k], p[SO+k], p[SA+k] );
                out[k+1][i] *= p[EUCST::IDXVOL];

                // Weigth and Add Current Band to main out
                out[0][i] += p[W+k] * out[k+1][i];
            }
            out[0][i] *= p[EUCST::IDXVOL];
        }
    }
    else{

        // If module is bypassed copy input to all outs
        memcpy( out[0], in, nframes * sizeof(sample_t) );
        for ( uint8_t i = 1; i < bandCount; i++ ){
            
            memcpy( out[i], in, nframes * sizeof(sample_t) );
        }
    }

    return 0;
}

void MatrixDriveEffect::update(){

    memcpy( m_paramArray, m_currentBank->second, m_paramSize * sizeof( float ) );

    float weigthSumm = 0;

    // Constrain Band count between 0 and max band count
    m_paramArray[BC] = (m_paramArray[BC] < 0)?0:(m_paramArray[BC]>= MXBC)?MXBC-1:m_paramArray[BC];

    // Update Parametric Equalizer
    for ( uint8_t i = 0; i < (uint8_t)m_paramArray[BC]; i++ ){

        m_eq->setPole( i, m_paramArray[CF + i], m_paramArray[QF + i], m_jackU->getSamplerate() );

        m_clip[i] = AbstractDriveBase::castClipper( static_cast<AbstractDriveBase::TYPE>((int)m_paramArray[T + i]) );

        weigthSumm += m_paramArray[W + i];
    }
    
    // Rescale weigths to have summ equal to 1
    for ( uint8_t i = 0; i < (uint8_t)m_paramArray[BC]; i++ ){

        m_paramArray[W + i] /= weigthSumm;
        std::cout << "Weigth[" << std::to_string(i) << "]=" << std::to_string(m_paramArray[W + i]) << std::endl;
    }
}
