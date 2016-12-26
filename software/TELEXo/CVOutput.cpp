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
    _envTarget = _tempTarget;
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
    _envTarget = _tempTarget;
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
    _envTarget = _tempTarget;
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

void CVOutput::SetFrequency(int freq){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = freq > 0;
  
  if (_oscilMode)
    _oscillator->SetFrequency(freq);
  else
    _set = true;
}

void CVOutput::TargetFrequency(int freq){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = freq > 0;
  
  if (_oscilMode)
    _oscillator->TargetFrequency(freq);
  else
    _set = true;
}

void CVOutput::SetQuantizedVOct(int value){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->SetFloatFrequency(_quantizer->Quantize(value).Frequency);
  else
    _set = true;
  
}

void CVOutput::TargetQuantizedVOct(int value){

  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->TargetFloatFrequency(_quantizer->Quantize(value).Frequency);
  else
    _set = true;
}


void CVOutput::SetFrequencySlew(int slew, short format){
  Serial.printf("MS: %d", TxHelper::ConvertMs(slew, format));
  _oscillator->SetPortamentoMs(TxHelper::ConvertMs(slew, format));
}


void CVOutput::SetVOct(int value){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  
  if (_oscilMode)
    _oscillator->SetFloatFrequency(TxHelper::VOct2Frequency(value));
  else
    _set = true;
}

void CVOutput::TargetVOct(int value){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = value > 0;
  if (_oscilMode)
    _oscillator->TargetFloatFrequency(TxHelper::VOct2Frequency(value));
  else
    _set = true;
}

void CVOutput::SetOscNote(int note){
   _oscilMode = true;
   _oscillator->SetFloatFrequency((_oscQuantizer->GetFrequencyForNote(note)));
}

void CVOutput::TargetOscNote(int note){
   _oscilMode = true;
   _oscillator->TargetFloatFrequency((_oscQuantizer->GetFrequencyForNote(note)));
}

void CVOutput::SetWidth(int width){
  _oscillator->SetWidth(width);
}

void CVOutput::SetRectify(int mode){
  _oscillator->SetRectify(mode);
}

void CVOutput::SetLFO(int millihertz){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = millihertz > 0;
  
  if (_oscilMode)
    _oscillator->SetLFO(millihertz);
  else
    _set = true;
}

void CVOutput::TargetLFO(int millihertz){
  if (!_oscilMode)
    _oscillator->ResetPhase(_target);
    
  _oscilMode = millihertz > 0;
  
  if (_oscilMode)
    _oscillator->TargetLFO(millihertz);
  else
    _set = true;
}

void CVOutput::Sync(){
  if (_oscilMode)
    _oscillator->ResetPhase(0);
  else
    _oscillator->ResetPhase(_target);
}

void CVOutput::SetWaveform(int wave){
  _oscillator->SetTable(wave);
}

void CVOutput::SetOscQuantizationScale(int scale){
  _oscQuantizer->SetScale(scale);
}


void CVOutput::SetAttack(int att, short format){
  _attack = TxHelper::ConvertMs(max(att, 1), format);
  _attackSlew = CalculateRawSlew(_attack, _envTarget, _lOffset);
}

void CVOutput::SetDecay(int dec, short format){
  _decay = TxHelper::ConvertMs(max(dec, 1), format);
  _decaySlew = CalculateRawSlew(_decay, _lOffset, _envTarget);
}

void CVOutput::SetEnvelopeMode(int mode){
  
  _envelopeMode = mode != 0;
  
  if (_envelopeMode){
    
    _envTarget = _target;
    _target = _lOffset;
    SetAttack(_attack, 0);
    SetDecay(_decay, 0);
    
  } else {
    
    _target = _envTarget;
    
  }
  
  _set = true;
}

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

    // just update the dac (for oscil - no LED change)
    UpdateDAC(_smallCurrent);
    
  }

}


/*
 * Update the DAC
 */
void FASTRUN CVOutput::UpdateDAC(int value){

  // invert for DAC circuit
  if (_oscilMode)
    value = (int)(value * (_oscillator->Oscillate() / 32768.));
  
  _cvHelper = 32767 - value;
  
  _dac.writeChannel(_output, _cvHelper);  
  
}

/*
 * Update the LED (runs at a slower rate than the DAC)
 */
void CVOutput::UpdateLED() {
  if (_updateLED){
    _updateLED = false;
    // calculate the LED value and update it
    _ledHelper = abs(_current) >> 22;
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

