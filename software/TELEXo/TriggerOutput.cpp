/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#include "Arduino.h"
#include "TriggerOutput.h"
#include "TxHelper.h"

/*
 * Initialize a Trigger Output without an LED
 */
TriggerOutput::TriggerOutput(int output) : TriggerOutput(output, -1) { }

/*
 * Initialize a Trigger Output and its LED
 */
TriggerOutput::TriggerOutput(int output, int led) : Output(output, led) {
 
  // initialize the pins
  pinMode(_output, OUTPUT);
  pinMode(_led, OUTPUT);
}

/*
 * Set the State of the Trigger Output and Write it To the Output Pins
 */
void TriggerOutput::SetState(bool state){
  _state = state;
  digitalWrite(_output, _state ? HIGH : LOW);
  if (_led > -1) digitalWrite(_led, _state ? HIGH : LOW);
}

/*
 * Sets the duration of the trigger pulse in multiple time formats
 */
void TriggerOutput::SetTime(int value, short format){
  _widthMode = false;
  _pulseTime = TxHelper::ConvertMs(value, format);
}

/*
 * Sets the duration of the trigger pulse as a percentage of the metro interval
 */
void TriggerOutput::SetWidth(int value){
  _widthMode = true;
  _width = constrain(value, 0, 100);
  _pulseTime = _metroInterval * _width / 100.;
}

/*
 * Toggle the State of the Trigger Output
 */
void TriggerOutput::ToggleState(){
  _toggle = MAXTIME;
  SetState(!_state);
}

/*
 * Pulse the Trigger for the Time Interval
 */
void TriggerOutput::Pulse() {

  // implement the clock divider (if active)
  if (_divide) {
    if (++_counter >= _division)
      _counter = 0;
    else
      return; 
  }
  
  if (_state != _polarity)
    SetState(_polarity);
  _toggle = millis() + _pulseTime;
}

/*
 * Sets the Polarity of the Output (connected to pulse)
 */
void TriggerOutput::SetPolarity(bool polarity){
  _polarity = polarity;
}

/*
 * Sets the division value for the PULSE clock divider
 */
void TriggerOutput::SetDivision(int division){
  _division = division;
  _counter = _division;
  _divide = division > 1 ? true : false;
}

/**
 * Activates or Deactivates the Metro for this Trigger
 */
void TriggerOutput::SetMetro(int state){
  bool m = state != 0;
  if (m && !_metro) Sync();
  _metro = m;
}

/**
 * Sets the time for the metro pulse (in ms, sec, min, and bpm)
 */
void TriggerOutput::SetMetroTime(int value, short format){
  _metroInterval = TxHelper::ConvertMs(value, format);
  if (_widthMode) SetWidth(_width);
}

/**
 * Syncs the metro pulse to now
 */
void TriggerOutput::Sync(){
  _nextEvent = millis();
}


/*
 * Stop All Pulses
 */
void TriggerOutput::Kill(){
  _toggle = MAXTIME;
  _metro = false;
}

/*
 * Update Function (Call This a Lot)
 */
void FASTRUN TriggerOutput::Update(unsigned long currentTime){

  // turn off the pulse
  if (currentTime >= _toggle) {
    if (_state == _polarity)
      SetState(!_polarity);
    _toggle = MAXTIME;
  }

  // evaluate pinging the metro event
  if (_metro && currentTime >= _nextEvent){
    _nextEvent = currentTime + _metroInterval;
    Pulse();
  }
  
}




