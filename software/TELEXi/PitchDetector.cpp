/*
 * TELEXi Eurorack Module
 * (c) 2016-2017 Brendon Cassidy
 * MIT License
 */
 
#include "Arduino.h"
#include "PitchDetector.h"

void PitchDetector::LoadSample(int sample){

  _frameBuffer[_buffer ? 0 : 1][_location++] = sample;

  if (_location >= FRAMESIZE){
    _buffer = !_buffer;
    _location = 0;
    AnalyzeBuffer();
  }
  
}

void PitchDetector::AnalyzeBuffer(){

  int buffNum = !_buffer ? 0 : 1;

  long sum = 0;
  long pastSum = 0;
  long sumMinusPastSum = 0;
  int state = 0;
  int thresh = 0;
  float freq = 0.;
  int period = 0;

  for (int i = 0; i < FRAMESIZE; i++){

    pastSum = sum;
    sum = 0;

    for (int q=0; q < FRAMESIZE - i; q++)
      sum += (_frameBuffer[buffNum][q] * _frameBuffer[buffNum][q+i]) / 16384;

    sumMinusPastSum = sum - pastSum;

    if (state == 2 && sumMinusPastSum <= 0) {
      period = i;
      state = 3;
    }
    
    if (state == 1 && (sum > thresh) && sumMinusPastSum > 0)
      state = 2;
      
    if (!i) {
      thresh = sum * 0.5;
      state = 1;
    }   
    
  }

  if (thresh > 100) {
      _freq = SAMPLERATE / period;
      Serial.print(freq);
  }
  
 
}

