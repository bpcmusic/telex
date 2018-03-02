/*
 * TELEXo Eurorack Module
 * (c) 2016, 2017 Brendon Cassidy
 * MIT License
 */
 
#ifndef CVOutput_h
#define CVOutput_h

#include "DAC7565.h"

#include "Arduino.h"
#include "Output.h"
#include "Quantizer.h"
#include "Oscillator.h"
#include "TriggerOutput.h"

#define RETRIGGERMS 5
#define DACCENTER 32767

// 50 microseconds per millisecond - 1000 / 50

struct SlewSteps {
  long Duration = 0;
  int Steps = 0; 
  long Delta = 0;
};

class CVOutput : public Output
{
  public:
  
    CVOutput(int output, int led, DAC& dac);

    void ReferenceTriggers(TriggerOutput (*triggerOutputs[]), int count);

    // audio-rate update methid
    void Update();

    void SetValue(int value);
    void TargetValue(int value);
    void SetSlew(int slew, short format);
    void SetOffset(int value);
    void SetLog(int value);

    int Calibrate();
    void ResetCalibration();
    void SetCalibrationValue(int value);

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
    void SetPhaseOffset(int phase);
    void SetFrequencySlew(int slew, short format);
    void SetCycle(int value, short format);
    void TargetCycle(int value, short format);
    void SetCenter(int value);

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
    void SetLoop(int loopEnv);

    void SetEOR(int trNumber);
    void SetEOC(int trNumber);

    void SetENV(int value);

    // reset
    void Reset();
    
    // overidden implementation
    void SetTimeFormat(int format);

    // virtual implementations
    void Kill();
    void UpdateLED();
    
  protected:

    static const uint8_t _ledMap[256];
    
  private:

    void RecomputeEnvelopes();
    
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

    int _calibration = 0;

    int _zero = 0;
    long _lZero = 0;

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

    void SharedOscil(int value);

    int _dacCenter = DACCENTER;
    int _oscilCenter = 0;

    int const _peak = DAC_MAX_SCALE - 32769;

    unsigned long _attack = 12;
    unsigned long _decay = 250;

    SlewSteps _attackSlew;
    SlewSteps _decaySlew;
    SlewSteps _retriggerSlew;

    bool _envelopeState = false;
    bool _envelopeMode = false;
    long _envTarget = 0;
    bool _envelopeActive = false;
    bool _decaying = false;
    bool _retrigger = false;
    bool _envLoop = false;
    bool _infLoop = false;
    int _loopTimes = -1;
    int _loopCount = 0;

    bool _peakLED = false;

    TriggerOutput **_triggerOutputs;
    int _triggerOutputCount = 0;

    bool _triggerEOR = false;
    int _triggerForEOR = -1;
    bool _triggerEOC = false;
    int _triggerForEOC = -1;

    bool _doLog = false;
    uint8_t _logRange = 1;
    bool _wasNg = false;
};

#endif

