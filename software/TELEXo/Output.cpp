/*
 * TELEXo Eurorack Module
 * (c) 2016, 2017 Brendon Cassidy
 * MIT License
 */
 
#include "Arduino.h"
#include "Output.h"

/*
 * Initialize and Output and its LED 
 */
Output::Output(int output, int led){
  
  // store the variables
  _output = output; 
  _led = led;

}

