/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#include "Arduino.h"
#include "CVOutput.h"
#include "TxHelper.h"
#include "DAC7565.h"

/*
 * Constructor for Setting up the Output
 */
CVOutput::CVOutput(int output, int led, DAC& dac, int samplingRate) : Output(output, led){
  // store the DAC reference
  _dac = dac;
  // sampling rate
  _samplingRate = samplingRate;
  _krate = _samplingRate / 1000;
  // initialize the Quantizers
  _quantizer = new Quantizer(0);
  _oscQuantizer = new Quantizer(0);
  // initialize the oscillator
  _oscillator = new Oscillator(_samplingRate);
}

/*
 * Sets the Value without Slew
 */
void CVOutput::SetValue(int value){
  // target is the delivered value plus the configured offset
  _tempTarget = Constrain(value + _offset) << 15;
  if (_envelopeMode){
    if (_envTarget != _tempTarget) {
      _envTarget = _tempTarget;
      RecomputeEnvelopes();
    }
  } else {
    _target = _tempTarget;
    // this boolean tells the update method to skip slewing
    _set = true;
  }
}

/*
 * Sets the Target to Slew to
 */
void CVOutput::TargetValue(int value){
  // target is the delivered value plus the configured offset
  _tempTarget = Constrain(value + _offset) << 15;
  if (_envelopeMode){
    if (_envTarget != _tempTarget) {
      _envTarget = _tempTarget;
      RecomputeEnvelopes();
    }
  } else {
    _target = _tempTarget;
    // false indicates that we want it to slew
    _set = false;
    // calculate the new slew increment based on the new value
    CalculateSlewValue();
  }

}

/*
 * Sets the Slew Time (using the protected _time variable)
 */
void CVOutput::SetSlew(int value, short format){
  // create the slew value from the passed integer in several formats (0=ms; 1=sec; 2=min)
  _slewTime = TxHelper::ConvertMs(value, format);
  CalculateSlewValue();  
  // ensure that we are slewing to our destination
  _set = false;
}

/*
 * Store a New Offset Value
 */
void CVOutput::SetOffset(int value){
  // neutralize old offset and add new offset to target
  _tempTarget = Constrain((_target >> 15) - _offset + value) << 15;
  // store the new offset
  _offset = value;
  _lOffset = _offset << 15;
  
  if (_envelopeMode){
    if (_envTarget != _tempTarget) {
      _envTarget = _tempTarget;
      RecomputeEnvelopes();
    }
  } else {
    _target = _tempTarget;
     // if slewing - calculate a new slew value
    if (_slewTime != 0) CalculateSlewValue();
  }

}

/*
 * Stop Slew Activity and Jump to Target
 */
void CVOutput::Kill(){
  // stop slewing
  _set = true;
}

/*
 * Reset the CV Output
 */
void CVOutput::Reset(){
  SetOffset(0);
  SetValue(0);
  SetSlew(0,0);
  SetQuantizationScale(0);
  
  SetFrequency(0);
  SetOscQuantizationScale(0);
  SetWaveform(0);
  SetPhaseOffset(0);
  SetRectify(0);
  SetWidth(50);
  SetFrequencySlew(0, 0);
  
  SetEnvelopeMode(0);
  SetAttack(12, 0);
  SetDecay(250, 0);
  
}

/*
 * Sets Output Quantization Mode for the CV Outputs
 */
void CVOutput::SetQuantizationScale(int scale){
    _quantizer->SetScale(scale);
}

/*
 * Quantizes and sets a value to the current scale
 */
void CVOutput::SetQuantizedValue(int note){
  int16_t neg = note < 0 ? -1 : 1;
  SetValue((neg * _quantizer->Quantize(abs(note)).Value) << 1);
}

/*
 * Quantizes and targets a value (with slew) to the current scale
 */
void CVOutput::TargetQuantizedValue(int note){
  int16_t neg = note < 0 ? -1 : 1;
  TargetValue((neg * _quantizer->Quantize(abs(note)).Value) << 1);
}

/*
 * Sets a CV Value by Note Number
 * (against the active Quantization Scale)
 */
