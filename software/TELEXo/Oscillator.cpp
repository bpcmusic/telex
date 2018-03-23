/*
 * TELEXo Eurorack Module
 * (c) 2016-2018 Brendon Cassidy
 * MIT License
 */

#include "defines.h" 
#include "Arduino.h"
#include "Oscillator.h"
#include "Wavetables.h"

/*
 * Constructor; requires the sampling rate
 */
Oscillator::Oscillator() {
}

/*
 * The primary function called once per sample.
 * Every operation counts here; if you can, do math elsewhere.
 */
float Oscillator::Oscillate() {

  // slew frequency?
  if (_portamento) {
    if (_steps-- <= 0){
      _ulstep = _targetUlstep;
      _portamento = false;
    } else {
      _ulstep = _sign ? _ulstep + _delta : _ulstep - _delta;
    }
  }

  // unsigned long automatically wraps
  _actualPhase += _ulstep;

  // reduce this down to meet the tablesize range
  _location = _actualPhase >> REDUCEBITS;

  // too expensive to do this for the primary and morphing waveforms -
  // we do the PolyBlep calculations once for both
  if (_blepItOne){
    _blepOne = PolyBlepFixed(_actualPhase);
    if (_blepItTwo){
      _blepTwo = PolyBlepFixed((FULLPHASEL - _ulWidth + 1) + _actualPhase);
    }
  }

  // optimized to chained if statements
  if (_wave == SQUARE_WAVE) { 
    _lastValue =  _actualPhase & 0x80000000 ? 32767 : -32767;    
  #ifdef TURBO
    // polyblep frequencies above 20k
    if (_ulstep >= FQ20K){
      _lastValue -= _blepOne;
      _lastValue += _blepTwo;     
    }
  #endif
  } else if (_wave == SAW_WAVE) {  
    // do actual calculations when we have the CPU
    _lastValue = (int)(_actualPhase >> 16) - 32767;
  #ifdef TURBO
    // polyblep frequencies above 20k
    if (_ulstep >= FQ20K)
      _lastValue -= _blepOne;      
  } else if (_wave == TRIANGLE_WAVE) { 
    // do actual calculations when we have the CPU  
    _lastValue = _actualPhase & 0x80000000 ? (int)((FULLPHASEL - _actualPhase) >> 15) - 32767 : (int)(_actualPhase >> 15) - 32767;
  #endif 
  // fall back on the table if we don't have the CPU to spare
  } else if (_wave < WAVETABLECOUNT) {
    #ifdef BASIC
    if (_portamento || _morphing || _doRect){
      // no interpolation or rounding
      _lastValue =  wavetables[_wave][_location];
    } else {  
    #endif
      // interpolate using some fixed math magic (and a floating point scaler)
      _lastValue = wavetables[_wave][_location] + (_actualPhase & PHASEMASK) * _phasescale * (wavetables[_wave][_location + 1] - wavetables[_wave][_location]);
    #ifdef BASIC
    }
    #endif
  } else if (_wave == WAVETABLECOUNT) {
    // generate a new number if we have flipped
    if (_actualPhase < _oldPhase)
      _lastValue = random(0, 65536) - 32878.;
    _oldPhase = _actualPhase;
  } else {
    _lastValue =  0;
  }

  // optimized by moving to chained if statements
  if (_morphing){
    if (_morphWave == 3) {
      _morphValue =  _actualPhase & 0x80000000 ? 32767 : -32767;   
    #ifdef TURBO
      // polyblep frequencies above 20k
      if (_ulstep >= FQ20K){
        _morphValue -= _blepOne;
        _morphValue += _blepTwo;     
      }
    #endif
    } else if (_wave == SAW_WAVE) {  
      // do actual calculations when we have the CPU
      _morphValue = (int)(_actualPhase >> 16) - 32767;
    #ifdef TURBO
      // polyblep frequencies above 20k
      if (_ulstep >= FQ20K)
        _morphValue -= _blepOne;      
    } else if (_morphWave == TRIANGLE_WAVE) { 
      // do actual calculations when we have the CPU 
      _morphValue =  _actualPhase & 0x80000000 ? (int)((FULLPHASEL - _actualPhase) >> 15) - 32767 : (int)(_actualPhase >> 15) - 32767 ;  
    #endif 
    // fall back on the table if we don't have the CPU to spare
    } else if (_morphWave < WAVETABLECOUNT){
      #ifdef BASIC
      _morphValue =  wavetables[_morphWave][_location];
      #else
      _morphValue =  wavetables[_morphWave][_location] + (_actualPhase & PHASEMASK) * _phasescale * (wavetables[_morphWave][_location + 1] - wavetables[_morphWave][_location]);
    #endif
    } else if (_morphWave == WAVETABLECOUNT) {
      if (_actualPhase < _oldPhase)
        _morphValue = random(0, 65536) - 32878.;
      _oldPhase = _actualPhase;
    } else {
      _morphValue =  0;
    }
    _lastValue = (_lastValue * _invMorph + _morphValue * _morph) / MORPHRANGE;
  }

  // optimized by moving to sequential if statements and a rect bool
  if (_doRect){
    if(_rectify == -2){
      _lastValue = -abs(_lastValue);
    } else if (_rectify == -1) {
      _lastValue = _lastValue <= 0 ? _lastValue : 0;
    } else if (_rectify == 1) {
      _lastValue = _lastValue >= 0 ? _lastValue : 0;
    } else if (_rectify == 2) {
      _lastValue = abs(_lastValue);
    }
  }
  
  return _lastValue;
  
}

