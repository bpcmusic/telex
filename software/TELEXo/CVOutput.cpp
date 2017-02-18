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
 * Stop Slew Activity and Jump to Target
 */
void CVOutput::Reset(){
  SetFrequencySlew(0, 0);
  SetFrequency(0);
  SetQuantizationScale(0);
  SetOscQuantizationScale(0);
  SetEnvelopeMode(0);
  SetAttack(7, 0);
  SetDecay(500, 0);
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
  SetValue(_quantizer->Quantize(note).Value << 1);
}

/*
 * Quantizes and targets a value (with slew) to the current scale
 */
void CVOutput::TargetQuantizedValue(int note){
  TargetValue(_quantizer->Quantize(note).Value << 1);
}

/*
 * Sets a CV Value by Note Number
 * (against the active Quantization Scale)
 */
void CVOutput::SetNote(int note){
    SetValue((int)_quantizer->GetValueForNote(note) << 1);
}

/*
 * Sets a CV Value by Note Number
 * (against the active Quantization Scale)
 */
void CVOutput::TargetNote(int note){
  TargetValue((int)_quantizer->GetValueForNote(note) <<  1);
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
 * 0 = Sine
 * 1 = Triangle
 * 2 = Saw
 * 3 = Square
 * 4 = Noise / Sample-and-Hold
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
  
  _cvHelper = value;
  
  _dac.writeChannel(_output, (32767 - _cvHelper));  
  
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
    analogWrite(_led, _ledHelper);
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

