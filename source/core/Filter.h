#ifndef DEF_FILTER_H
#define DEF_FILTER_H

#include "Utility.h"

/*
*	Three bands EQ class
*/
class Filter_3EQ{
	
	public :
	
		/*
		*	Main Constructor
		*	lf : low cutoff frequency
		*	hf : high cutoff frequency
		*	sr : samplerate
		*/
		Filter_3EQ(float lf, float hf, int sr);
		
		/*
		*	Compute three bands eq on given sample
		*	gl : low band gain
		*	gm : mid band gain
		*	gh : high band gain
		*/
		sample_t compute(sample_t sample, float gl, float gm, float gh);	
		
		void set_freq(float lf, float hf);
		void set_lf(float lf);
		void set_hf(float hf);
		
		float get_lf() const;
		float get_hf() const;
	
	private :
	
		int samplerate;
		
		//Lowpass filter
		float fl; 		//Filter frequency scaled at samplerate
		float fl_bak;	//Filter frequency in Hz
		float f1p0; 	//Filter poles ....
		float f1p1;
		float f1p2;
		float f1p3;

		//Highpass filter
		float fh;
		float fh_bak;
		float f2p0;
		float f2p1;
		float f2p2;
		float f2p3;
		
		//Save last samples
		float sm1; // Sample - 1
		float sm2; // - 2
		float sm3; // - 3
	
};

float spi_scale_frequency(float f, int sr);

/*
*	Comb filter class
*/
class Filter_Comb{
	
	public :
		
		/*
		*	Comb filter constructor
		*	f : Firt peak frequency
		*	sr : samplerate
		*/
		Filter_Comb(float f, float d, float fb, int sr);
		Filter_Comb(float d, float fb, int s, int sr, int n);
		~Filter_Comb();
		
		/*
		*	Compute comb filtering on given sample
		*/
		sample_t compute(sample_t sample);
		
		void set_size(int s);
		void set_freq(float f);
		void set_damp(float d);
		void set_feedback(float f);
		
		int get_size() const;
		float get_freq() const;
		float get_damp() const;
		float get_feed() const;
		
	private :
	
		int samplerate;
	
		sample_t *buffer;
		int size;
		int index;
		
		sample_t store;
		
		float damp1;
		float damp2;
		
		float feedback;
};

int spi_scale_comb(float f, int sr);

/*
*	Allpass filter class
*/
class Filter_Allpass{
	
	public :
		
		/*
		*	Allpass Filter constructor
		*	f : feedback
		*	s : size
		*/
		Filter_Allpass(float f, int s);
		~Filter_Allpass();
		
		/*
		*	Compute allpass filtering on the given sample
		*/
		sample_t compute(sample_t sample);
		
		void set_size(int s);
		void set_feedback(float f);
		
		int get_size() const;
		float get_feedback() const;
		
	private :
		
		sample_t *buffer;
		int size;
		int index;
		
		float feedback;
};

#endif