/*
 * Sets the frequency of the oscillator
 */
void Oscillator::SetFreq(float freq){
  _frequency = freq;
  _portamento = false;
  _ulstep = (int)((freq / SAMPLINGRATE) * FULLPHASE);
  #ifdef DEBUG
  Serial.printf("FQ: %f - %lu\n", freq, _ulstep); 
  #endif
}

/*
 * Targets the frequency for the oscillator (when portamento is active)
 */
void Oscillator::TargetFreq(float freq){
  _frequency = freq;
  if (_stepsCalculated == 0){
    SetFreq(freq);
  } else {
    _targetUlstep = (int)((freq / SAMPLINGRATE) * FULLPHASE);
    if (_targetUlstep > _ulstep){
      _delta = (_targetUlstep - _ulstep) / _stepsCalculated;
      _sign = true;
    } else {
      _delta = (_ulstep - _targetUlstep) / _stepsCalculated;
      _sign = false;
    }
    _steps = _stepsCalculated;
    _portamento = true;
  }
}


/*
 * Sets the freqency via an integer
 */
void Oscillator::SetFrequency(int freq) {
  freq = constrain(freq, 0, SAMPLINGRATEDIV2);
  SetFreq(freq);
}

/*
 * Targets the freqency via an integer (when portamento is active)
 */
void Oscillator::TargetFrequency(int freq){
  freq = constrain(freq, 0, SAMPLINGRATEDIV2);
  TargetFreq(freq);
}

/*
 * Sets a floating point frequency
 */
void Oscillator::SetFloatFrequency(float freq) {
  freq = constrain(freq, 0, SAMPLINGRATEDIV2);
  SetFreq(freq);
}

/*
 * Tarets a floating point frequency (when portamento is active)
 */
void Oscillator::TargetFloatFrequency(float freq){
  freq = constrain(freq, 0, SAMPLINGRATEDIV2);
  TargetFreq(freq);
}

/*
 * Sets the LFO in millihertz (10^3 Hz)
 */
void Oscillator::SetLFO(int millihertz) {
  millihertz = constrain(millihertz, 0, 32767);
  SetFreq((float)millihertz / 1000.);
}

/*
 * Tarets the LFO in millihertz (10^3 Hz)
 */
void Oscillator::TargetLFO(int millihertz) {
  millihertz = constrain(millihertz, 0, 32767);
  TargetFreq((float)millihertz / 1000.);
}

