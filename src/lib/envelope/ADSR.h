//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

/**
 * Modifications : Space-Computer
 * 
 * Changement des calculs de croissance et decroissance des pahses A, D et R, il y avais un problème dans 
 * la version initiale : Le taux de croissance ne variais pas en fonction de la distance encore à parcourir...
 */

#ifndef ADRS_h
#define ADRS_h

#include "noyau/log.h"

#define NAME "ADSR"

class ADSR {
public:
	ADSR(void);
	~ADSR(void);
	float process(void);
    float getOutput(void);
    int getState(void);
	void gate(int on);
    void setAttackRate(float rate);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
	void setSustainLevel(float level);
    void setTargetRatioA(float targetRatio);
    void setTargetRatioDR(float targetRatio);
    void reset(void);

    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release
    };

protected:
	int state;
	float output;
	float attackRate;
	float decayRate;
	float releaseRate;
	float attackCoef;
	float decayCoef;
	float releaseCoef;
	float sustainLevel;
    float targetRatioA;
    float targetRatioDR;
    float attackBase;
    float decayBase;
    float releaseBase;
 
    float calcCoef(float rate, float targetRatio);
};

inline float ADSR::process() {
	switch (state) {
        case env_idle:
            break;
        case env_attack:
            output += (1 + targetRatioA - output) * attackCoef;
            if (output >= 1.0) {
                output = 1.0;
                state = env_decay;
                
                sfx::debug(NAME, "State => Decay\n");
            }
            break;
        case env_decay:
            output += (sustainLevel - targetRatioDR - output) * decayCoef;
            if (output <= sustainLevel) {
                output = sustainLevel;
                state = env_sustain;
                
                sfx::debug(NAME, "State => Sustain\n");
            }
            break;
        case env_sustain:
            break;
        case env_release:
            output += (-targetRatioDR - output) * releaseCoef;
            if (output <= 0.0) {
                output = 0.0;
                state = env_idle;
                
                sfx::debug(NAME, "State => Idle\n");
            }
	}
	return output;
}

inline void ADSR::gate(int gate) {
	if (gate)
    {
        output = 0.0;
		state = env_attack;
        
        sfx::debug(NAME, "State => Attack\n");
    }
	else if (state != env_idle)
    {
        state = env_release;
        sfx::debug(NAME, "State => Release\n");
    }
}

inline int ADSR::getState() {
    return state;
}

inline void ADSR::reset() {
    state = env_idle;
    output = 0.0;
}

inline float ADSR::getOutput() {
	return output;
}

#undef NAME
#endif