void CVOutput::SetNote(int note){
  int16_t neg = note < 0 ? -1 : 1;
    SetValue((neg * (int)_quantizer->GetValueForNote(abs(note))) << 1);
}

/*
 * Sets a CV Value by Note Number
 * (against the active Quantization Scale)
 */
void CVOutput::TargetNote(int note){
  int16_t neg = note < 0 ? -1 : 1;
  TargetValue((neg * (int)_quantizer->GetValueForNote(abs(note))) <<  1);
}

/*
 * Sets the Format for the Slew Time Value
 */
void CVOutput::SetTimeFormat(int format){
  // call the base class
  // Output::SetTimeFormat(format);
}

/*
 * Sets the oscillation frequency in Hz
 */
void CVOutput::SetFrequency(int freq){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = freq > 0;
  
  if (_oscilMode)
    _oscillator->SetFrequency(freq);
  else
    _set = true;
}

/*
 * Targets the oscillation frequency in Hz (for slew)
 */
void CVOutput::TargetFrequency(int freq){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = freq > 0;
  
  if (_oscilMode)
    _oscillator->TargetFrequency(freq);
  else
    _set = true;
}

/*
 * Sets the oscillation frequency using the TT integer value
 * using the current quantizer scale
 */
void CVOutput::SetQuantizedVOct(int value){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->SetFloatFrequency(_quantizer->Quantize(value).Frequency);
  else
    _set = true;
  
}

/*
 * Targets the oscillation frequencty using the TT integer value
 * using the current quantizer scale
 */
void CVOutput::TargetQuantizedVOct(int value){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->TargetFloatFrequency(_quantizer->Quantize(value).Frequency);
  else
    _set = true;
}

/*
 * Sets the slew amount for the frequency (portamento) in the supplied format
 */
void CVOutput::SetFrequencySlew(int slew, short format){
  _oscillator->SetPortamentoMs(TxHelper::ConvertMs(slew, format));
}

/*
 * Sets the oscillation frequency using the TT integer value
 */
void CVOutput::SetVOct(int value){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->SetFloatFrequency(TxHelper::VOct2Frequency(value));
  else
    _set = true;
}

/*
 * Targets the oscillation frequency using the TT integer value
 */
void CVOutput::TargetVOct(int value){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  if (_oscilMode)
    _oscillator->TargetFloatFrequency(TxHelper::VOct2Frequency(value));
  else
    _set = true;
}

/*
 * Sets the oscillation frequency via note number 
 * (against the current quantized scale)
 */
void CVOutput::SetOscNote(int note){
   _oscilMode = true;
   _oscillator->SetFloatFrequency((_oscQuantizer->GetFrequencyForNote(note)));
}

/*
 * Targets the oscillation frequency via note number 
 * (against the current quantized scale)
 */
void CVOutput::TargetOscNote(int note){
   _oscilMode = true;
   _oscillator->TargetFloatFrequency((_oscQuantizer->GetFrequencyForNote(note)));
}

/*
 * Sets the duration of a single cycle
 */
void CVOutput::SetCycle(int value, short format){
   _oscilMode = true;
   value = TxHelper::ConvertMs(value, format);
   _oscillator->SetFloatFrequency(1000. / value);
}

/*
 * Targets the duration of a single cycle
 */
void CVOutput::TargetCycle(int value, short format){
   _oscilMode = true;
   value = TxHelper::ConvertMs(value, format);
   _oscillator->TargetFloatFrequency(1000. / value);
}

/*
 * Sets the pulse width of the square wave waveform
 */
void CVOutput::SetWidth(int width){
  _oscillator->SetWidth(width);
}

/*
 * Sets the rectification mode (-2 to + 2)
 * 0 = No Rectification
 * -1/+1 = Partial Rectification (lops off values on the other side of zero)
 * -2/+2 = Full Rectification (does an ABS on the waveform and forces polarity)
 */
void CVOutput::SetRectify(int mode){
  _oscillator->SetRectify(mode);
}

/*
 * Set the oscillator frequency in Millihertz (1 Hz = .001 mHz)
 */
void CVOutput::SetLFO(int millihertz){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = millihertz > 0;
  
  if (_oscilMode)
    _oscillator->SetLFO(millihertz);
  else
    _set = true;
}