/*
 * Sets the width of the pulse wave (0-100)
 */
void Oscillator::SetWidth(int width) {
  width = constrain(width, 0, 100);
  _fWidth = (float)width / 100.;
  _ulWidth = _fWidth * (FULLPHASE - 1);
  _width = _fWidth * (TABLERANGE - 1);

  #ifdef DEBUG
  Serial.printf("width: %d; _fWidth: %f; _ulWidth: %lu; _width: %d\n",width, _fWidth, _ulWidth, _width);
  #endif
  
}

/*
 * Sets the rectification mode:
 * -2 - full negative rectification (-(abs)value)
 * -1 - half negative rectification (ignores the positive values)
 *  0 - no rectification
 * +1 - half positive rectification (ignores the negative values)
 * +2 - full positive rectification ((abs)value)
 */
void Oscillator::SetRectify(int mode) {
  _rectify = constrain(mode, -2, 2);
  _doRect = _rectify != 0;
}

/*
 * Sets the waveform for the oscillator
 */
void Oscillator::SetWaveform(int wave) {
  _wave = constrain((wave / MORPHRANGE) % (WAVETABLECOUNT + 1), 0, WAVETABLECOUNT);
  #ifdef DEBUG
  Serial.printf("Waveform: %d [%d]\n", _wave, wave);
  #endif
  _morphWave = _wave + 1;
  if (_morphWave > WAVETABLECOUNT) _morphWave = 0;
  _morph = wave % MORPHRANGE;
  _invMorph = MORPHRANGE - _morph;
  _morphing = _morph != 0;

  if (_wave == SAW_WAVE || _morphWave == SAW_WAVE){
    _blepItOne = true;
    if (_wave == SQUARE_WAVE || _morphWave == SQUARE_WAVE) {
      _blepItTwo = true;
    } else {
      _blepItTwo = false;
    }
  } else {
    _blepItOne = false;
    _blepItTwo = false;
  }
}

/*
 * Resets the phase of the oscillator to its default
 */
void Oscillator::ResetPhase(long polarity) {
  if (polarity == 0)
    _actualPhase = _phaseOffset << PHASEBITS;
  else
    _actualPhase = _wave < 2 ? (unsigned long)peaks[_wave] : 0 << REDUCEBITS;
}

/*
 * Sets the oscillator's phase offset
 */
void Oscillator::SetPhaseOffset(int phase) {
  phase = constrain(phase, 0, 16384);
  _phaseDelta = phase - _phaseOffset;
  _phaseOffset = phase;
  _actualPhase += _phaseDelta << PHASEBITS;
}

/*
 * Sets the time for portamento in milliseconds
 */
void Oscillator::SetPortamentoMs(unsigned long milliseconds){
  _stepsCalculated = milliseconds * KRATE;
  if (_portamento && _steps > 0){
      if (_targetUlstep > _ulstep){
        _delta = (_targetUlstep - _ulstep) / _stepsCalculated;
        _sign = true;
      } else {
        _delta = (_ulstep - _targetUlstep) / _stepsCalculated;
        _sign = false;
      }
    _steps = _stepsCalculated;
  }
}

/*
 * Returns the floating point frequency of the oscillator
 */
float Oscillator::GetFrequency(){
  return _frequency;
}

/*
 * PolyBLEP by Tale (slightly modified several times)
 * http://www.kvraudio.com/forum/viewtopic.php?t=375517
 * http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
 * http://research.spa.aalto.fi/publications/papers/smc2010-phaseshaping/phaseshapers.py
*/ 
double Oscillator::PolyBlepFixed(unsigned long ulT){
    // 0 <= t < 1
    if (ulT < _ulstep) {
        t = (double)ulT / _ulstep;
        return (t+t - t*t - 1.0) * 32767;
    }
    // -1 < t < 0
    else if (ulT > FULLPHASE - _ulstep) {
        t = ((double)ulT - FULLPHASE) / _ulstep;
        return (t*t + t+t + 1.0) * 32767;
    }
    // 0 otherwise
    else return 0.0;
}
