/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//#ifndef synth_waveform_h_
//#define synth_waveform_h_
#pragma once

#include <Arduino.h>
#include "AudioStream.h"
#include "arm_math.h"

// waveforms.c
extern "C" {
extern const int16_t AudioWaveformSine[257];
}


#define WAVEFORM_SINE              0
#define WAVEFORM_SAWTOOTH          1
#define WAVEFORM_SQUARE            2
#define WAVEFORM_TRIANGLE          3
#define WAVEFORM_ARBITRARY         4
#define WAVEFORM_PULSE             5
#define WAVEFORM_SAWTOOTH_REVERSE  6
#define WAVEFORM_SAMPLE_HOLD       7
#define WAVEFORM_TRIANGLE_VARIABLE 8
#define WAVEFORM_BANDLIMIT_SAWTOOTH  9
#define WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE 10
#define WAVEFORM_BANDLIMIT_SQUARE 11
#define WAVEFORM_BANDLIMIT_PULSE  12

#define WAVEFORM_MULTISAW 30
#define WAVEFORM_SHRUTHI_ZSYNC 33
#define WAVEFORM_BRAIDS_CSAW 38

#define DIV32768 3.0517578E-5f
#define DIV255 0.00392156

typedef struct P3_step_state
{
  int offset ;
  bool positive ;
} P3_step_state ;

class P3_BandLimitedWaveform
{
public:
  P3_BandLimitedWaveform (void) ;
  int16_t generate_sawtooth (uint32_t new_phase, int i) ;
  int16_t generate_square (uint32_t new_phase, int i) ;
  int16_t generate_pulse (uint32_t new_phase, uint32_t pulse_width, int i) ;
  void init_sawtooth (uint32_t freq_word) ;
  void init_square (uint32_t freq_word) ;
  void init_pulse (uint32_t freq_word, uint32_t pulse_width) ;
  

private:
  int32_t lookup (int offset) ;
  void insert_step (int offset, bool rising, int i) ;
  int32_t process_step (int i) ;
  int32_t process_active_steps (uint32_t new_phase) ;
  int32_t process_active_steps_saw (uint32_t new_phase) ;
  int32_t process_active_steps_pulse (uint32_t new_phase, uint32_t pulse_width) ;
  void new_step_check_square (uint32_t new_phase, int i) ;
  void new_step_check_pulse (uint32_t new_phase, uint32_t pulse_width, int i) ;
  void new_step_check_saw (uint32_t new_phase, int i) ;

  
  uint32_t phase_word ;
  int32_t dc_offset ;
  P3_step_state states [32] ; // circular buffer of active steps
  int newptr ;         // buffer pointers into states, AND'd with PTRMASK to keep in buffer range.
  int delptr ;
  int32_t  cyclic[16] ;    // circular buffer of output samples
  bool pulse_state ;
  uint32_t sampled_width ; // pulse width is sampled once per waveform
};


class P3_AudioSynthWaveform : public AudioStream
{
public:
	P3_AudioSynthWaveform(void) : AudioStream(0,NULL),
		phase_accumulator(0), phase_increment(0), phase_offset(0),
		magnitude(0), pulse_width(0x40000000),
		arbdata(NULL), sample(0), tone_type(WAVEFORM_SINE),
		tone_offset(0) {
	}