/*
 * Target the oscillator frequency in Millihertz (1 Hz = .001 mHz)
 */
void CVOutput::TargetLFO(int millihertz){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = millihertz > 0;
  
  if (_oscilMode)
    _oscillator->TargetLFO(millihertz);
  else
    _set = true;
}

/*
 * Resets the phase of the oscillator
 */
void CVOutput::Sync(){
  if (_oscilMode)
    _oscillator->ResetPhase(0);
  else
    _oscillator->ResetPhase(_target);
}

/*
 * Set the oscillator phase offset
 */
void CVOutput::SetPhaseOffset(int phase){
  _oscillator->SetPhaseOffset(phase);
}

/*
 * Selects the oscillator waveform
 * 0    = Sine
 * 1000 = Triangle
 * 2000 = Saw
 * 3000 = Square
 * 4000 = Noise / Sample-and-Hold
 */
void CVOutput::SetWaveform(int wave){
  _oscillator->SetWaveform(wave);
}

/*
 * Sets the quantization scale based on the included scales 
 * (see the Quantizer for the list)
 */
void CVOutput::SetOscQuantizationScale(int scale){
  _oscQuantizer->SetScale(scale);
}

/*
 * Sets the attack rate for the envelope generator
 */
void CVOutput::SetAttack(int att, short format){
  _attack = TxHelper::ConvertMs(max(att, 1), format);
  _attackSlew = CalculateRawSlew(_attack, _envTarget, _lOffset);
}

/*
 * Sets the decay rate for the envelope generator
 */
void CVOutput::SetDecay(int dec, short format){
  _decay = TxHelper::ConvertMs(max(dec, 1), format);
  _decaySlew = CalculateRawSlew(_decay, _lOffset, _envTarget);
}

/*
 * Turns envelopes on (1) and off (0) and initializes them
 */
void CVOutput::SetEnvelopeMode(int mode){

  // thanks to @scanner_darkly for suggesting this bugfix
  // only want to set the targets if the envelope mode has changed
  bool eMode = mode != 0;
  if (eMode != _envelopeMode){
  
    _envelopeMode = eMode;
    
    if (_envelopeMode){
    
      _envTarget = _target;
      _target = _lOffset;
      RecomputeEnvelopes();
    
    } else {
    
      _target = _envTarget;
    
    }
    
    _set = true;
  }
}

/*
 * Recomputes the Envlope Values
 */
void CVOutput::RecomputeEnvelopes(){
      SetAttack(_attack, 0);
      SetDecay(_decay, 0);
}

/*
 * Triggers or Retriggers the current envelope
 */
void CVOutput::TriggerEnvelope(){

  if (_envelopeMode) {

    if (_decaying) {
      
      // retrigger the envelope by going to zero/offset first
      _slew = CalculateRawSlew(RETRIGGERMS, _lOffset, _current);
      _target = _lOffset;
      _retrigger = true;
      
    } else {
            
      _current = _lOffset;
      _target = _envTarget;
      _slew = _attackSlew;

    }
    _envelopeActive = true;
  }
  
}
   
/*
 * The Update Function to Fulfil the Virtual Requirement
 * Keep it Lean - You don't have Much CPU
 */
void FASTRUN CVOutput::Update() {

  if (_set || _slew.Steps == 1){

    _smallCurrent = _target >> 15;
    
    // set the CV directly (skipping any slew behavior)
    UpdateDAC(_smallCurrent);
    _updateLED = true;

    if (_envelopeActive){

      if (_retrigger) {
        
         // do the attack
        _current = _lOffset;
        _target = _envTarget;
        _slew = _attackSlew;
        _retrigger = false;
        
      } else {
        
        // do the decay
        _envelopeActive = false;
        _target = _lOffset;
        _slew = _decaySlew;
        _decaying = true;
        
      }
      
    } else {
      
      // set the current to the target and turn off the set boolean
      _current = _target;
      _set = false; 
      _slew.Steps = 0;  
      _decaying = false;
            
    }

    _smallCurrent = _current >> 15;
    
  } else if (_slew.Steps > 1){
    
    _slew.Steps--;
    _current += _slew.Delta;

    _smallCurrent = _current >> 15;
    
     // update the DAC
    UpdateDAC(_smallCurrent);
    _updateLED = true;
    
  } else if (_oscilMode) { 
    
    // just update the dac
    UpdateDAC(_smallCurrent);

  }

}

