/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#ifndef Oscillator_h
#define Oscillator_h

#include "Arduino.h"

#define TABLERANGE 512
#define TABLERANGEDIV2 256
#define TABLESIZE 513
#define TABLECOUNT 3
#define WAVEFORMS 5
#define MORPHRANGE 1000

#define PHASEBITS 18
#define TABLEBITS 9
#define REDUCEBITS 23 // 32 - TABLEBITS
#define PHASEMASK 8388607 // ( 1 << REDUCEBITS ) - 1
// #define PHASESCALE = 1.1920928955078125e-7 // 1.0 / ( 1 << REDUCEBITS )

#define FULLPHASE 4294967296.

class Oscillator
{
  public:
  
    Oscillator(int samplingRate);
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

    static int *tables[TABLECOUNT];
    static int peaks[WAVEFORMS];

    static int table0[TABLESIZE];
    static int table1[TABLESIZE];
    static int table2[TABLESIZE];
    
  private:

  int _wave = 0;
  int _morphWave = 1;
  int _morph = 0;
  int _invMorph = MORPHRANGE;
  bool _morphing = false;
  int _morphValue = 0;

  float _frequency = 0;
  unsigned long _ulphase = 0;
  unsigned long _ulstep = 0;
  unsigned long _oldPhase = 0;
  int _phaseOffset = 0;
  unsigned long _actualPhase = 0;

  int _location;
  float _remainder;
  
  float _lastValue;
  int _width = TABLERANGEDIV2;

  int _rectify = 0;

  int _samplingRate;
  int _samplingRateDiv2;
  int _krate;

  double _phasescale = 1.0 / ( 1 << REDUCEBITS );

  // portamento
  unsigned long _targetUlstep = 0;
  unsigned long _stepsCalculated = 0;
  unsigned long _steps = 0;
  unsigned long _delta = 0;
  bool _portamento = false;
  bool _sign = true;
  
};

#endif