	void frequency(float freq) {
		if (freq < 0.0f) {
			freq = 0.0;
		} else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2.0f) {
			freq = AUDIO_SAMPLE_RATE_EXACT / 2.0f;
		}
		phase_increment = freq * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT);
		if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;
	}
	void phase(float angle) {
		if (angle < 0.0f) {
			angle = 0.0;
		} else if (angle > 360.0f) {
			angle = angle - 360.0f;
			if (angle >= 360.0f) return;
		}
		phase_offset = angle * (float)(4294967296.0 / 360.0);
	}
	void amplitude(float n) {	// 0 to 1.0
		if (n < 0) {
			n = 0;
		} else if (n > 1.0f) {
			n = 1.0;
		}
		magnitude = n * 65536.0f;
	}
	void offset(float n) {
		if (n < -1.0f) {
			n = -1.0f;
		} else if (n > 1.0f) {
			n = 1.0f;
		}
		tone_offset = n * 32767.0f;
	}
	void pulseWidth(float n) {	// 0.0 to 1.0
		if (n < 0) {
			n = 0;
		} else if (n > 1.0f) {
			n = 1.0f;
		}
		pulse_width = n * 4294967296.0f;
	}
	void begin(short t_type) {
		phase_offset = 0;
		tone_type = t_type;
		if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
		  band_limit_waveform.init_square (phase_increment) ;
		else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
		  band_limit_waveform.init_pulse (phase_increment, pulse_width) ;
		else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
		  band_limit_waveform.init_sawtooth (phase_increment) ;
	}
	void begin(float t_amp, float t_freq, short t_type) {
		amplitude(t_amp);
		frequency(t_freq);
		phase_offset = 0;
		begin (t_type);
	}
	void arbitraryWaveform(const int16_t *data, float maxFreq) {
		arbdata = data;
	}
	virtual void update(void);



private:
	uint32_t phase_accumulator;
	uint32_t phase_increment;
	uint32_t phase_offset;
	int32_t  magnitude;
	uint32_t pulse_width;
	const int16_t *arbdata;
	int16_t  sample; // for WAVEFORM_SAMPLE_HOLD
	short    tone_type;
	int16_t  tone_offset;
        P3_BandLimitedWaveform band_limit_waveform ;
};


class P3_AudioSynthWaveformModulated : public AudioStream
{
public:
	P3_AudioSynthWaveformModulated(void) : AudioStream(2, inputQueueArray),
		phase_accumulator(0), phase_increment(0), modulation_factor(32768),
		magnitude(0), arbdata(NULL), sample(0), tone_offset(0),
		tone_type(WAVEFORM_SINE), modulation_type(0) {
	}

	void frequency(float freq) {
		if (freq < 0.0f) {
			freq = 0.0;
		} else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2.0f) {
			freq = AUDIO_SAMPLE_RATE_EXACT / 2.0f;
		}
		phase_increment = freq * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT);
		if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;
	}
	void amplitude(float n) {	// 0 to 1.0
		if (n < 0) {
			n = 0;
		} else if (n > 1.0f) {
			n = 1.0f;
		}
		magnitude = n * 65536.0f;
	}
	void offset(float n) {
		if (n < -1.0f) {
			n = -1.0f;
		} else if (n > 1.0f) {
			n = 1.0f;
		}
		tone_offset = n * 32767.0f;
	}
	void begin(short t_type) {
		tone_type = t_type;
		if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
		  band_limit_waveform.init_square (phase_increment) ;
		else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
		  band_limit_waveform.init_pulse (phase_increment, 0x80000000u) ;
		else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
		  band_limit_waveform.init_sawtooth (phase_increment) ;
	}
	void begin(float t_amp, float t_freq, short t_type) {
		amplitude(t_amp);
		frequency(t_freq);
		begin (t_type) ;
	}
	void arbitraryWaveform(const int16_t *data, float maxFreq) {
		arbdata = data;
	}
	void frequencyModulation(float octaves) {
		if (octaves > 12.0f) {
			octaves = 12.0f;
		} else if (octaves < 0.1f) {
			octaves = 0.1f;
		}
		modulation_factor = octaves * 4096.0f;
		modulation_type = 0;
	}
	void phaseModulation(float degrees) {
		if (degrees > 9000.0f) {
			degrees = 9000.0f;
		} else if (degrees < 30.0f) {
			degrees = 30.0f;
		}
		modulation_factor = degrees * (float)(65536.0 / 180.0);
		modulation_type = 1;
	}
	virtual void update(void);

	void sync() {syncIn = true;}
	
	bool getSync() {
	  bool temp = syncOut;
	  syncOut = false;
	  return temp;
	}
	
	uint32_t getPhaseAccumulator() { return phase_accumulator;}

	struct SawSwarmState {
	  uint32_t phase[6];
	  uint32_t increments[5];
	  uint32_t data_qs_phase[4];
	  int32_t filter_state[2][2];
	  int32_t dc_blocked;
	  int32_t lp;
	  int32_t bp;
	} state_saw;
	
	inline int32_t ThisBlepSample(uint32_t t) {
	    if (t > 65535) {
	      t = 65535;
	    }
	    return t * t >> 18;
	}
  
  	inline int32_t NextBlepSample(uint32_t t) {
	    if (t > 65535) {
     	 t = 65535;
	    }
	    t = 65535 - t;
	    return -static_cast<int32_t>(t * t >> 18);
	  }

	uint8_t Osc_data_cr_decimate;
	uint16_t Osc_data_cr_state;
	uint16_t OscData_sec_phase;
	uint16_t osc_par_a = 0;
	uint16_t osc_par_b = 0;
	uint32_t phaseOld_ = 0;
	int16_t par_a_mod_ = 0; // parameter_A
	int16_t par_b_mod_ = 0; // parameter_B
	int32_t parameter_[2];

