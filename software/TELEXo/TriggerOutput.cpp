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

void TriggerOutput::SetTime(int value, short format){
  _pulseTime = TxHelper::ConvertMs(value, format);
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
 * Stop All Pulses
 */
void TriggerOutput::Kill(){
  _toggle = MAXTIME;
}

/*
 * Update Function (Call This a Lot)
 */
void FASTRUN TriggerOutput::Update(unsigned long currentTime){
  if (currentTime >= _toggle) {
    if (_state == _polarity)
      SetState(!_polarity);
    _toggle = MAXTIME;
  }
}




