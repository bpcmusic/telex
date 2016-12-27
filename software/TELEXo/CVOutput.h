/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#ifndef CVOutput_h
#define CVOutput_h

#include "DAC7565.h"

#include "Arduino.h"
#include "Output.h"
#include "Quantizer.h"
#include "Oscillator.h"

#define RETRIGGERMS 5

// 50 microseconds per millisecond - 1000 / 50

struct SlewSteps {
  int Steps = 0; 
  long Delta = 0;
};

class CVOutput : public Output
{
  public:
  
    CVOutput(int output, int led, DAC& dac, int samplingRate);

    // audio-rate update methid
    void Update();

    void SetValue(int value);
    void TargetValue(int value);
    void SetSlew(int slew, short format);
    void SetOffset(int value);

    // quantization
    void SetQuantizationScale(int scale);
    void SetQuantizedValue(int value);
    void TargetQuantizedValue(int value);
    void SetNote(int note);
    void TargetNote(int note);

    // OSC/LFO
    void SetFrequency(int freq);
    void TargetFrequency(int freq);
    void SetVOct(int value);
    void TargetVOct(int value);
    void SetLFO(int millihertz);
    void TargetLFO(int millihertz);
    void SetWaveform(int wave);
    void SetWidth(int width);
    void SetRectify(int mode);
    void Sync();
    void SetFrequencySlew(int slew, short format);

    void SetOscQuantizationScale(int scale);
    void SetQuantizedVOct(int value);
    void TargetQuantizedVOct(int value);
    void SetOscNote(int note);
    void TargetOscNote(int note);

    // Envelope Generator
    void SetAttack(int att, short format);
    void SetDecay(int dec, short format);
    void SetEnvelopeMode(int mode);
    void TriggerEnvelope();

    // reset
    void Reset();
    
    // overidden implementation
    void SetTimeFormat(int format);

    // virtual implementations
    void Kill();
    void UpdateLED();
    
  protected:
    
  private:

    int _samplingRate = 20000;
    int _krate = 20;

    // int _intcurrent = 0;
    
    volatile long _current = 0;
    long _target = 0;
    long _tempTarget = 0;

    int _smallCurrent = 0;
    
    bool _set = false;
    
    SlewSteps _slew;
    // 1ms is the teletypes default value for slew time
    unsigned long _slewTime = 1;
    
    float _tempMS = 0.;
    
    int _offset = 0;
    long _lOffset = 0;

    int _ledHelper;
    volatile bool _updateLED = false;
    
    int _cvHelper;

    DAC _dac;

    void UpdateDAC(int value);
    void CalculateSlewValue();
    SlewSteps CalculateRawSlew(long value, long target, long current);
    int Constrain(int value);

    Quantizer *_quantizer;

    Quantizer *_oscQuantizer;

    Oscillator *_oscillator;
    bool _oscilMode = false;

    int const _peak = DAC_MAX_SCALE - 32769;

    unsigned long _attack = 12;
    unsigned long _decay = 250;

    SlewSteps _attackSlew;
    SlewSteps _decaySlew;
    SlewSteps _retriggerSlew;
    
    bool _envelopeMode = false;
    long _envTarget = 0;
    bool _envelopeActive = false;
    bool _decaying = false;
    bool _retrigger = false;
};

#endif