private:
	audio_block_t *inputQueueArray[2];
	uint32_t phase_accumulator;
	uint32_t phase_increment;
	uint32_t modulation_factor;
	int32_t  magnitude;
	const int16_t *arbdata;
	uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
	int16_t  sample; // for WAVEFORM_SAMPLE_HOLD
	int16_t  tone_offset;
	uint8_t  tone_type;
	uint8_t  modulation_type;
        P3_BandLimitedWaveform band_limit_waveform ;
	bool syncIn = false;
	bool syncOut = false;
	uint32_t state_saw_phase_[4];
	int32_t next_sample_;
	bool high_;
	int16_t discontinuity_depth_;
	int16_t aux_parameter_;
};

const uint16_t wav_res_sine16[] PROGMEM = {
    0, 10, 39, 88, 157, 245, 352, 479,
    625, 790, 974, 1178, 1400, 1641, 1901, 2179,
    2475, 2790, 3122, 3472, 3840, 4225, 4627, 5045,
    5481, 5932, 6400, 6884, 7382, 7897, 8426, 8969,
    9527, 10098, 10684, 11282, 11893, 12517, 13153, 13800,
    14459, 15129, 15809, 16500, 17200, 17909, 18628, 19355,
    20090, 20832, 21582, 22338, 23100, 23869, 24642, 25421,
    26203, 26990, 27780, 28574, 29369, 30167, 30966, 31767,
    32568, 33369, 34170, 34969, 35768, 36565, 37359, 38151,
    38940, 39724, 40505, 41281, 42052, 42818, 43577, 44330,
    45076, 45815, 46546, 47268, 47982, 48687, 49383, 50068,
    50743, 51408, 52061, 52703, 53332, 53950, 54555, 55147,
    55725, 56290, 56840, 57377, 57898, 58405, 58896, 59372,
    59831, 60275, 60702, 61112, 61506, 61882, 62241, 62582,
    62906, 63211, 63499, 63767, 64018, 64249, 64462, 64656,
    64831, 64987, 65123, 65240, 65338, 65416, 65475, 65514,
    65534, 65534, 65514, 65475, 65416, 65338, 65240, 65123,
    64987, 64831, 64656, 64462, 64249, 64018, 63767, 63499,
    63211, 62906, 62582, 62241, 61882, 61506, 61112, 60702,
    60275, 59831, 59372, 58896, 58405, 57898, 57377, 56840,
    56290, 55725, 55147, 54555, 53950, 53332, 52703, 52061,
    51408, 50743, 50068, 49383, 48687, 47982, 47268, 46546,
    45815, 45076, 44330, 43577, 42818, 42052, 41281, 40505,
    39724, 38940, 38151, 37359, 36565, 35768, 34969, 34170,
    33369, 32568, 31767, 30966, 30167, 29369, 28574, 27780,
    26990, 26203, 25421, 24642, 23869, 23100, 22338, 21582,
    20832, 20090, 19355, 18628, 17909, 17200, 16500, 15809,
    15129, 14459, 13800, 13153, 12517, 11893, 11282, 10684,
    10098, 9527, 8969, 8426, 7897, 7382, 6884, 6400,
    5932, 5481, 5045, 4627, 4225, 3840, 3472, 3122,
    2790, 2475, 2179, 1901, 1641, 1400, 1178, 974,
    790, 625, 479, 352, 245, 157, 88, 39,
    10};

//#endif