/*
 * Update the DAC
 * Keep it Very Lean - Hardly Any CPU to Spare!
 */
void FASTRUN CVOutput::UpdateDAC(int value){

  // invert for DAC circuit
  if (_oscilMode)
    value = (int)(value * (_oscillator->Oscillate() / 32768.));

  // added the conditional write only if the CV value changes
  if (value != _cvHelper){
    _cvHelper = value;
    _dac.writeChannel(_output, (32767 - _cvHelper));
  }  
  
}

/*
 * Update the LED (runs at a slower rate than the DAC)
 */
void CVOutput::UpdateLED() {
  // update the LED if changed OR the frequency rate is < 1 Hz
  if (_updateLED || (_oscilMode && _oscillator->GetFrequency() <= 1)){
    _updateLED = false;
    // calculate the LED value and update it
    _ledHelper = _oscilMode ? abs(_cvHelper) >> 7 : abs(_current) >> 22;
    _ledHelper = constrain(_ledHelper, 0, 255);
    analogWrite(_led, _ledMap[_ledHelper]);
  }
}

/*
 * Calculate the Slew Value (for increments)
 */
void CVOutput::CalculateSlewValue(){
  _slew = CalculateRawSlew(_slewTime, _target, _current);
}

/*
 * Calculate the Slew Value from Raw MS for the value
 */
SlewSteps CVOutput::CalculateRawSlew(long value, long target, long current){
  SlewSteps ret;
  // split here so we don't divide by zero
  if (value == 0 || target == current){
    // if slew is zero - we just set the slew increment to the value we want to traverse
    ret.Steps = 1;
    ret.Delta = target - current;
  } else {
    // caculate the slew increment value
    ret.Steps = value * _krate;
    ret.Delta = (target - current) / ret.Steps;
  }
  return ret;
}

/*
 * Constrain the values to 16-bit Integer Range
 */
int CVOutput::Constrain(int value){
  return constrain(value, -32768, 32767);
}


int CVOutput::_ledMap[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,
0x4,0x4,0x4,0x4,0x4,0x5,0x5,0x5,0x5,0x5,0x6,0x6,0x6,0x6,0x7,0x7,
0x7,0x8,0x8,0x8,0x9,0x9,0x9,0xa,0xa,0xa,0xb,0xb,0xb,0xc,0xc,0xd,
0xd,0xd,0xe,0xe,0xf,0xf,0x10,0x10,0x11,0x11,0x12,0x12,0x13,0x13,0x14,0x14,
0x15,0x16,0x16,0x17,0x17,0x18,0x19,0x19,0x1a,0x1b,0x1b,0x1c,0x1d,0x1d,0x1e,0x1f,
0x20,0x20,0x21,0x22,0x23,0x23,0x24,0x25,0x26,0x27,0x28,0x28,0x29,0x2a,0x2b,0x2c,
0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3b,0x3c,0x3d,
0x3e,0x3f,0x40,0x42,0x43,0x44,0x45,0x47,0x48,0x49,0x4a,0x4c,0x4d,0x4f,0x50,0x51,
0x53,0x54,0x56,0x57,0x58,0x5a,0x5b,0x5d,0x5f,0x60,0x62,0x63,0x65,0x67,0x68,0x6a,
0x6c,0x6d,0x6f,0x71,0x72,0x74,0x76,0x78,0x7a,0x7b,0x7d,0x7f,0x81,0x83,0x85,0x87,
0x89,0x8b,0x8d,0x8f,0x91,0x93,0x95,0x97,0x99,0x9b,0x9e,0xa0,0xa2,0xa4,0xa6,0xa9,
0xab,0xad,0xb0,0xb2,0xb4,0xb7,0xb9,0xbc,0xbe,0xc1,0xc3,0xc6,0xc8,0xcb,0xcd,0xd0,
0xd2,0xd5,0xd8,0xda,0xdd,0xe0,0xe3,0xe5,0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd
};
