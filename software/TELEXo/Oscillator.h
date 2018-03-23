/*
 * TELEXo Eurorack Module
 * (c) 2016, 2017 Brendon Cassidy
 * MIT License
 */
 
#ifndef Oscillator_h
#define Oscillator_h

#include "Arduino.h"

#define TABLERANGE 512
#define TABLERANGEDIV2 256
#define TABLESIZE 513

#define MORPHRANGE 100

#define PHASEBITS 18
#define TABLEBITS 9
#define REDUCEBITS 23 // 32 - TABLEBITS
#define PHASEMASK 8388607 // ( 1 << REDUCEBITS ) - 1
// #define PHASESCALE = 1.1920928955078125e-7 // 1.0 / ( 1 << REDUCEBITS )

#define FULLPHASE 4294967296.
#define FULLPHASEL 4294967295
#define HALFPHASE 2147483648

class Oscillator
{
  public:
  
    Oscillator();
    float Oscillate();
    
    void SetFrequency(int freq);
    void TargetFrequency(int freq);
    void SetFloatFrequency(float freq);
    void TargetFloatFrequency(float freq);
    void SetLFO(int millihertz);
    void TargetLFO(int millihertz);

    void SetWaveform(int wave);
    void ResetPhase(long polarity);
    void SetPhaseOffset(int phase);
    void SetWidth(int width);
    void SetRectify(int mode);

    void SetPortamentoMs(unsigned long milliseconds);

    float GetFrequency();
    
  protected:


    void SetFreq(float freq);
    void TargetFreq(float freq);

    double PolyBlepFixed(unsigned long ulT);

    const int peaks[2] = { 128, 256 };
  
  private:

  uint16_t _wave = 0;
  uint8_t _morphWave = 1;
  int _morph = 0;
  int _invMorph = MORPHRANGE;
  bool _morphing = false;
  int _morphValue = 0;

  float _frequency = 0;
  unsigned long _ulstep = 0;
  unsigned long _oldPhase = 0;
  int _phaseOffset = 0;
  unsigned long _actualPhase = 0;
  int _phaseDelta = 0;
  
  int _location;
  float _remainder;
  
  int _lastValue;
  
  int _width = TABLERANGEDIV2;
  float _fWidth = .5;
  unsigned long _ulWidth = FULLPHASEL >> 1;

  int8_t _rectify = 0;
  bool _doRect = false;

  double _phasescale = 1.0 / ( 1 << REDUCEBITS );

  // portamento
  unsigned long _targetUlstep = 0;
  unsigned long _stepsCalculated = 0;
  unsigned long _steps = 0;
  unsigned long _delta = 0;
  bool _portamento = false;
  bool _sign = true;

  // polyblep
  double t = 0.0;
  bool _blepItOne = false;
  bool _blepItTwo = false;
  int _blepOne = 0.0;
  int _blepTwo = 0.0;

};

#endif

