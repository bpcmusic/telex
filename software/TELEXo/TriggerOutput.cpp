/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#include "Arduino.h"
#include "TriggerOutput.h"
#include "TxHelper.h"

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
  digitalWrite(_led, _state ? HIGH : LOW);
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

  // jump out if pulses are muted
  if (_mutePulse) return;

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
  if (m){  
    _actualCount = _metroCount;
    if (!_metro) Sync();
  }
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
 * Sets the number of trigger repeats on an M event (0 = inf)
 */
void TriggerOutput::SetMetroCount(int value){
  _metroCount = value;
  _actualCount = value;
}

/*
 * Mute (true) or unmute (false) the PULSE command
 */
void TriggerOutput::SetMute(bool state){
  _mutePulse = state;
}

/**
 * Syncs the metro pulse to now
 */
void TriggerOutput::Sync(){
  _nextEvent = millis();
}


/**
 * Resets the trigger output
 */
void TriggerOutput::Reset(){
  SetPolarity(true);
  SetState(false);
  SetTime(100, 0);
  SetDivision(1);
  SetMetro(0);
  SetMetroTime(1000,0);
  SetMetroCount(0);
  SetMute(false);
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
    if (_metroCount == 0 || (_metroCount > 0 && --_actualCount > 0))
      _nextEvent = _nextEvent + _metroInterval;
    else
      _metro = false;
    Pulse();
  }
  
}




